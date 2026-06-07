# 🏥 Implementasi Database Sistem Klinik — Kode Lengkap

> Seluruh SQL siap dijalankan di PostgreSQL. Tidak ada komentar di dalam blok kode.
> Penjelasan diberikan **setelah** setiap blok.

---

## 1. DDL — Tabel Master & Operasional

```sql
DROP TABLE IF EXISTS
    Log_Pembayaran, Pembayaran, Pendaftaran_Layanan, Pendaftaran,
    Rujukan, Skrining, Pasien, Layanan, Petugas_Admin,
    Jadwal_Dokter, Dokter, Poli CASCADE;

CREATE TABLE Poli (
    id_poli       SERIAL PRIMARY KEY,
    nama_poli     VARCHAR(100) NOT NULL,
    nomor_ruangan VARCHAR(20)  NOT NULL
);

CREATE TABLE Layanan (
    id_layanan          SERIAL PRIMARY KEY,
    nama_layanan        VARCHAR(150)   NOT NULL,
    tarif_total_layanan NUMERIC(15, 2) NOT NULL DEFAULT 0
);

CREATE TABLE Petugas_Admin (
    id_admin      SERIAL PRIMARY KEY,
    nama_petugas  VARCHAR(100) NOT NULL,
    shift_tugas   VARCHAR(20)  CHECK (shift_tugas IN ('Pagi', 'Siang', 'Malam'))
);

CREATE TABLE Pasien (
    id_pasien       SERIAL PRIMARY KEY,
    nim_nik         VARCHAR(20)  UNIQUE NOT NULL,
    nama            VARCHAR(150) NOT NULL,
    asal_faskes     VARCHAR(100),
    no_telp         VARCHAR(20),
    kategori_pasien VARCHAR(50)  CHECK (kategori_pasien IN ('Umum', 'BPJS', 'Asuransi Lain'))
);

CREATE TABLE Dokter (
    id_dokter   SERIAL PRIMARY KEY,
    id_poli     INT          NOT NULL REFERENCES Poli(id_poli) ON DELETE RESTRICT,
    nama_dokter VARCHAR(150) NOT NULL
);

CREATE TABLE Jadwal_Dokter (
    id_jadwal     SERIAL PRIMARY KEY,
    id_dokter     INT     NOT NULL REFERENCES Dokter(id_dokter) ON DELETE CASCADE,
    hari_tanggal  DATE    NOT NULL,
    jam_mulai     TIME    NOT NULL,
    jam_selesai   TIME    NOT NULL,
    status_jadwal VARCHAR(20) DEFAULT 'Tersedia'
        CHECK (status_jadwal IN ('Tersedia', 'Penuh', 'Batal'))
);

CREATE TABLE Skrining (
    id_skrining       SERIAL PRIMARY KEY,
    id_pasien         INT       NOT NULL REFERENCES Pasien(id_pasien) ON DELETE CASCADE,
    tanggal_skrining  TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    hasil_skrinning   TEXT,
    status_kelayakan  VARCHAR(50)
        CHECK (status_kelayakan IN ('Layak Ditangani', 'Perlu Rujukan', 'Kritis'))
);

CREATE TABLE Rujukan (
    id_rujukan      SERIAL PRIMARY KEY,
    id_skrining     INT  UNIQUE NOT NULL REFERENCES Skrining(id_skrining) ON DELETE CASCADE,
    alasan_rujukan  TEXT NOT NULL,
    tanggal_rujukan TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    asal_rujukan    VARCHAR(100),
    tujuan_rujukan  VARCHAR(150) NOT NULL
);

CREATE TABLE Pendaftaran (
    id_daftar           SERIAL PRIMARY KEY,
    id_pasien           INT NOT NULL REFERENCES Pasien(id_pasien) ON DELETE RESTRICT,
    id_jadwal           INT NOT NULL REFERENCES Jadwal_Dokter(id_jadwal) ON DELETE RESTRICT,
    id_admin            INT NOT NULL REFERENCES Petugas_Admin(id_admin),
    tanggal_pendaftaran TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    status_pendaftaran  VARCHAR(20) DEFAULT 'Menunggu'
        CHECK (status_pendaftaran IN ('Menunggu', 'Diperiksa', 'Selesai', 'Batal')),
    nomor_antrean       INT
);

CREATE TABLE Pendaftaran_Layanan (
    id_daftar   INT NOT NULL REFERENCES Pendaftaran(id_daftar) ON DELETE CASCADE,
    id_layanan  INT NOT NULL REFERENCES Layanan(id_layanan) ON DELETE RESTRICT,
    PRIMARY KEY (id_daftar, id_layanan)
);

CREATE TABLE Pembayaran (
    id_pembayaran SERIAL PRIMARY KEY,
    id_daftar     INT UNIQUE NOT NULL REFERENCES Pendaftaran(id_daftar) ON DELETE CASCADE,
    metode_bayar  VARCHAR(50)
        CHECK (metode_bayar IN ('Tunai', 'Transfer', 'E-Wallet', 'BPJS')),
    status_bayar  VARCHAR(20) DEFAULT 'Pending'
        CHECK (status_bayar IN ('Pending', 'Lunas', 'Gagal'))
);
```

**Penjelasan langkah demi langkah:**

1. **DROP TABLE CASCADE** — Hapus semua tabel lama (jika ada) agar script bisa dijalankan ulang tanpa error konflik.
2. **Poli** — Tabel master poli/klinik. Menyimpan nama poli dan nomor ruangan.
3. **Layanan** — Daftar layanan medis beserta tarif. Dipakai untuk kalkulasi tagihan.
4. **Petugas_Admin** — Data admin klinik. Field `shift_tugas` dibatasi 3 opsi via CHECK constraint.
5. **Pasien** — Data pasien dengan `nim_nik` sebagai identifier unik. `kategori_pasien` menentukan jalur pembayaran (Umum/BPJS/Asuransi Lain).
6. **Dokter** — Setiap dokter terikat ke satu Poli via foreign key. `ON DELETE RESTRICT` mencegah hapus poli yang masih punya dokter.
7. **Jadwal_Dokter** — Jadwal praktek per dokter per hari. Status `Tersedia/Penuh/Batal` dikontrol otomatis oleh trigger.
8. **Skrining** — Hasil pemeriksaan awal pasien. Status kelayakan menentukan apakah pasien dirujuk atau ditangani.
9. **Rujukan** — Satu skrining maksimal satu rujukan (UNIQUE constraint pada `id_skrining`).
10. **Pendaftaran** — Tabel transaksional utama. Menghubungkan pasien, jadwal, dan admin. Nomor antrean di-assign otomatis oleh trigger.
11. **Pendaftaran_Layanan** — Tabel pivot many-to-many antara Pendaftaran dan Layanan.
12. **Pembayaran** — Satu pendaftaran punya satu pembayaran (UNIQUE pada `id_daftar`). Status dan metode bayar divalidasi via CHECK dan trigger.

---

## 2. Tabel Audit Log

```sql
CREATE TABLE Log_Pembayaran (
    id_log          SERIAL PRIMARY KEY,
    id_pembayaran   INT         NOT NULL,
    id_daftar       INT         NOT NULL,
    status_lama     VARCHAR(20),
    status_baru     VARCHAR(20),
    metode_lama     VARCHAR(50),
    metode_baru     VARCHAR(50),
    waktu_perubahan TIMESTAMP   DEFAULT CURRENT_TIMESTAMP,
    keterangan      TEXT
);
```

**Penjelasan langkah demi langkah:**

1. Tabel ini menyimpan jejak setiap perubahan pada tabel `Pembayaran`.
2. Kolom `status_lama` dan `status_baru` merekam transisi status pembayaran.
3. Kolom `metode_lama` dan `metode_baru` merekam perubahan metode bayar.
4. `waktu_perubahan` otomatis terisi timestamp saat record dibuat.
5. Diisi otomatis oleh Trigger 8 (`trg_audit_perubahan_pembayaran`), bukan manual.

---

## 3. Functions

### 3.1 fn_Hitung_Total_Tagihan

```sql
CREATE OR REPLACE FUNCTION fn_Hitung_Total_Tagihan(p_id_daftar INT)
RETURNS NUMERIC(15, 2) AS $$
DECLARE
    v_total    NUMERIC(15, 2) := 0;
    v_kategori VARCHAR(50);
BEGIN
    SELECT p.kategori_pasien
    INTO   v_kategori
    FROM   Pendaftaran dft
    JOIN   Pasien p ON dft.id_pasien = p.id_pasien
    WHERE  dft.id_daftar = p_id_daftar;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'fn_Hitung_Total_Tagihan: Pendaftaran ID % tidak ditemukan.', p_id_daftar;
    END IF;

    IF v_kategori = 'BPJS' THEN
        RETURN 0.00;
    END IF;

    SELECT COALESCE(SUM(l.tarif_total_layanan), 0)
    INTO   v_total
    FROM   Pendaftaran_Layanan pl
    JOIN   Layanan l ON pl.id_layanan = l.id_layanan
    WHERE  pl.id_daftar = p_id_daftar;

    RETURN v_total;
END;
$$ LANGUAGE plpgsql;
```

**Penjelasan langkah demi langkah:**

1. Menerima `p_id_daftar` (ID pendaftaran) sebagai input.
2. JOIN `Pendaftaran` → `Pasien` untuk ambil kategori pasien.
3. Jika pendaftaran tidak ditemukan → raise exception.
4. Jika pasien BPJS → langsung return 0 (ditanggung BPJS).
5. Jika pasien non-BPJS → SUM semua tarif layanan dari tabel pivot `Pendaftaran_Layanan` JOIN `Layanan`.
6. COALESCE memastikan return 0 jika belum ada layanan terdaftar.

---

### 3.2 fn_Cek_Ketersediaan_Jadwal

```sql
CREATE OR REPLACE FUNCTION fn_Cek_Ketersediaan_Jadwal(p_id_jadwal INT)
RETURNS BOOLEAN AS $$
DECLARE
    v_status_jadwal VARCHAR(20);
    v_jumlah_aktif  INT;
    v_limit_antrean CONSTANT INT := 30;
BEGIN
    SELECT status_jadwal
    INTO   v_status_jadwal
    FROM   Jadwal_Dokter
    WHERE  id_jadwal = p_id_jadwal;

    IF NOT FOUND THEN
        RETURN FALSE;
    END IF;

    IF v_status_jadwal IN ('Penuh', 'Batal') THEN
        RETURN FALSE;
    END IF;

    SELECT COUNT(*)
    INTO   v_jumlah_aktif
    FROM   Pendaftaran
    WHERE  id_jadwal = p_id_jadwal
      AND  status_pendaftaran != 'Batal';

    RETURN v_jumlah_aktif < v_limit_antrean;
END;
$$ LANGUAGE plpgsql;
```

**Penjelasan langkah demi langkah:**

1. Menerima `p_id_jadwal` sebagai input, return `TRUE`/`FALSE`.
2. Ambil status jadwal dari `Jadwal_Dokter`.
3. Return FALSE jika jadwal tidak ditemukan.
4. Return FALSE jika status sudah `Penuh` atau `Batal`.
5. Hitung jumlah pendaftaran aktif (bukan Batal) pada jadwal tersebut.
6. Return TRUE hanya jika jumlah aktif masih di bawah batas 30 pasien.

---

### 3.3 fn_Hitung_Estimasi_Tunggu

```sql
CREATE OR REPLACE FUNCTION fn_Hitung_Estimasi_Tunggu(p_id_daftar INT)
RETURNS TIME AS $$
DECLARE
    v_nomor_antrean        INT;
    v_id_jadwal            INT;
    v_jam_mulai            TIME;
    v_menit_per_pasien     INT := 10;
    v_estimasi_jam_panggil TIME;
BEGIN
    SELECT nomor_antrean, id_jadwal
    INTO   v_nomor_antrean, v_id_jadwal
    FROM   Pendaftaran
    WHERE  id_daftar = p_id_daftar;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'fn_Hitung_Estimasi_Tunggu: Pendaftaran ID % tidak ditemukan.', p_id_daftar;
    END IF;

    SELECT jam_mulai
    INTO   v_jam_mulai
    FROM   Jadwal_Dokter
    WHERE  id_jadwal = v_id_jadwal;

    v_estimasi_jam_panggil := v_jam_mulai +
        ((v_nomor_antrean - 1) * v_menit_per_pasien || ' minutes')::INTERVAL;

    RETURN v_estimasi_jam_panggil;
END;
$$ LANGUAGE plpgsql;
```

**Penjelasan langkah demi langkah:**

1. Menerima `p_id_daftar`, return estimasi waktu dipanggil sebagai `TIME`.
2. Ambil `nomor_antrean` dan `id_jadwal` dari `Pendaftaran`.
3. Ambil `jam_mulai` praktek dokter dari `Jadwal_Dokter`.
4. Kalkulasi: `jam_mulai + (nomor_antrean - 1) × 10 menit`.
5. Contoh: antrean ke-5, jam mulai 09:00 → estimasi dipanggil 09:40.
6. Cast string interval ke tipe INTERVAL PostgreSQL untuk operasi aritmatika waktu.

---

### 3.4 fn_Validasi_Metode_Bayar

```sql
CREATE OR REPLACE FUNCTION fn_Validasi_Metode_Bayar(
    p_id_daftar   INT,
    p_metode_bayar VARCHAR(50)
)
RETURNS BOOLEAN AS $$
DECLARE
    v_kategori VARCHAR(50);
BEGIN
    SELECT p.kategori_pasien
    INTO   v_kategori
    FROM   Pendaftaran dft
    JOIN   Pasien p ON dft.id_pasien = p.id_pasien
    WHERE  dft.id_daftar = p_id_daftar;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'fn_Validasi_Metode_Bayar: Pendaftaran ID % tidak ditemukan.', p_id_daftar;
    END IF;

    IF v_kategori = 'BPJS' AND p_metode_bayar != 'BPJS' THEN
        RETURN FALSE;
    END IF;

    IF v_kategori != 'BPJS' AND p_metode_bayar = 'BPJS' THEN
        RETURN FALSE;
    END IF;

    RETURN TRUE;
END;
$$ LANGUAGE plpgsql;
```

**Penjelasan langkah demi langkah:**

1. Menerima `p_id_daftar` dan `p_metode_bayar`, return `BOOLEAN`.
2. Ambil kategori pasien melalui JOIN `Pendaftaran` → `Pasien`.
3. Rule 1: Pasien BPJS **wajib** bayar via metode BPJS → return FALSE jika tidak.
4. Rule 2: Pasien non-BPJS **tidak boleh** klaim BPJS → return FALSE jika mencoba.
5. Jika lolos kedua rule → return TRUE (metode bayar valid).

---

### 3.5 fn_Get_Statistik_Harian

```sql
CREATE OR REPLACE FUNCTION fn_Get_Statistik_Harian(p_tanggal DATE)
RETURNS TABLE (
    total_pendaftaran INT,
    total_selesai     INT,
    total_batal       INT,
    total_menunggu    INT,
    total_pendapatan  NUMERIC(15, 2),
    total_rujukan     INT
) AS $$
BEGIN
    RETURN QUERY
    SELECT
        COUNT(DISTINCT p.id_daftar)::INT,

        COUNT(DISTINCT p.id_daftar)
            FILTER (WHERE p.status_pendaftaran = 'Selesai')::INT,

        COUNT(DISTINCT p.id_daftar)
            FILTER (WHERE p.status_pendaftaran = 'Batal')::INT,

        COUNT(DISTINCT p.id_daftar)
            FILTER (WHERE p.status_pendaftaran IN ('Menunggu', 'Diperiksa'))::INT,

        COALESCE(SUM(l.tarif_total_layanan)
            FILTER (
                WHERE py.status_bayar = 'Lunas'
                  AND pas.kategori_pasien != 'BPJS'
            ), 0),

        COUNT(DISTINCT r.id_rujukan)::INT

    FROM   Pendaftaran p
    JOIN   Jadwal_Dokter jd  ON p.id_jadwal  = jd.id_jadwal
    JOIN   Pasien pas        ON p.id_pasien  = pas.id_pasien
    LEFT JOIN Pembayaran py  ON p.id_daftar  = py.id_daftar
    LEFT JOIN Pendaftaran_Layanan pl ON p.id_daftar  = pl.id_daftar
    LEFT JOIN Layanan l      ON pl.id_layanan = l.id_layanan
    LEFT JOIN Skrining sk    ON pas.id_pasien = sk.id_pasien
                             AND DATE(sk.tanggal_skrining) = p_tanggal
    LEFT JOIN Rujukan r      ON sk.id_skrining = r.id_skrining
    WHERE  jd.hari_tanggal = p_tanggal;
END;
$$ LANGUAGE plpgsql;
```

**Penjelasan langkah demi langkah:**

1. Menerima `p_tanggal` (DATE), return tabel dengan 6 kolom statistik.
2. `total_pendaftaran` — hitung semua pendaftaran unik pada tanggal tersebut.
3. `total_selesai` — filter hanya status 'Selesai' menggunakan FILTER clause PostgreSQL.
4. `total_batal` — filter status 'Batal'.
5. `total_menunggu` — gabungan status 'Menunggu' dan 'Diperiksa' (masih dalam proses).
6. `total_pendapatan` — SUM tarif layanan hanya dari pembayaran Lunas dan pasien non-BPJS.
7. `total_rujukan` — hitung rujukan unik yang terjadi pada tanggal tersebut.
8. Multi-JOIN menghubungkan Pendaftaran → Jadwal (filter tanggal) → Pasien → Pembayaran → Layanan → Skrining → Rujukan.

---

## 4. Stored Procedures

### 4.1 sp_Daftar_Pasien_Baru_Kompleks

```sql
CREATE OR REPLACE PROCEDURE sp_Daftar_Pasien_Baru_Kompleks(
    IN  p_nim_nik          VARCHAR(20),
    IN  p_nama             VARCHAR(150),
    IN  p_asal_faskes      VARCHAR(100),
    IN  p_no_telp          VARCHAR(20),
    IN  p_kategori_pasien  VARCHAR(50),
    IN  p_id_jadwal        INT,
    IN  p_id_admin         INT,
    IN  p_hasil_skrining   TEXT,
    IN  p_status_kelayakan VARCHAR(50),
    OUT p_id_daftar        INT,
    OUT p_pesan            VARCHAR(500)
)
LANGUAGE plpgsql AS $$
DECLARE
    v_id_pasien       INT;
    v_id_skrining     INT;
    v_jadwal_tersedia BOOLEAN;
BEGIN
    v_jadwal_tersedia := fn_Cek_Ketersediaan_Jadwal(p_id_jadwal);
    IF NOT v_jadwal_tersedia THEN
        RAISE EXCEPTION 'JADWAL_TIDAK_TERSEDIA: Jadwal ID % penuh atau sudah dibatalkan.', p_id_jadwal;
    END IF;

    SELECT id_pasien
    INTO   v_id_pasien
    FROM   Pasien
    WHERE  nim_nik = p_nim_nik;

    IF NOT FOUND THEN
        INSERT INTO Pasien (nim_nik, nama, asal_faskes, no_telp, kategori_pasien)
        VALUES (p_nim_nik, p_nama, p_asal_faskes, p_no_telp, p_kategori_pasien)
        RETURNING id_pasien INTO v_id_pasien;
    ELSE
        UPDATE Pasien
        SET    no_telp     = COALESCE(NULLIF(p_no_telp, ''), no_telp),
               asal_faskes = COALESCE(NULLIF(p_asal_faskes, ''), asal_faskes)
        WHERE  id_pasien   = v_id_pasien;
    END IF;

    INSERT INTO Skrining (id_pasien, hasil_skrinning, status_kelayakan)
    VALUES (v_id_pasien, p_hasil_skrining, p_status_kelayakan)
    RETURNING id_skrining INTO v_id_skrining;

    INSERT INTO Pendaftaran (id_pasien, id_jadwal, id_admin)
    VALUES (v_id_pasien, p_id_jadwal, p_id_admin)
    RETURNING id_daftar INTO p_id_daftar;

    p_pesan := FORMAT(
        'SUKSES: Pasien "%s" berhasil didaftarkan. ID Daftar: %s | ID Skrining: %s | Jadwal ID: %s.',
        p_nama, p_id_daftar, v_id_skrining, p_id_jadwal
    );

EXCEPTION
    WHEN OTHERS THEN
        RAISE;
END;
$$;
```

**Penjelasan langkah demi langkah:**

1. **Step 1 — Validasi jadwal**: Panggil `fn_Cek_Ketersediaan_Jadwal`. Jika FALSE → exception, seluruh operasi batal.
2. **Step 2 — Cek/buat pasien**: Cari pasien berdasarkan `nim_nik`. Jika tidak ditemukan → INSERT pasien baru. Jika sudah ada → UPDATE data kontak terkini (no_telp, asal_faskes) tanpa menimpa data lama yang kosong.
3. **Step 3 — INSERT Skrining**: Buat record skrining. Trigger `trg_buat_draft_rujukan` otomatis aktif jika status = 'Perlu Rujukan'.
4. **Step 4 — INSERT Pendaftaran**: Buat record pendaftaran. Trigger `trg_blokir_daftar_ke_jadwal_nonaktif` validasi jadwal, `trg_set_nomor_antrean` assign nomor antrean otomatis.
5. **Output**: Kembalikan `p_id_daftar` dan pesan sukses via parameter OUT.
6. **Exception handler**: Re-raise error agar transaksi pemanggil di-rollback.

---

### 4.2 sp_Pembatalan_Jadwal_Dokter

```sql
CREATE OR REPLACE PROCEDURE sp_Pembatalan_Jadwal_Dokter(
    IN  p_id_jadwal          INT,
    OUT p_jumlah_terpengaruh INT
)
LANGUAGE plpgsql AS $$
DECLARE
    r RECORD;
BEGIN
    p_jumlah_terpengaruh := 0;

    IF NOT EXISTS (
        SELECT 1 FROM Jadwal_Dokter
        WHERE  id_jadwal     = p_id_jadwal
          AND  status_jadwal NOT IN ('Batal')
    ) THEN
        RAISE EXCEPTION 'Jadwal ID % tidak ditemukan atau sudah berstatus Batal.', p_id_jadwal;
    END IF;

    UPDATE Jadwal_Dokter
    SET    status_jadwal = 'Batal'
    WHERE  id_jadwal     = p_id_jadwal;

    FOR r IN
        SELECT p.id_daftar, pas.no_telp, pas.nama
        FROM   Pendaftaran p
        JOIN   Pasien pas ON p.id_pasien = pas.id_pasien
        WHERE  p.id_jadwal          = p_id_jadwal
          AND  p.status_pendaftaran NOT IN ('Selesai', 'Batal')
    LOOP
        UPDATE Pendaftaran
        SET    status_pendaftaran = 'Batal'
        WHERE  id_daftar          = r.id_daftar;

        RAISE NOTICE 'NOTIF_FLAG|PHONE:%|NAMA:%|PESAN:Jadwal pemeriksaan Anda dibatalkan. Mohon hubungi klinik untuk penjadwalan ulang.',
            r.no_telp, r.nama;

        p_jumlah_terpengaruh := p_jumlah_terpengaruh + 1;
    END LOOP;

    RAISE NOTICE 'Jadwal ID % berhasil dibatalkan. % pasien terpengaruh.', p_id_jadwal, p_jumlah_terpengaruh;

EXCEPTION
    WHEN OTHERS THEN
        RAISE;
END;
$$;
```

**Penjelasan langkah demi langkah:**

1. **Step 1 — Validasi**: Pastikan jadwal ada dan belum berstatus 'Batal'.
2. **Step 2 — Batalkan jadwal**: UPDATE status jadwal menjadi 'Batal'.
3. **Step 3 — Cascade pembatalan**: Loop semua pendaftaran aktif (bukan Selesai/Batal) pada jadwal tersebut. Setiap pendaftaran diubah ke 'Batal'. Trigger `trg_buka_kembali_jadwal` otomatis aktif.
4. **Notifikasi**: RAISE NOTICE menghasilkan flag yang bisa dibaca backend untuk kirim WhatsApp/SMS.
5. **Output**: Counter `p_jumlah_terpengaruh` menunjukkan berapa pasien terdampak.

---

### 4.3 sp_Proses_Pembayaran

```sql
CREATE OR REPLACE PROCEDURE sp_Proses_Pembayaran(
    IN  p_id_daftar     INT,
    IN  p_metode_bayar  VARCHAR(50),
    OUT p_id_pembayaran INT,
    OUT p_total_tagihan NUMERIC(15, 2),
    OUT p_pesan         VARCHAR(500)
)
LANGUAGE plpgsql AS $$
DECLARE
    v_valid_metode  BOOLEAN;
    v_status_daftar VARCHAR(20);
BEGIN
    SELECT status_pendaftaran
    INTO   v_status_daftar
    FROM   Pendaftaran
    WHERE  id_daftar = p_id_daftar;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'sp_Proses_Pembayaran: Pendaftaran ID % tidak ditemukan.', p_id_daftar;
    END IF;

    IF v_status_daftar = 'Batal' THEN
        RAISE EXCEPTION 'Pendaftaran ID % sudah dibatalkan, tidak dapat diproses.', p_id_daftar;
    END IF;

    IF v_status_daftar = 'Selesai' AND EXISTS (
        SELECT 1 FROM Pembayaran WHERE id_daftar = p_id_daftar AND status_bayar = 'Lunas'
    ) THEN
        RAISE EXCEPTION 'Pembayaran untuk pendaftaran ID % sudah Lunas sebelumnya.', p_id_daftar;
    END IF;

    v_valid_metode := fn_Validasi_Metode_Bayar(p_id_daftar, p_metode_bayar);
    IF NOT v_valid_metode THEN
        RAISE EXCEPTION 'Metode bayar "%" tidak sesuai dengan kategori pasien.', p_metode_bayar;
    END IF;

    p_total_tagihan := fn_Hitung_Total_Tagihan(p_id_daftar);

    IF EXISTS (SELECT 1 FROM Pembayaran WHERE id_daftar = p_id_daftar) THEN
        UPDATE Pembayaran
        SET    metode_bayar = p_metode_bayar,
               status_bayar = 'Lunas'
        WHERE  id_daftar    = p_id_daftar
        RETURNING id_pembayaran INTO p_id_pembayaran;
    ELSE
        INSERT INTO Pembayaran (id_daftar, metode_bayar, status_bayar)
        VALUES (p_id_daftar, p_metode_bayar, 'Lunas')
        RETURNING id_pembayaran INTO p_id_pembayaran;
    END IF;

    p_pesan := FORMAT(
        'Pembayaran LUNAS. ID Pembayaran: %s | Total: Rp %s | Metode: %s.',
        p_id_pembayaran, p_total_tagihan, p_metode_bayar
    );

EXCEPTION
    WHEN OTHERS THEN
        RAISE;
END;
$$;
```

**Penjelasan langkah demi langkah:**

1. **Step 1 — Validasi status pendaftaran**: Ambil status. Tolak jika 'Batal'. Tolak jika sudah 'Selesai' dan Lunas (double payment).
2. **Step 2 — Validasi metode bayar**: Panggil `fn_Validasi_Metode_Bayar` untuk cek kesesuaian kategori pasien dan metode bayar (guard BPJS).
3. **Step 3 — Kalkulasi tagihan**: Panggil `fn_Hitung_Total_Tagihan`. BPJS = Rp 0, non-BPJS = SUM tarif layanan.
4. **Step 4 — Simpan pembayaran**: Jika record pembayaran sudah ada → UPDATE. Jika belum → INSERT baru. Keduanya langsung set status 'Lunas'.
5. Trigger `trg_validasi_metode_bayar`, `trg_update_status_pendaftaran`, dan `trg_audit_perubahan_pembayaran` aktif otomatis.
6. **Output**: ID pembayaran, total tagihan, dan pesan konfirmasi.

---

### 4.4 sp_Reschedule_Pendaftaran

```sql
CREATE OR REPLACE PROCEDURE sp_Reschedule_Pendaftaran(
    IN  p_id_daftar      INT,
    IN  p_id_jadwal_baru INT,
    OUT p_pesan           VARCHAR(500)
)
LANGUAGE plpgsql AS $$
DECLARE
    v_id_jadwal_lama  INT;
    v_status_daftar   VARCHAR(20);
    v_jadwal_tersedia BOOLEAN;
BEGIN
    SELECT id_jadwal, status_pendaftaran
    INTO   v_id_jadwal_lama, v_status_daftar
    FROM   Pendaftaran
    WHERE  id_daftar = p_id_daftar;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'sp_Reschedule: Pendaftaran ID % tidak ditemukan.', p_id_daftar;
    END IF;

    IF v_status_daftar IN ('Selesai', 'Diperiksa') THEN
        RAISE EXCEPTION 'Pendaftaran ID % tidak bisa di-reschedule karena berstatus "%".',
            p_id_daftar, v_status_daftar;
    END IF;

    v_jadwal_tersedia := fn_Cek_Ketersediaan_Jadwal(p_id_jadwal_baru);
    IF NOT v_jadwal_tersedia THEN
        RAISE EXCEPTION 'Jadwal baru ID % tidak tersedia atau sudah penuh.', p_id_jadwal_baru;
    END IF;

    UPDATE Pendaftaran
    SET    id_jadwal            = p_id_jadwal_baru,
           status_pendaftaran   = 'Menunggu',
           tanggal_pendaftaran  = CURRENT_TIMESTAMP,
           nomor_antrean        = (
               SELECT COALESCE(MAX(nomor_antrean), 0) + 1
               FROM   Pendaftaran
               WHERE  id_jadwal  = p_id_jadwal_baru
                 AND  id_daftar  != p_id_daftar
           )
    WHERE  id_daftar = p_id_daftar;

    p_pesan := FORMAT(
        'Reschedule berhasil. ID Daftar: %s | Dari Jadwal ID: %s → Jadwal ID: %s.',
        p_id_daftar, v_id_jadwal_lama, p_id_jadwal_baru
    );

EXCEPTION
    WHEN OTHERS THEN
        RAISE;
END;
$$;
```

**Penjelasan langkah demi langkah:**

1. **Step 1 — Ambil data saat ini**: Ambil jadwal lama dan status pendaftaran. Tolak jika tidak ditemukan.
2. **Validasi status**: Pendaftaran 'Selesai' atau 'Diperiksa' tidak boleh di-reschedule (sudah dalam proses medis).
3. **Step 2 — Validasi jadwal baru**: Panggil `fn_Cek_Ketersediaan_Jadwal` untuk pastikan jadwal tujuan masih bisa menerima pasien.
4. **Step 3 — Pindahkan jadwal**: UPDATE pendaftaran ke jadwal baru. Reset status ke 'Menunggu'. Hitung ulang nomor antrean dengan subquery MAX+1 (exclude diri sendiri). Reset timestamp pendaftaran.
5. Trigger `trg_buka_kembali_jadwal` memeriksa jadwal lama — jika sebelumnya Penuh, bisa dibuka kembali.

---

### 4.5 sp_Update_Status_Pemeriksaan

```sql
CREATE OR REPLACE PROCEDURE sp_Update_Status_Pemeriksaan(
    IN  p_id_daftar   INT,
    IN  p_status_baru VARCHAR(20),
    IN  p_id_admin    INT,
    OUT p_pesan       VARCHAR(500)
)
LANGUAGE plpgsql AS $$
DECLARE
    v_status_lama    VARCHAR(20);
    v_valid_transisi BOOLEAN := FALSE;
BEGIN
    SELECT status_pendaftaran
    INTO   v_status_lama
    FROM   Pendaftaran
    WHERE  id_daftar = p_id_daftar;

    IF NOT FOUND THEN
        RAISE EXCEPTION 'sp_Update_Status: Pendaftaran ID % tidak ditemukan.', p_id_daftar;
    END IF;

    IF    v_status_lama = 'Menunggu'  AND p_status_baru IN ('Diperiksa', 'Batal') THEN
        v_valid_transisi := TRUE;
    ELSIF v_status_lama = 'Diperiksa' AND p_status_baru IN ('Selesai',   'Batal') THEN
        v_valid_transisi := TRUE;
    END IF;

    IF NOT v_valid_transisi THEN
        RAISE EXCEPTION 'Transisi status dari "%" ke "%" tidak valid.', v_status_lama, p_status_baru;
    END IF;

    UPDATE Pendaftaran
    SET    status_pendaftaran = p_status_baru
    WHERE  id_daftar          = p_id_daftar;

    p_pesan := FORMAT(
        'Status pendaftaran ID %s diubah: "%s" → "%s" oleh Admin ID %s.',
        p_id_daftar, v_status_lama, p_status_baru, p_id_admin
    );

EXCEPTION
    WHEN OTHERS THEN
        RAISE;
END;
$$;
```

**Penjelasan langkah demi langkah:**

1. Ambil status pendaftaran saat ini (`v_status_lama`).
2. **State machine validation** — hanya transisi berikut yang diperbolehkan:
   - `Menunggu` → `Diperiksa` atau `Batal`
   - `Diperiksa` → `Selesai` atau `Batal`
   - `Selesai` dan `Batal` = terminal state (tidak bisa diubah lagi)
3. Jika transisi tidak valid → raise exception.
4. UPDATE status pendaftaran ke status baru.
5. Jika status baru = 'Batal', trigger `trg_buka_kembali_jadwal` akan cek apakah slot jadwal bisa dibuka kembali.

---

## 5. Triggers

### 5.1 Trigger 1 — Sinkronisasi Status Pendaftaran dari Pembayaran

```sql
CREATE OR REPLACE FUNCTION trg_func_batal_daftar_karena_bayar()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.status_bayar = 'Gagal' THEN
        UPDATE Pendaftaran
        SET    status_pendaftaran = 'Batal'
        WHERE  id_daftar = NEW.id_daftar;
    END IF;

    IF NEW.status_bayar = 'Lunas' AND OLD.status_bayar = 'Pending' THEN
        UPDATE Pendaftaran
        SET    status_pendaftaran = 'Menunggu'
        WHERE  id_daftar = NEW.id_daftar;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_update_status_pendaftaran
AFTER UPDATE OF status_bayar ON Pembayaran
FOR EACH ROW
WHEN (OLD.status_bayar IS DISTINCT FROM NEW.status_bayar)
EXECUTE FUNCTION trg_func_batal_daftar_karena_bayar();
```

**Penjelasan langkah demi langkah:**

1. Trigger aktif **AFTER UPDATE** pada kolom `status_bayar` di tabel `Pembayaran`.
2. Hanya fire jika status_bayar benar-benar berubah (IS DISTINCT FROM).
3. Jika pembayaran menjadi 'Gagal' → otomatis batalkan pendaftaran terkait.
4. Jika pembayaran berubah dari 'Pending' ke 'Lunas' → aktifkan status pendaftaran menjadi 'Menunggu'.
5. Return NEW karena ini AFTER trigger (return value tidak mempengaruhi data, tapi wajib ada).

---

### 5.2 Trigger 2 — Auto-Generate Nomor Antrean

```sql
CREATE OR REPLACE FUNCTION trg_func_auto_antrean()
RETURNS TRIGGER AS $$
DECLARE
    v_next_antrean INT;
BEGIN
    PERFORM id_jadwal
    FROM    Jadwal_Dokter
    WHERE   id_jadwal = NEW.id_jadwal
    FOR UPDATE;

    SELECT COALESCE(MAX(nomor_antrean), 0) + 1
    INTO   v_next_antrean
    FROM   Pendaftaran
    WHERE  id_jadwal = NEW.id_jadwal;

    NEW.nomor_antrean := v_next_antrean;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_set_nomor_antrean
BEFORE INSERT ON Pendaftaran
FOR EACH ROW
EXECUTE FUNCTION trg_func_auto_antrean();
```

**Penjelasan langkah demi langkah:**

1. Trigger aktif **BEFORE INSERT** pada `Pendaftaran`.
2. `PERFORM ... FOR UPDATE` melakukan row-level lock pada baris `Jadwal_Dokter` terkait. Ini mencegah race condition saat banyak pasien mendaftar bersamaan.
3. Hitung nomor antrean berikutnya: MAX(nomor_antrean) + 1 pada jadwal yang sama.
4. COALESCE menangani kasus pertama (belum ada antrean → mulai dari 1).
5. Set `NEW.nomor_antrean` sebelum INSERT dieksekusi.

---

### 5.3 Trigger 3 — Jadwal Otomatis "Penuh"

```sql
CREATE OR REPLACE FUNCTION trg_func_lock_jadwal_penuh()
RETURNS TRIGGER AS $$
DECLARE
    v_jumlah_pasien INT;
    v_limit_antrean INT := 30;
BEGIN
    SELECT COUNT(*)
    INTO   v_jumlah_pasien
    FROM   Pendaftaran
    WHERE  id_jadwal          = NEW.id_jadwal
      AND  status_pendaftaran != 'Batal';

    IF v_jumlah_pasien >= v_limit_antrean THEN
        UPDATE Jadwal_Dokter
        SET    status_jadwal = 'Penuh'
        WHERE  id_jadwal     = NEW.id_jadwal
          AND  status_jadwal = 'Tersedia';
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_cek_kuota_jadwal
AFTER INSERT ON Pendaftaran
FOR EACH ROW
EXECUTE FUNCTION trg_func_lock_jadwal_penuh();
```

**Penjelasan langkah demi langkah:**

1. Trigger aktif **AFTER INSERT** pada `Pendaftaran`.
2. Hitung jumlah pendaftaran aktif (bukan Batal) pada jadwal yang baru diinsert.
3. Jika jumlah ≥ 30 (batas kuota) → UPDATE status jadwal menjadi 'Penuh'.
4. Guard condition `AND status_jadwal = 'Tersedia'` mencegah overwrite status 'Batal' yang sudah di-set manual.

---

### 5.4 Trigger 4 — Rujukan Otomatis dari Skrining

```sql
CREATE OR REPLACE FUNCTION trg_func_draft_rujukan_otomatis()
RETURNS TRIGGER AS $$
BEGIN
    IF NEW.status_kelayakan = 'Perlu Rujukan' THEN
        IF NOT EXISTS (SELECT 1 FROM Rujukan WHERE id_skrining = NEW.id_skrining) THEN
            INSERT INTO Rujukan (id_skrining, alasan_rujukan, asal_rujukan, tujuan_rujukan)
            VALUES (
                NEW.id_skrining,
                'Draft Otomatis — Hasil Skrining: Perlu Rujukan',
                'Klinik Utama',
                'Belum Ditentukan'
            );
        END IF;
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_buat_draft_rujukan
AFTER INSERT OR UPDATE OF status_kelayakan ON Skrining
FOR EACH ROW
EXECUTE FUNCTION trg_func_draft_rujukan_otomatis();
```

**Penjelasan langkah demi langkah:**

1. Trigger aktif **AFTER INSERT OR UPDATE** pada kolom `status_kelayakan` di `Skrining`.
2. Jika status = 'Perlu Rujukan' → otomatis buat draft rujukan.
3. Guard `NOT EXISTS` mencegah duplikasi (karena `id_skrining` sudah UNIQUE di tabel Rujukan, tapi guard ini memberikan pesan error yang lebih bersih).
4. Tujuan rujukan di-set 'Belum Ditentukan' — admin mengisi manual kemudian.

---

### 5.5 Trigger 5 — Validasi Metode Bayar vs Kategori Pasien

```sql
CREATE OR REPLACE FUNCTION trg_func_validasi_metode_bayar()
RETURNS TRIGGER AS $$
DECLARE
    v_kategori VARCHAR(50);
BEGIN
    SELECT p.kategori_pasien
    INTO   v_kategori
    FROM   Pendaftaran dft
    JOIN   Pasien p ON dft.id_pasien = p.id_pasien
    WHERE  dft.id_daftar = NEW.id_daftar;

    IF v_kategori = 'BPJS' AND NEW.metode_bayar != 'BPJS' THEN
        RAISE EXCEPTION
            'VALIDASI: Pasien BPJS harus menggunakan metode bayar BPJS, bukan "%".',
            NEW.metode_bayar;
    END IF;

    IF v_kategori != 'BPJS' AND NEW.metode_bayar = 'BPJS' THEN
        RAISE EXCEPTION
            'VALIDASI: Pasien kategori "%" tidak dapat menggunakan metode bayar BPJS.',
            v_kategori;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_validasi_metode_bayar
BEFORE INSERT OR UPDATE OF metode_bayar ON Pembayaran
FOR EACH ROW
EXECUTE FUNCTION trg_func_validasi_metode_bayar();
```

**Penjelasan langkah demi langkah:**

1. Trigger aktif **BEFORE INSERT OR UPDATE** pada kolom `metode_bayar` di `Pembayaran`.
2. JOIN `Pendaftaran` → `Pasien` untuk ambil kategori pasien.
3. Rule 1: Pasien BPJS + metode bukan BPJS → RAISE EXCEPTION (tolak operasi).
4. Rule 2: Pasien non-BPJS + metode BPJS → RAISE EXCEPTION (tolak operasi).
5. Ini adalah **last-line defense** di level database — bahkan jika validasi aplikasi dilewati, data kotor tetap ditolak.

---

### 5.6 Trigger 6 — Buka Kembali Jadwal saat Pembatalan

```sql
CREATE OR REPLACE FUNCTION trg_func_buka_kembali_jadwal()
RETURNS TRIGGER AS $$
DECLARE
    v_jumlah_aktif  INT;
    v_limit         CONSTANT INT := 30;
    v_status_jadwal VARCHAR(20);
BEGIN
    IF NEW.status_pendaftaran = 'Batal' AND OLD.status_pendaftaran != 'Batal' THEN

        SELECT status_jadwal INTO v_status_jadwal
        FROM   Jadwal_Dokter
        WHERE  id_jadwal = NEW.id_jadwal;

        SELECT COUNT(*) INTO v_jumlah_aktif
        FROM   Pendaftaran
        WHERE  id_jadwal          = NEW.id_jadwal
          AND  status_pendaftaran != 'Batal';

        IF v_status_jadwal = 'Penuh' AND v_jumlah_aktif < v_limit THEN
            UPDATE Jadwal_Dokter
            SET    status_jadwal = 'Tersedia'
            WHERE  id_jadwal     = NEW.id_jadwal;
        END IF;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_buka_kembali_jadwal
AFTER UPDATE OF status_pendaftaran ON Pendaftaran
FOR EACH ROW
EXECUTE FUNCTION trg_func_buka_kembali_jadwal();
```

**Penjelasan langkah demi langkah:**

1. Trigger aktif **AFTER UPDATE** pada kolom `status_pendaftaran` di `Pendaftaran`.
2. Hanya proses jika status berubah **ke** 'Batal' (dari status lain).
3. Ambil status jadwal terkait. Hitung sisa antrean aktif.
4. Jika jadwal sebelumnya 'Penuh' **dan** antrean aktif sekarang < 30 → kembalikan status ke 'Tersedia'.
5. Melengkapi Trigger 3 (lock penuh) membentuk siklus penuh: Penuh ↔ Tersedia.

---

### 5.7 Trigger 7 — Blokir Pendaftaran ke Jadwal Tidak Aktif

```sql
CREATE OR REPLACE FUNCTION trg_func_blokir_daftar_ke_jadwal_nonaktif()
RETURNS TRIGGER AS $$
DECLARE
    v_status_jadwal VARCHAR(20);
BEGIN
    SELECT status_jadwal
    INTO   v_status_jadwal
    FROM   Jadwal_Dokter
    WHERE  id_jadwal = NEW.id_jadwal;

    IF v_status_jadwal = 'Batal' THEN
        RAISE EXCEPTION
            'DITOLAK: Tidak dapat mendaftar ke jadwal ID % yang sudah dibatalkan.',
            NEW.id_jadwal;
    END IF;

    IF v_status_jadwal = 'Penuh' THEN
        RAISE EXCEPTION
            'DITOLAK: Jadwal ID % sudah penuh. Silakan pilih jadwal lain.',
            NEW.id_jadwal;
    END IF;

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_blokir_daftar_ke_jadwal_nonaktif
BEFORE INSERT ON Pendaftaran
FOR EACH ROW
EXECUTE FUNCTION trg_func_blokir_daftar_ke_jadwal_nonaktif();
```

**Penjelasan langkah demi langkah:**

1. Trigger aktif **BEFORE INSERT** pada `Pendaftaran`.
2. Ambil status jadwal yang dipilih pasien.
3. Jika jadwal 'Batal' → tolak pendaftaran dengan exception.
4. Jika jadwal 'Penuh' → tolak pendaftaran dengan exception.
5. Nama trigger dimulai `trg_blokir...` sehingga secara alfabet berjalan **sebelum** `trg_set_nomor_antrean` — validasi dulu, baru assign nomor.
6. Ini adalah lapisan validasi kedua (setelah `fn_Cek_Ketersediaan_Jadwal`) sebagai perlindungan terhadap race condition.

---

### 5.8 Trigger 8 — Audit Log Perubahan Pembayaran

```sql
CREATE OR REPLACE FUNCTION trg_func_audit_pembayaran()
RETURNS TRIGGER AS $$
BEGIN
    INSERT INTO Log_Pembayaran (
        id_pembayaran,
        id_daftar,
        status_lama,
        status_baru,
        metode_lama,
        metode_baru,
        keterangan
    )
    VALUES (
        NEW.id_pembayaran,
        NEW.id_daftar,
        OLD.status_bayar,
        NEW.status_bayar,
        OLD.metode_bayar,
        NEW.metode_bayar,
        FORMAT('Perubahan direkam pada %s', NOW())
    );

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_audit_perubahan_pembayaran
AFTER UPDATE ON Pembayaran
FOR EACH ROW
WHEN (
    OLD.status_bayar IS DISTINCT FROM NEW.status_bayar
    OR OLD.metode_bayar IS DISTINCT FROM NEW.metode_bayar
)
EXECUTE FUNCTION trg_func_audit_pembayaran();
```

**Penjelasan langkah demi langkah:**

1. Trigger aktif **AFTER UPDATE** pada tabel `Pembayaran`.
2. Hanya fire jika `status_bayar` atau `metode_bayar` benar-benar berubah (WHEN clause).
3. INSERT record baru ke `Log_Pembayaran` dengan:
   - Nilai lama (OLD) dan baru (NEW) untuk status dan metode bayar.
   - Timestamp otomatis via `NOW()`.
4. Jejak audit ini permanen — tidak bisa dihapus oleh transaksi yang di-rollback karena trigger hanya fire pada operasi yang sukses.
5. Berguna untuk rekonsiliasi keuangan, deteksi anomali, dan investigasi.

---

## 6. Views

### 6.1 v_Antrian_Aktif

```sql
CREATE OR REPLACE VIEW v_Antrian_Aktif AS
SELECT
    jd.id_jadwal,
    jd.hari_tanggal,
    jd.jam_mulai,
    jd.jam_selesai,
    d.nama_dokter,
    pl.nama_poli,
    p.id_daftar,
    p.nomor_antrean,
    ps.nama          AS nama_pasien,
    ps.kategori_pasien,
    ps.no_telp,
    p.status_pendaftaran,
    fn_Hitung_Estimasi_Tunggu(p.id_daftar) AS estimasi_dipanggil
FROM   Pendaftaran p
JOIN   Jadwal_Dokter jd ON p.id_jadwal  = jd.id_jadwal
JOIN   Dokter d         ON jd.id_dokter = d.id_dokter
JOIN   Poli pl          ON d.id_poli    = pl.id_poli
JOIN   Pasien ps        ON p.id_pasien  = ps.id_pasien
WHERE  p.status_pendaftaran IN ('Menunggu', 'Diperiksa')
ORDER  BY jd.id_jadwal, p.nomor_antrean;
```

**Penjelasan langkah demi langkah:**

1. View ini menampilkan antrean pasien aktif (Menunggu + Diperiksa).
2. JOIN 5 tabel: Pendaftaran → Jadwal_Dokter → Dokter → Poli → Pasien.
3. Kolom `estimasi_dipanggil` memanggil function `fn_Hitung_Estimasi_Tunggu` untuk setiap baris.
4. Diurutkan berdasarkan jadwal lalu nomor antrean.
5. Siap dipakai untuk papan antrean digital: `SELECT * FROM v_Antrian_Aktif WHERE hari_tanggal = CURRENT_DATE;`

---

### 6.2 v_Resume_Harian_Jadwal

```sql
CREATE OR REPLACE VIEW v_Resume_Harian_Jadwal AS
SELECT
    jd.hari_tanggal,
    jd.id_jadwal,
    d.nama_dokter,
    pl.nama_poli,
    jd.jam_mulai,
    jd.jam_selesai,
    jd.status_jadwal,
    COUNT(p.id_daftar)
        FILTER (WHERE p.status_pendaftaran != 'Batal')    AS total_aktif,
    COUNT(p.id_daftar)
        FILTER (WHERE p.status_pendaftaran = 'Selesai')   AS total_selesai,
    COUNT(p.id_daftar)
        FILTER (WHERE p.status_pendaftaran = 'Diperiksa') AS sedang_diperiksa,
    COUNT(p.id_daftar)
        FILTER (WHERE p.status_pendaftaran = 'Menunggu')  AS total_menunggu,
    COUNT(p.id_daftar)
        FILTER (WHERE p.status_pendaftaran = 'Batal')     AS total_batal
FROM   Jadwal_Dokter jd
JOIN   Dokter d   ON jd.id_dokter = d.id_dokter
JOIN   Poli pl    ON d.id_poli    = pl.id_poli
LEFT JOIN Pendaftaran p ON p.id_jadwal = jd.id_jadwal
GROUP  BY jd.id_jadwal, jd.hari_tanggal, jd.jam_mulai, jd.jam_selesai,
          jd.status_jadwal, d.nama_dokter, pl.nama_poli
ORDER  BY jd.hari_tanggal DESC, jd.jam_mulai;
```

**Penjelasan langkah demi langkah:**

1. View ini menampilkan ringkasan operasional per jadwal per hari.
2. JOIN Jadwal_Dokter → Dokter → Poli, LEFT JOIN Pendaftaran (agar jadwal tanpa pendaftaran tetap muncul).
3. FILTER clause PostgreSQL menghitung setiap status secara terpisah dalam satu query.
4. `total_aktif` = semua pendaftaran kecuali yang Batal.
5. GROUP BY per jadwal, ORDER BY tanggal terbaru dulu.
6. Berguna untuk monitoring beban kerja dokter: `SELECT * FROM v_Resume_Harian_Jadwal WHERE hari_tanggal = CURRENT_DATE;`

---

## 7. Alur Trigger per Skenario

```
DAFTAR PASIEN BARU
  INSERT Pendaftaran
    → [BEFORE] trg_blokir_daftar_ke_jadwal_nonaktif  ← validasi jadwal
    → [BEFORE] trg_set_nomor_antrean                  ← assign nomor antrean
    → [AFTER]  trg_cek_kuota_jadwal                   ← kunci jadwal jika penuh

PEMBAYARAN
  INSERT/UPDATE Pembayaran
    → [BEFORE] trg_validasi_metode_bayar               ← validasi BPJS
    → [AFTER]  trg_update_status_pendaftaran            ← sinkronisasi status
    → [AFTER]  trg_audit_perubahan_pembayaran           ← catat audit log

PEMBATALAN PENDAFTARAN
  UPDATE Pendaftaran SET status = 'Batal'
    → [AFTER]  trg_buka_kembali_jadwal                 ← buka slot jadwal

SKRINING
  INSERT/UPDATE Skrining SET status = 'Perlu Rujukan'
    → [AFTER]  trg_buat_draft_rujukan                  ← buat draft rujukan
```
