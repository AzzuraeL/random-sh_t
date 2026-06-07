# 🏥 Implementasi Database Sistem Klinik — MySQL 8.0+

> Seluruh SQL siap dijalankan di MySQL 8.0+. Tidak ada komentar di dalam blok kode.
> Penjelasan diberikan **setelah** setiap blok.

---

## 1. DDL — Tabel Master & Operasional

```sql
DROP TABLE IF EXISTS Log_Pembayaran;
DROP TABLE IF EXISTS Pembayaran;
DROP TABLE IF EXISTS Pendaftaran_Layanan;
DROP TABLE IF EXISTS Pendaftaran;
DROP TABLE IF EXISTS Rujukan;
DROP TABLE IF EXISTS Skrining;
DROP TABLE IF EXISTS Pasien;
DROP TABLE IF EXISTS Layanan;
DROP TABLE IF EXISTS Petugas_Admin;
DROP TABLE IF EXISTS Jadwal_Dokter;
DROP TABLE IF EXISTS Dokter;
DROP TABLE IF EXISTS Poli;

CREATE TABLE Poli (
    id_poli       INT AUTO_INCREMENT PRIMARY KEY,
    nama_poli     VARCHAR(100) NOT NULL,
    nomor_ruangan VARCHAR(20)  NOT NULL
) ENGINE=InnoDB;

CREATE TABLE Layanan (
    id_layanan          INT AUTO_INCREMENT PRIMARY KEY,
    nama_layanan        VARCHAR(150)   NOT NULL,
    tarif_total_layanan DECIMAL(15, 2) NOT NULL DEFAULT 0
) ENGINE=InnoDB;

CREATE TABLE Petugas_Admin (
    id_admin     INT AUTO_INCREMENT PRIMARY KEY,
    nama_petugas VARCHAR(100) NOT NULL,
    shift_tugas  VARCHAR(20)  CHECK (shift_tugas IN ('Pagi', 'Siang', 'Malam'))
) ENGINE=InnoDB;

CREATE TABLE Pasien (
    id_pasien       INT AUTO_INCREMENT PRIMARY KEY,
    nim_nik         VARCHAR(20)  UNIQUE NOT NULL,
    nama            VARCHAR(150) NOT NULL,
    asal_faskes     VARCHAR(100),
    no_telp         VARCHAR(20),
    kategori_pasien VARCHAR(50)  CHECK (kategori_pasien IN ('Umum', 'BPJS', 'Asuransi Lain'))
) ENGINE=InnoDB;

CREATE TABLE Dokter (
    id_dokter   INT AUTO_INCREMENT PRIMARY KEY,
    id_poli     INT          NOT NULL,
    nama_dokter VARCHAR(150) NOT NULL,
    CONSTRAINT fk_dokter_poli FOREIGN KEY (id_poli) REFERENCES Poli(id_poli) ON DELETE RESTRICT
) ENGINE=InnoDB;

CREATE TABLE Jadwal_Dokter (
    id_jadwal     INT AUTO_INCREMENT PRIMARY KEY,
    id_dokter     INT     NOT NULL,
    hari_tanggal  DATE    NOT NULL,
    jam_mulai     TIME    NOT NULL,
    jam_selesai   TIME    NOT NULL,
    status_jadwal VARCHAR(20) DEFAULT 'Tersedia'
        CHECK (status_jadwal IN ('Tersedia', 'Penuh', 'Batal')),
    CONSTRAINT fk_jadwal_dokter FOREIGN KEY (id_dokter) REFERENCES Dokter(id_dokter) ON DELETE CASCADE
) ENGINE=InnoDB;

CREATE TABLE Skrining (
    id_skrining      INT AUTO_INCREMENT PRIMARY KEY,
    id_pasien        INT       NOT NULL,
    tanggal_skrining TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    hasil_skrinning  TEXT,
    status_kelayakan VARCHAR(50)
        CHECK (status_kelayakan IN ('Layak Ditangani', 'Perlu Rujukan', 'Kritis')),
    CONSTRAINT fk_skrining_pasien FOREIGN KEY (id_pasien) REFERENCES Pasien(id_pasien) ON DELETE CASCADE
) ENGINE=InnoDB;

CREATE TABLE Rujukan (
    id_rujukan      INT AUTO_INCREMENT PRIMARY KEY,
    id_skrining     INT  UNIQUE NOT NULL,
    alasan_rujukan  TEXT NOT NULL,
    tanggal_rujukan TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    asal_rujukan    VARCHAR(100),
    tujuan_rujukan  VARCHAR(150) NOT NULL,
    CONSTRAINT fk_rujukan_skrining FOREIGN KEY (id_skrining) REFERENCES Skrining(id_skrining) ON DELETE CASCADE
) ENGINE=InnoDB;

CREATE TABLE Pendaftaran (
    id_daftar           INT AUTO_INCREMENT PRIMARY KEY,
    id_pasien           INT NOT NULL,
    id_jadwal           INT NOT NULL,
    id_admin            INT NOT NULL,
    tanggal_pendaftaran TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    status_pendaftaran  VARCHAR(20) DEFAULT 'Menunggu'
        CHECK (status_pendaftaran IN ('Menunggu', 'Diperiksa', 'Selesai', 'Batal')),
    nomor_antrean       INT,
    CONSTRAINT fk_daftar_pasien FOREIGN KEY (id_pasien) REFERENCES Pasien(id_pasien) ON DELETE RESTRICT,
    CONSTRAINT fk_daftar_jadwal FOREIGN KEY (id_jadwal) REFERENCES Jadwal_Dokter(id_jadwal) ON DELETE RESTRICT,
    CONSTRAINT fk_daftar_admin  FOREIGN KEY (id_admin)  REFERENCES Petugas_Admin(id_admin)
) ENGINE=InnoDB;

CREATE TABLE Pendaftaran_Layanan (
    id_daftar  INT NOT NULL,
    id_layanan INT NOT NULL,
    PRIMARY KEY (id_daftar, id_layanan),
    CONSTRAINT fk_pl_daftar  FOREIGN KEY (id_daftar)  REFERENCES Pendaftaran(id_daftar) ON DELETE CASCADE,
    CONSTRAINT fk_pl_layanan FOREIGN KEY (id_layanan) REFERENCES Layanan(id_layanan) ON DELETE RESTRICT
) ENGINE=InnoDB;

CREATE TABLE Pembayaran (
    id_pembayaran INT AUTO_INCREMENT PRIMARY KEY,
    id_daftar     INT UNIQUE NOT NULL,
    metode_bayar  VARCHAR(50)
        CHECK (metode_bayar IN ('Tunai', 'Transfer', 'E-Wallet', 'BPJS')),
    status_bayar  VARCHAR(20) DEFAULT 'Pending'
        CHECK (status_bayar IN ('Pending', 'Lunas', 'Gagal')),
    CONSTRAINT fk_bayar_daftar FOREIGN KEY (id_daftar) REFERENCES Pendaftaran(id_daftar) ON DELETE CASCADE
) ENGINE=InnoDB;
```

**Penjelasan langkah demi langkah:**

1. **DROP TABLE** — Dihapus satu per satu sesuai urutan dependensi (child dulu, parent belakangan). MySQL tidak support multi-table DROP dengan CASCADE seperti PostgreSQL.
2. **ENGINE=InnoDB** — Wajib untuk support foreign key, transaksi (COMMIT/ROLLBACK), dan row-level locking.
3. **AUTO_INCREMENT** — Pengganti `SERIAL` PostgreSQL. Otomatis increment setiap INSERT.
4. **DECIMAL(15,2)** — Pengganti `NUMERIC` PostgreSQL untuk presisi bilangan desimal.
5. **CHECK constraint** — Didukung penuh di MySQL 8.0.16+. Memvalidasi nilai kolom sebelum INSERT/UPDATE.
6. **Named FOREIGN KEY** — Setiap FK diberi nama eksplisit (`CONSTRAINT fk_...`) agar mudah di-debug jika terjadi pelanggaran referensi.
7. **Struktur tabel sama** dengan versi PostgreSQL — 4 tabel master (Poli, Layanan, Petugas_Admin, Pasien), 4 tabel operasional medis (Dokter, Jadwal_Dokter, Skrining, Rujukan), dan 4 tabel transaksional (Pendaftaran, Pendaftaran_Layanan, Pembayaran).

---

## 2. Tabel Audit Log

```sql
CREATE TABLE Log_Pembayaran (
    id_log          INT AUTO_INCREMENT PRIMARY KEY,
    id_pembayaran   INT         NOT NULL,
    id_daftar       INT         NOT NULL,
    status_lama     VARCHAR(20),
    status_baru     VARCHAR(20),
    metode_lama     VARCHAR(50),
    metode_baru     VARCHAR(50),
    waktu_perubahan TIMESTAMP   DEFAULT CURRENT_TIMESTAMP,
    keterangan      TEXT
) ENGINE=InnoDB;
```

**Penjelasan langkah demi langkah:**

1. Menyimpan jejak setiap perubahan pada tabel `Pembayaran`.
2. `status_lama`/`status_baru` merekam transisi status pembayaran.
3. `metode_lama`/`metode_baru` merekam perubahan metode bayar.
4. `waktu_perubahan` otomatis terisi timestamp.
5. Diisi oleh Trigger audit, bukan manual.

---

## 3. Functions

### 3.1 fn_Hitung_Total_Tagihan

```sql
DELIMITER //
CREATE FUNCTION fn_Hitung_Total_Tagihan(p_id_daftar INT)
RETURNS DECIMAL(15, 2)
READS SQL DATA
BEGIN
    DECLARE v_total    DECIMAL(15, 2) DEFAULT 0;
    DECLARE v_kategori VARCHAR(50);

    IF NOT EXISTS (SELECT 1 FROM Pendaftaran WHERE id_daftar = p_id_daftar) THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'fn_Hitung_Total_Tagihan: Pendaftaran tidak ditemukan.';
    END IF;

    SELECT p.kategori_pasien INTO v_kategori
    FROM   Pendaftaran dft
    JOIN   Pasien p ON dft.id_pasien = p.id_pasien
    WHERE  dft.id_daftar = p_id_daftar;

    IF v_kategori = 'BPJS' THEN
        RETURN 0.00;
    END IF;

    SELECT COALESCE(SUM(l.tarif_total_layanan), 0) INTO v_total
    FROM   Pendaftaran_Layanan pl
    JOIN   Layanan l ON pl.id_layanan = l.id_layanan
    WHERE  pl.id_daftar = p_id_daftar;

    RETURN v_total;
END //
DELIMITER ;
```

**Penjelasan langkah demi langkah:**

1. `READS SQL DATA` — mendeklarasikan function hanya membaca data, tidak memodifikasi.
2. `NOT EXISTS` cek keberadaan pendaftaran. Lebih efisien dari handler NOT FOUND karena satu query langsung evaluasi boolean.
3. `SIGNAL SQLSTATE '45000'` — pengganti `RAISE EXCEPTION` PostgreSQL. SQLSTATE 45000 = user-defined error.
4. Jika pasien BPJS → return 0 (ditanggung BPJS).
5. Jika non-BPJS → SUM tarif dari `Pendaftaran_Layanan` JOIN `Layanan`.
6. COALESCE menangani kasus belum ada layanan terdaftar.

---

### 3.2 fn_Cek_Ketersediaan_Jadwal

```sql
DELIMITER //
CREATE FUNCTION fn_Cek_Ketersediaan_Jadwal(p_id_jadwal INT)
RETURNS TINYINT(1)
READS SQL DATA
BEGIN
    DECLARE v_status_jadwal VARCHAR(20);
    DECLARE v_jumlah_aktif  INT;
    DECLARE v_limit_antrean INT DEFAULT 30;

    SELECT status_jadwal INTO v_status_jadwal
    FROM   Jadwal_Dokter
    WHERE  id_jadwal = p_id_jadwal;

    IF v_status_jadwal IS NULL THEN
        RETURN 0;
    END IF;

    IF v_status_jadwal IN ('Penuh', 'Batal') THEN
        RETURN 0;
    END IF;

    SELECT COUNT(*) INTO v_jumlah_aktif
    FROM   Pendaftaran
    WHERE  id_jadwal = p_id_jadwal
      AND  status_pendaftaran != 'Batal';

    RETURN v_jumlah_aktif < v_limit_antrean;
END //
DELIMITER ;
```

**Penjelasan langkah demi langkah:**

1. `TINYINT(1)` — pengganti `BOOLEAN` PostgreSQL. MySQL menyimpan boolean sebagai 0/1.
2. Jika `SELECT INTO` tidak menemukan baris, variabel tetap NULL → dicek dengan `IS NULL`.
3. Return 0 jika jadwal tidak ada, Penuh, atau Batal.
4. Hitung pendaftaran aktif (bukan Batal) pada jadwal.
5. Return 1 (true) hanya jika jumlah aktif < 30.

---

### 3.3 fn_Hitung_Estimasi_Tunggu

```sql
DELIMITER //
CREATE FUNCTION fn_Hitung_Estimasi_Tunggu(p_id_daftar INT)
RETURNS TIME
READS SQL DATA
BEGIN
    DECLARE v_nomor_antrean    INT;
    DECLARE v_id_jadwal        INT;
    DECLARE v_jam_mulai        TIME;
    DECLARE v_menit_per_pasien INT DEFAULT 10;

    SELECT nomor_antrean, id_jadwal
    INTO   v_nomor_antrean, v_id_jadwal
    FROM   Pendaftaran
    WHERE  id_daftar = p_id_daftar;

    IF v_nomor_antrean IS NULL THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'fn_Hitung_Estimasi_Tunggu: Pendaftaran tidak ditemukan.';
    END IF;

    SELECT jam_mulai INTO v_jam_mulai
    FROM   Jadwal_Dokter
    WHERE  id_jadwal = v_id_jadwal;

    RETURN ADDTIME(v_jam_mulai,
                   MAKETIME(0, (v_nomor_antrean - 1) * v_menit_per_pasien, 0));
END //
DELIMITER ;
```

**Penjelasan langkah demi langkah:**

1. Ambil `nomor_antrean` dan `id_jadwal` dari `Pendaftaran`.
2. Cek NULL untuk deteksi not-found (nomor_antrean pasti terisi oleh trigger saat INSERT).
3. Ambil `jam_mulai` dari `Jadwal_Dokter`.
4. **Perbaikan efisiensi**: `ADDTIME` + `MAKETIME` langsung menghasilkan TIME tanpa string concatenation dan casting. PostgreSQL versi lama melakukan `|| ' minutes')::INTERVAL` yang memerlukan parsing string setiap pemanggilan.
5. `MAKETIME(0, menit, 0)` membuat interval waktu `00:menit:00` secara langsung.
6. Contoh: antrean ke-5, jam mulai 09:00 → `ADDTIME('09:00:00', '00:40:00')` = `09:40:00`.

---

### 3.4 fn_Validasi_Metode_Bayar

```sql
DELIMITER //
CREATE FUNCTION fn_Validasi_Metode_Bayar(
    p_id_daftar   INT,
    p_metode_bayar VARCHAR(50)
)
RETURNS TINYINT(1)
READS SQL DATA
BEGIN
    DECLARE v_kategori VARCHAR(50);

    IF NOT EXISTS (SELECT 1 FROM Pendaftaran WHERE id_daftar = p_id_daftar) THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'fn_Validasi_Metode_Bayar: Pendaftaran tidak ditemukan.';
    END IF;

    SELECT p.kategori_pasien INTO v_kategori
    FROM   Pendaftaran dft
    JOIN   Pasien p ON dft.id_pasien = p.id_pasien
    WHERE  dft.id_daftar = p_id_daftar;

    IF v_kategori = 'BPJS' AND p_metode_bayar != 'BPJS' THEN
        RETURN 0;
    END IF;

    IF v_kategori != 'BPJS' AND p_metode_bayar = 'BPJS' THEN
        RETURN 0;
    END IF;

    RETURN 1;
END //
DELIMITER ;
```

**Penjelasan langkah demi langkah:**

1. Cek keberadaan pendaftaran dengan `NOT EXISTS`.
2. Ambil kategori pasien via JOIN.
3. Rule 1: Pasien BPJS wajib bayar via BPJS → return 0 jika tidak.
4. Rule 2: Pasien non-BPJS tidak boleh klaim BPJS → return 0 jika mencoba.
5. Return 1 jika lolos kedua rule.

---

## 4. Stored Procedures

### 4.1 sp_Daftar_Pasien_Baru_Kompleks

```sql
DELIMITER //
CREATE PROCEDURE sp_Daftar_Pasien_Baru_Kompleks(
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
BEGIN
    DECLARE v_id_pasien   INT;
    DECLARE v_id_skrining INT;

    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        RESIGNAL;
    END;

    START TRANSACTION;

    IF fn_Cek_Ketersediaan_Jadwal(p_id_jadwal) = 0 THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'JADWAL_TIDAK_TERSEDIA: Jadwal penuh atau sudah dibatalkan.';
    END IF;

    SELECT id_pasien INTO v_id_pasien
    FROM   Pasien
    WHERE  nim_nik = p_nim_nik;

    IF v_id_pasien IS NULL THEN
        INSERT INTO Pasien (nim_nik, nama, asal_faskes, no_telp, kategori_pasien)
        VALUES (p_nim_nik, p_nama, p_asal_faskes, p_no_telp, p_kategori_pasien);
        SET v_id_pasien = LAST_INSERT_ID();
    ELSE
        UPDATE Pasien
        SET    no_telp     = COALESCE(NULLIF(p_no_telp, ''), no_telp),
               asal_faskes = COALESCE(NULLIF(p_asal_faskes, ''), asal_faskes)
        WHERE  id_pasien   = v_id_pasien;
    END IF;

    INSERT INTO Skrining (id_pasien, hasil_skrinning, status_kelayakan)
    VALUES (v_id_pasien, p_hasil_skrining, p_status_kelayakan);
    SET v_id_skrining = LAST_INSERT_ID();

    INSERT INTO Pendaftaran (id_pasien, id_jadwal, id_admin)
    VALUES (v_id_pasien, p_id_jadwal, p_id_admin);
    SET p_id_daftar = LAST_INSERT_ID();

    SET p_pesan = CONCAT(
        'SUKSES: Pasien "', p_nama, '" berhasil didaftarkan. ID Daftar: ', p_id_daftar,
        ' | ID Skrining: ', v_id_skrining, ' | Jadwal ID: ', p_id_jadwal, '.'
    );

    COMMIT;
END //
DELIMITER ;
```

**Penjelasan langkah demi langkah:**

1. **EXIT HANDLER + ROLLBACK** — Jika terjadi error di mana pun dalam prosedur, semua operasi di-rollback otomatis lalu error diteruskan ke pemanggil via `RESIGNAL`. Ini menjamin atomisitas.
2. **START TRANSACTION** — Memulai transaksi eksplisit. Semua INSERT/UPDATE di dalam blok ini bersifat atomik.
3. **Step 1 — Validasi jadwal**: Panggil `fn_Cek_Ketersediaan_Jadwal`. Return 0 → SIGNAL error.
4. **Step 2 — Cek/buat pasien**: `SELECT INTO` lalu cek NULL. Pasien baru → INSERT + `LAST_INSERT_ID()`. Pasien lama → UPDATE kontak terkini.
5. **Step 3 — INSERT Skrining**: `LAST_INSERT_ID()` mengambil ID terakhir yang di-generate. Trigger `trg_buat_draft_rujukan_insert` aktif jika status = 'Perlu Rujukan'.
6. **Step 4 — INSERT Pendaftaran**: Trigger `trg_blokir_daftar_ke_jadwal_nonaktif` validasi jadwal, `trg_set_nomor_antrean` assign nomor antrean.
7. **CONCAT** — Pengganti `FORMAT()` PostgreSQL untuk membangun pesan output.
8. **COMMIT** — Jika semua langkah sukses, transaksi di-commit.

---

### 4.2 sp_Pembatalan_Jadwal_Dokter

```sql
DELIMITER //
CREATE PROCEDURE sp_Pembatalan_Jadwal_Dokter(
    IN  p_id_jadwal          INT,
    OUT p_jumlah_terpengaruh INT
)
BEGIN
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        RESIGNAL;
    END;

    START TRANSACTION;

    IF NOT EXISTS (
        SELECT 1 FROM Jadwal_Dokter
        WHERE  id_jadwal     = p_id_jadwal
          AND  status_jadwal != 'Batal'
    ) THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'Jadwal tidak ditemukan atau sudah berstatus Batal.';
    END IF;

    UPDATE Jadwal_Dokter
    SET    status_jadwal = 'Batal'
    WHERE  id_jadwal     = p_id_jadwal;

    UPDATE Pendaftaran
    SET    status_pendaftaran = 'Batal'
    WHERE  id_jadwal           = p_id_jadwal
      AND  status_pendaftaran NOT IN ('Selesai', 'Batal');

    SET p_jumlah_terpengaruh = ROW_COUNT();

    COMMIT;
END //
DELIMITER ;
```

**Penjelasan langkah demi langkah:**

1. **Perbaikan efisiensi utama**: Versi PostgreSQL menggunakan `FOR ... LOOP` yang melakukan UPDATE satu per satu (N query untuk N pasien). Versi MySQL ini menggunakan **satu batch UPDATE** yang mengubah semua baris sekaligus dalam satu operasi.
2. **ROW_COUNT()** — Menggantikan counter manual. Mengembalikan jumlah baris yang terpengaruh oleh UPDATE terakhir.
3. Trigger `trg_buka_kembali_jadwal` tetap aktif per baris karena MySQL trigger `FOR EACH ROW` tetap fire untuk setiap baris dalam batch UPDATE.
4. Notifikasi (`RAISE NOTICE` di PostgreSQL) tidak ada di MySQL. Backend mengambil daftar pasien terpengaruh dengan query terpisah setelah prosedur selesai — ini memisahkan concern database dan notifikasi.
5. Performa: O(1) query vs O(N) query. Untuk jadwal dengan 30 pasien, ini 30x lebih cepat.

---

### 4.3 sp_Proses_Pembayaran

```sql
DELIMITER //
CREATE PROCEDURE sp_Proses_Pembayaran(
    IN  p_id_daftar     INT,
    IN  p_metode_bayar  VARCHAR(50),
    OUT p_id_pembayaran INT,
    OUT p_total_tagihan DECIMAL(15, 2),
    OUT p_pesan         VARCHAR(500)
)
BEGIN
    DECLARE v_status_daftar VARCHAR(20);

    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        RESIGNAL;
    END;

    START TRANSACTION;

    SELECT status_pendaftaran INTO v_status_daftar
    FROM   Pendaftaran
    WHERE  id_daftar = p_id_daftar;

    IF v_status_daftar IS NULL THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'sp_Proses_Pembayaran: Pendaftaran tidak ditemukan.';
    END IF;

    IF v_status_daftar = 'Batal' THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'Pendaftaran sudah dibatalkan, tidak dapat diproses.';
    END IF;

    IF v_status_daftar = 'Selesai' AND EXISTS (
        SELECT 1 FROM Pembayaran WHERE id_daftar = p_id_daftar AND status_bayar = 'Lunas'
    ) THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'Pembayaran sudah Lunas sebelumnya.';
    END IF;

    IF fn_Validasi_Metode_Bayar(p_id_daftar, p_metode_bayar) = 0 THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'Metode bayar tidak sesuai dengan kategori pasien.';
    END IF;

    SET p_total_tagihan = fn_Hitung_Total_Tagihan(p_id_daftar);

    IF EXISTS (SELECT 1 FROM Pembayaran WHERE id_daftar = p_id_daftar) THEN
        UPDATE Pembayaran
        SET    metode_bayar = p_metode_bayar,
               status_bayar = 'Lunas'
        WHERE  id_daftar    = p_id_daftar;

        SELECT id_pembayaran INTO p_id_pembayaran
        FROM   Pembayaran
        WHERE  id_daftar = p_id_daftar;
    ELSE
        INSERT INTO Pembayaran (id_daftar, metode_bayar, status_bayar)
        VALUES (p_id_daftar, p_metode_bayar, 'Lunas');
        SET p_id_pembayaran = LAST_INSERT_ID();
    END IF;

    SET p_pesan = CONCAT(
        'Pembayaran LUNAS. ID Pembayaran: ', p_id_pembayaran,
        ' | Total: Rp ', p_total_tagihan, ' | Metode: ', p_metode_bayar, '.'
    );

    COMMIT;
END //
DELIMITER ;
```

**Penjelasan langkah demi langkah:**

1. **Step 1 — Validasi pendaftaran**: `SELECT INTO` + cek NULL (not found). Tolak jika 'Batal' atau sudah 'Lunas'.
2. **Step 2 — Validasi metode bayar**: Panggil `fn_Validasi_Metode_Bayar` (guard BPJS).
3. **Step 3 — Kalkulasi tagihan**: Panggil `fn_Hitung_Total_Tagihan`. BPJS = Rp 0.
4. **Step 4 — Simpan pembayaran**: MySQL tidak support `RETURNING ... INTO` pada UPDATE, jadi pakai `SELECT ... INTO` terpisah setelah UPDATE. Pada INSERT, pakai `LAST_INSERT_ID()`.
5. Trigger `trg_validasi_metode_bayar_insert`/`_update`, `trg_update_status_pendaftaran`, dan `trg_audit_perubahan_pembayaran` aktif otomatis.
6. `CONCAT` menggantikan `FORMAT` PostgreSQL.

---

### 4.4 sp_Reschedule_Pendaftaran

```sql
DELIMITER //
CREATE PROCEDURE sp_Reschedule_Pendaftaran(
    IN  p_id_daftar      INT,
    IN  p_id_jadwal_baru INT,
    OUT p_pesan           VARCHAR(500)
)
BEGIN
    DECLARE v_id_jadwal_lama INT;
    DECLARE v_status_daftar  VARCHAR(20);
    DECLARE v_antrean_baru   INT;

    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        RESIGNAL;
    END;

    START TRANSACTION;

    SELECT id_jadwal, status_pendaftaran
    INTO   v_id_jadwal_lama, v_status_daftar
    FROM   Pendaftaran
    WHERE  id_daftar = p_id_daftar;

    IF v_id_jadwal_lama IS NULL THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'sp_Reschedule: Pendaftaran tidak ditemukan.';
    END IF;

    IF v_status_daftar IN ('Selesai', 'Diperiksa') THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'Pendaftaran tidak bisa di-reschedule (status Selesai/Diperiksa).';
    END IF;

    IF fn_Cek_Ketersediaan_Jadwal(p_id_jadwal_baru) = 0 THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'Jadwal baru tidak tersedia atau sudah penuh.';
    END IF;

    SELECT COALESCE(MAX(nomor_antrean), 0) + 1 INTO v_antrean_baru
    FROM   Pendaftaran
    WHERE  id_jadwal = p_id_jadwal_baru
      AND  id_daftar != p_id_daftar;

    UPDATE Pendaftaran
    SET    id_jadwal           = p_id_jadwal_baru,
           status_pendaftaran  = 'Menunggu',
           tanggal_pendaftaran = CURRENT_TIMESTAMP,
           nomor_antrean       = v_antrean_baru
    WHERE  id_daftar = p_id_daftar;

    SET p_pesan = CONCAT(
        'Reschedule berhasil. ID Daftar: ', p_id_daftar,
        ' | Dari Jadwal ID: ', v_id_jadwal_lama, ' → Jadwal ID: ', p_id_jadwal_baru, '.'
    );

    COMMIT;
END //
DELIMITER ;
```

**Penjelasan langkah demi langkah:**

1. **Step 1 — Ambil data saat ini**: Ambil jadwal lama dan status. Cek NULL untuk not-found.
2. **Validasi status**: Tolak jika 'Selesai' atau 'Diperiksa'.
3. **Step 2 — Validasi jadwal baru**: Panggil `fn_Cek_Ketersediaan_Jadwal`.
4. **Perbaikan efisiensi**: Nomor antrean dihitung lebih dulu ke variabel `v_antrean_baru` terpisah, lalu dipakai di UPDATE. PostgreSQL versi lama menggunakan correlated subquery di dalam SET clause yang bisa kurang optimal pada beberapa query planner.
5. **Step 3 — Pindahkan jadwal**: UPDATE pendaftaran ke jadwal baru, reset status dan timestamp.
6. Trigger `trg_buka_kembali_jadwal` memeriksa jadwal lama untuk kemungkinan dibuka kembali.

---

### 4.5 sp_Update_Status_Pemeriksaan

```sql
DELIMITER //
CREATE PROCEDURE sp_Update_Status_Pemeriksaan(
    IN  p_id_daftar   INT,
    IN  p_status_baru VARCHAR(20),
    IN  p_id_admin    INT,
    OUT p_pesan       VARCHAR(500)
)
BEGIN
    DECLARE v_status_lama    VARCHAR(20);
    DECLARE v_valid_transisi TINYINT(1) DEFAULT 0;

    SELECT status_pendaftaran INTO v_status_lama
    FROM   Pendaftaran
    WHERE  id_daftar = p_id_daftar;

    IF v_status_lama IS NULL THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'sp_Update_Status: Pendaftaran tidak ditemukan.';
    END IF;

    IF v_status_lama = 'Menunggu' AND p_status_baru IN ('Diperiksa', 'Batal') THEN
        SET v_valid_transisi = 1;
    ELSEIF v_status_lama = 'Diperiksa' AND p_status_baru IN ('Selesai', 'Batal') THEN
        SET v_valid_transisi = 1;
    END IF;

    IF v_valid_transisi = 0 THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'Transisi status tidak valid.';
    END IF;

    UPDATE Pendaftaran
    SET    status_pendaftaran = p_status_baru
    WHERE  id_daftar          = p_id_daftar;

    SET p_pesan = CONCAT(
        'Status pendaftaran ID ', p_id_daftar, ' diubah: "', v_status_lama,
        '" → "', p_status_baru, '" oleh Admin ID ', p_id_admin, '.'
    );
END //
DELIMITER ;
```

**Penjelasan langkah demi langkah:**

1. Ambil status saat ini. Cek NULL untuk not-found.
2. **State machine** — hanya transisi valid:
   - `Menunggu` → `Diperiksa` atau `Batal`
   - `Diperiksa` → `Selesai` atau `Batal`
   - `Selesai` dan `Batal` = terminal state
3. `TINYINT(1)` menggantikan `BOOLEAN` untuk flag validasi.
4. `ELSEIF` — MySQL syntax (PostgreSQL = `ELSIF`).
5. Tidak perlu transaksi eksplisit karena hanya satu UPDATE statement.

---

### 4.6 sp_Get_Statistik_Harian

```sql
DELIMITER //
CREATE PROCEDURE sp_Get_Statistik_Harian(
    IN  p_tanggal          DATE,
    OUT p_total_pendaftaran INT,
    OUT p_total_selesai     INT,
    OUT p_total_batal       INT,
    OUT p_total_menunggu    INT,
    OUT p_total_pendapatan  DECIMAL(15, 2),
    OUT p_total_rujukan     INT
)
BEGIN
    SELECT
        COUNT(*),
        SUM(p.status_pendaftaran = 'Selesai'),
        SUM(p.status_pendaftaran = 'Batal'),
        SUM(p.status_pendaftaran IN ('Menunggu', 'Diperiksa'))
    INTO p_total_pendaftaran, p_total_selesai, p_total_batal, p_total_menunggu
    FROM   Pendaftaran p
    JOIN   Jadwal_Dokter jd ON p.id_jadwal = jd.id_jadwal
    WHERE  jd.hari_tanggal = p_tanggal;

    SELECT COALESCE(SUM(l.tarif_total_layanan), 0) INTO p_total_pendapatan
    FROM   Pendaftaran p
    JOIN   Jadwal_Dokter jd    ON p.id_jadwal   = jd.id_jadwal
    JOIN   Pasien pas          ON p.id_pasien   = pas.id_pasien
    JOIN   Pembayaran py       ON p.id_daftar   = py.id_daftar
    JOIN   Pendaftaran_Layanan pl ON p.id_daftar = pl.id_daftar
    JOIN   Layanan l           ON pl.id_layanan  = l.id_layanan
    WHERE  jd.hari_tanggal     = p_tanggal
      AND  py.status_bayar     = 'Lunas'
      AND  pas.kategori_pasien != 'BPJS';

    SELECT COUNT(DISTINCT r.id_rujukan) INTO p_total_rujukan
    FROM   Rujukan r
    JOIN   Skrining sk ON r.id_skrining = sk.id_skrining
    WHERE  DATE(sk.tanggal_skrining) = p_tanggal;
END //
DELIMITER ;
```

**Penjelasan langkah demi langkah:**

1. **Dikonversi dari function ke procedure** — MySQL function tidak bisa return TABLE. Procedure dengan OUT parameter sebagai gantinya.
2. **Perbaikan efisiensi utama**: Versi PostgreSQL menggabungkan semua data dalam satu query besar dengan 7 JOIN (termasuk LEFT JOIN). Ini menyebabkan **cross-join multiplication** — jika satu pendaftaran punya 3 layanan DAN pasien punya 2 skrining, baris termultiplikasi jadi 6x.
3. **Solusi: 3 query terpisah** yang masing-masing hanya JOIN tabel yang dibutuhkan:
   - Query 1: Count pendaftaran + status breakdown (2 tabel saja).
   - Query 2: Total pendapatan (5 tabel, tapi hanya JOIN yang relevan untuk revenue).
   - Query 3: Total rujukan (2 tabel saja).
4. `SUM(condition)` — MySQL shorthand. Ekspresi boolean menghasilkan 1 (true) atau 0 (false), dijumlahkan langsung. Menggantikan `COUNT(*) FILTER (WHERE ...)` PostgreSQL.
5. `INNER JOIN` dipakai (bukan LEFT JOIN) di query 2 karena kita hanya butuh pendaftaran yang sudah punya pembayaran Lunas.
6. Pemanggilan: `CALL sp_Get_Statistik_Harian('2025-01-15', @a, @b, @c, @d, @e, @f);`

---

## 5. Triggers

### 5.1 Trigger 1 — Sinkronisasi Status Pendaftaran dari Pembayaran

```sql
DELIMITER //
CREATE TRIGGER trg_update_status_pendaftaran
AFTER UPDATE ON Pembayaran
FOR EACH ROW
BEGIN
    IF NOT (OLD.status_bayar <=> NEW.status_bayar) THEN
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
    END IF;
END //
DELIMITER ;
```

**Penjelasan langkah demi langkah:**

1. `AFTER UPDATE` — MySQL tidak support `AFTER UPDATE OF column`, sehingga trigger fire pada setiap UPDATE tabel `Pembayaran`.
2. `NOT (OLD.status_bayar <=> NEW.status_bayar)` — Operator `<=>` adalah NULL-safe equality MySQL. `NOT <=>` setara dengan `IS DISTINCT FROM` PostgreSQL. Guard ini memastikan trigger hanya proses jika status_bayar benar-benar berubah.
3. Pembayaran 'Gagal' → pendaftaran otomatis 'Batal'.
4. Pembayaran Pending → Lunas → pendaftaran diaktifkan ke 'Menunggu'.

---

### 5.2 Trigger 2 — Blokir Pendaftaran ke Jadwal Tidak Aktif

```sql
DELIMITER //
CREATE TRIGGER trg_blokir_daftar_ke_jadwal_nonaktif
BEFORE INSERT ON Pendaftaran
FOR EACH ROW
BEGIN
    DECLARE v_status_jadwal VARCHAR(20);

    SELECT status_jadwal INTO v_status_jadwal
    FROM   Jadwal_Dokter
    WHERE  id_jadwal = NEW.id_jadwal;

    IF v_status_jadwal = 'Batal' THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'DITOLAK: Jadwal sudah dibatalkan.';
    END IF;

    IF v_status_jadwal = 'Penuh' THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'DITOLAK: Jadwal sudah penuh.';
    END IF;
END //
DELIMITER ;
```

**Penjelasan langkah demi langkah:**

1. `BEFORE INSERT` pada `Pendaftaran` — validasi sebelum data masuk.
2. Ambil status jadwal yang dipilih.
3. Jadwal 'Batal' atau 'Penuh' → tolak pendaftaran via SIGNAL.
4. Trigger ini dibuat **sebelum** `trg_set_nomor_antrean` sehingga MySQL menjalankannya lebih dulu (urutan berdasarkan waktu pembuatan).
5. Lapisan validasi kedua di level database sebagai perlindungan terhadap race condition.

---

### 5.3 Trigger 3 — Auto-Generate Nomor Antrean

```sql
DELIMITER //
CREATE TRIGGER trg_set_nomor_antrean
BEFORE INSERT ON Pendaftaran
FOR EACH ROW
FOLLOWS trg_blokir_daftar_ke_jadwal_nonaktif
BEGIN
    DECLARE v_next_antrean INT;

    SELECT id_jadwal INTO @_lock_jadwal
    FROM   Jadwal_Dokter
    WHERE  id_jadwal = NEW.id_jadwal
    FOR UPDATE;

    SELECT COALESCE(MAX(nomor_antrean), 0) + 1 INTO v_next_antrean
    FROM   Pendaftaran
    WHERE  id_jadwal = NEW.id_jadwal;

    SET NEW.nomor_antrean = v_next_antrean;
END //
DELIMITER ;
```

**Penjelasan langkah demi langkah:**

1. `FOLLOWS trg_blokir_daftar_ke_jadwal_nonaktif` — MySQL 8.0 syntax untuk mengatur urutan eksekusi trigger. Trigger ini berjalan **setelah** validasi jadwal selesai.
2. `SELECT ... FOR UPDATE` — Row-level lock pada `Jadwal_Dokter`. Mencegah race condition saat dua pasien mendaftar bersamaan pada jadwal yang sama.
3. `@_lock_jadwal` — User variable throwaway (pengganti `PERFORM` PostgreSQL).
4. Hitung MAX(nomor_antrean) + 1 pada jadwal yang sama.
5. `SET NEW.nomor_antrean` — Modifikasi data sebelum INSERT dieksekusi.

---

### 5.4 Trigger 4 — Jadwal Otomatis "Penuh"

```sql
DELIMITER //
CREATE TRIGGER trg_cek_kuota_jadwal
AFTER INSERT ON Pendaftaran
FOR EACH ROW
BEGIN
    DECLARE v_jumlah_pasien INT;

    SELECT COUNT(*) INTO v_jumlah_pasien
    FROM   Pendaftaran
    WHERE  id_jadwal          = NEW.id_jadwal
      AND  status_pendaftaran != 'Batal';

    IF v_jumlah_pasien >= 30 THEN
        UPDATE Jadwal_Dokter
        SET    status_jadwal = 'Penuh'
        WHERE  id_jadwal     = NEW.id_jadwal
          AND  status_jadwal = 'Tersedia';
    END IF;
END //
DELIMITER ;
```

**Penjelasan langkah demi langkah:**

1. `AFTER INSERT` pada `Pendaftaran` — evaluasi setelah data masuk.
2. Hitung pendaftaran aktif (bukan Batal) pada jadwal yang baru diinsert.
3. Jika jumlah ≥ 30 → set status jadwal menjadi 'Penuh'.
4. Guard `AND status_jadwal = 'Tersedia'` mencegah overwrite status 'Batal'.

---

### 5.5 Trigger 5a — Rujukan Otomatis dari Skrining (INSERT)

```sql
DELIMITER //
CREATE TRIGGER trg_buat_draft_rujukan_insert
AFTER INSERT ON Skrining
FOR EACH ROW
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
END //
DELIMITER ;
```

### 5.6 Trigger 5b — Rujukan Otomatis dari Skrining (UPDATE)

```sql
DELIMITER //
CREATE TRIGGER trg_buat_draft_rujukan_update
AFTER UPDATE ON Skrining
FOR EACH ROW
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
END //
DELIMITER ;
```

**Penjelasan langkah demi langkah (5a & 5b):**

1. **Dipecah jadi 2 trigger** — MySQL tidak support `AFTER INSERT OR UPDATE` dalam satu trigger. PostgreSQL bisa, MySQL harus terpisah.
2. Logika identik: jika status = 'Perlu Rujukan' → buat draft rujukan.
3. Guard `NOT EXISTS` mencegah duplikasi.
4. Tujuan rujukan di-set 'Belum Ditentukan' — admin mengisi manual.

---

### 5.7 Trigger 6a — Validasi Metode Bayar (INSERT)

```sql
DELIMITER //
CREATE TRIGGER trg_validasi_metode_bayar_insert
BEFORE INSERT ON Pembayaran
FOR EACH ROW
BEGIN
    DECLARE v_kategori VARCHAR(50);

    SELECT p.kategori_pasien INTO v_kategori
    FROM   Pendaftaran dft
    JOIN   Pasien p ON dft.id_pasien = p.id_pasien
    WHERE  dft.id_daftar = NEW.id_daftar;

    IF v_kategori = 'BPJS' AND NEW.metode_bayar != 'BPJS' THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'VALIDASI: Pasien BPJS harus menggunakan metode bayar BPJS.';
    END IF;

    IF v_kategori != 'BPJS' AND NEW.metode_bayar = 'BPJS' THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'VALIDASI: Pasien non-BPJS tidak dapat menggunakan metode bayar BPJS.';
    END IF;
END //
DELIMITER ;
```

### 5.8 Trigger 6b — Validasi Metode Bayar (UPDATE)

```sql
DELIMITER //
CREATE TRIGGER trg_validasi_metode_bayar_update
BEFORE UPDATE ON Pembayaran
FOR EACH ROW
BEGIN
    DECLARE v_kategori VARCHAR(50);

    IF NOT (OLD.metode_bayar <=> NEW.metode_bayar) THEN
        SELECT p.kategori_pasien INTO v_kategori
        FROM   Pendaftaran dft
        JOIN   Pasien p ON dft.id_pasien = p.id_pasien
        WHERE  dft.id_daftar = NEW.id_daftar;

        IF v_kategori = 'BPJS' AND NEW.metode_bayar != 'BPJS' THEN
            SIGNAL SQLSTATE '45000'
                SET MESSAGE_TEXT = 'VALIDASI: Pasien BPJS harus menggunakan metode bayar BPJS.';
        END IF;

        IF v_kategori != 'BPJS' AND NEW.metode_bayar = 'BPJS' THEN
            SIGNAL SQLSTATE '45000'
                SET MESSAGE_TEXT = 'VALIDASI: Pasien non-BPJS tidak dapat menggunakan metode bayar BPJS.';
        END IF;
    END IF;
END //
DELIMITER ;
```

**Penjelasan langkah demi langkah (6a & 6b):**

1. **Dipecah jadi 2 trigger** — MySQL tidak support `BEFORE INSERT OR UPDATE` dalam satu trigger.
2. **Trigger UPDATE punya guard tambahan**: `NOT (OLD.metode_bayar <=> NEW.metode_bayar)` — hanya validasi jika metode bayar benar-benar berubah. Ini menghindari JOIN yang tidak perlu saat hanya status_bayar yang berubah.
3. Rule 1: Pasien BPJS + metode bukan BPJS → SIGNAL (tolak).
4. Rule 2: Pasien non-BPJS + metode BPJS → SIGNAL (tolak).
5. **Last-line defense** di level database.

---

### 5.9 Trigger 7 — Buka Kembali Jadwal saat Pembatalan

```sql
DELIMITER //
CREATE TRIGGER trg_buka_kembali_jadwal
AFTER UPDATE ON Pendaftaran
FOR EACH ROW
BEGIN
    DECLARE v_jumlah_aktif  INT;
    DECLARE v_status_jadwal VARCHAR(20);

    IF NEW.status_pendaftaran = 'Batal' AND OLD.status_pendaftaran != 'Batal' THEN

        SELECT status_jadwal INTO v_status_jadwal
        FROM   Jadwal_Dokter
        WHERE  id_jadwal = NEW.id_jadwal;

        IF v_status_jadwal = 'Penuh' THEN
            SELECT COUNT(*) INTO v_jumlah_aktif
            FROM   Pendaftaran
            WHERE  id_jadwal          = NEW.id_jadwal
              AND  status_pendaftaran != 'Batal';

            IF v_jumlah_aktif < 30 THEN
                UPDATE Jadwal_Dokter
                SET    status_jadwal = 'Tersedia'
                WHERE  id_jadwal     = NEW.id_jadwal;
            END IF;
        END IF;
    END IF;
END //
DELIMITER ;
```

**Penjelasan langkah demi langkah:**

1. `AFTER UPDATE` pada `Pendaftaran` — fire setiap ada perubahan.
2. Guard: hanya proses jika status berubah **ke** 'Batal' dari status lain.
3. **Perbaikan efisiensi**: Cek `v_status_jadwal = 'Penuh'` **sebelum** menghitung COUNT. Jika jadwal bukan 'Penuh', tidak perlu query COUNT sama sekali. PostgreSQL versi lama selalu menjalankan COUNT.
4. Jika jadwal 'Penuh' dan antrean aktif < 30 → kembalikan ke 'Tersedia'.
5. Melengkapi Trigger 4 (lock penuh) membentuk siklus: Penuh ↔ Tersedia.

---

### 5.10 Trigger 8 — Audit Log Perubahan Pembayaran

```sql
DELIMITER //
CREATE TRIGGER trg_audit_perubahan_pembayaran
AFTER UPDATE ON Pembayaran
FOR EACH ROW
FOLLOWS trg_update_status_pendaftaran
BEGIN
    IF NOT (OLD.status_bayar <=> NEW.status_bayar)
    OR NOT (OLD.metode_bayar <=> NEW.metode_bayar) THEN

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
            CONCAT('Perubahan direkam pada ', NOW())
        );
    END IF;
END //
DELIMITER ;
```

**Penjelasan langkah demi langkah:**

1. `FOLLOWS trg_update_status_pendaftaran` — Eksplisit menyatakan trigger ini berjalan **setelah** trigger sinkronisasi status. Penting untuk urutan yang benar.
2. `NOT (OLD.col <=> NEW.col)` — Pengganti `IS DISTINCT FROM`. Operator `<=>` menangani NULL dengan benar (NULL <=> NULL = TRUE).
3. Guard IF: hanya catat audit jika ada perubahan nyata pada status atau metode bayar.
4. INSERT ke `Log_Pembayaran` dengan nilai lama dan baru.
5. `NOW()` di MySQL setara `NOW()` di PostgreSQL.

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

1. Menampilkan antrean pasien aktif (Menunggu + Diperiksa).
2. JOIN 5 tabel: Pendaftaran → Jadwal_Dokter → Dokter → Poli → Pasien.
3. Kolom `estimasi_dipanggil` memanggil `fn_Hitung_Estimasi_Tunggu` per baris.
4. Urut berdasarkan jadwal lalu nomor antrean.
5. Penggunaan: `SELECT * FROM v_Antrian_Aktif WHERE hari_tanggal = CURDATE();`

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
    SUM(CASE WHEN p.status_pendaftaran IS NOT NULL
              AND p.status_pendaftaran != 'Batal'
             THEN 1 ELSE 0 END)                            AS total_aktif,
    SUM(CASE WHEN p.status_pendaftaran = 'Selesai'
             THEN 1 ELSE 0 END)                            AS total_selesai,
    SUM(CASE WHEN p.status_pendaftaran = 'Diperiksa'
             THEN 1 ELSE 0 END)                            AS sedang_diperiksa,
    SUM(CASE WHEN p.status_pendaftaran = 'Menunggu'
             THEN 1 ELSE 0 END)                            AS total_menunggu,
    SUM(CASE WHEN p.status_pendaftaran = 'Batal'
             THEN 1 ELSE 0 END)                            AS total_batal
FROM   Jadwal_Dokter jd
JOIN   Dokter d    ON jd.id_dokter = d.id_dokter
JOIN   Poli pl     ON d.id_poli    = pl.id_poli
LEFT JOIN Pendaftaran p ON p.id_jadwal = jd.id_jadwal
GROUP  BY jd.id_jadwal, jd.hari_tanggal, jd.jam_mulai, jd.jam_selesai,
          jd.status_jadwal, d.nama_dokter, pl.nama_poli
ORDER  BY jd.hari_tanggal DESC, jd.jam_mulai;
```

**Penjelasan langkah demi langkah:**

1. Ringkasan operasional per jadwal per hari.
2. `SUM(CASE WHEN ... THEN 1 ELSE 0 END)` — Pengganti `COUNT(*) FILTER (WHERE ...)` PostgreSQL yang tidak didukung MySQL.
3. LEFT JOIN Pendaftaran agar jadwal tanpa pendaftaran tetap muncul.
4. `total_aktif` menambahkan guard `IS NOT NULL` untuk menangani NULL dari LEFT JOIN.
5. GROUP BY per jadwal, ORDER BY tanggal terbaru dulu.
6. Penggunaan: `SELECT * FROM v_Resume_Harian_Jadwal WHERE hari_tanggal = CURDATE();`

---

## 7. Ringkasan Perubahan PostgreSQL → MySQL

| Aspek | PostgreSQL | MySQL |
|---|---|---|
| Auto-increment | `SERIAL` | `INT AUTO_INCREMENT` |
| Desimal | `NUMERIC(15,2)` | `DECIMAL(15,2)` |
| Boolean | `BOOLEAN` | `TINYINT(1)` |
| Error handling | `RAISE EXCEPTION` | `SIGNAL SQLSTATE '45000'` |
| Return last ID | `RETURNING ... INTO` | `LAST_INSERT_ID()` |
| NULL-safe compare | `IS DISTINCT FROM` | `NOT <=>` |
| String format | `FORMAT('%s', var)` | `CONCAT('', var)` |
| Conditional count | `COUNT(*) FILTER(WHERE ...)` | `SUM(CASE WHEN ... END)` |
| Multi-event trigger | `INSERT OR UPDATE` | 2 trigger terpisah |
| Column trigger | `UPDATE OF col` | `IF` di dalam trigger |
| Trigger order | Alfabet nama | `FOLLOWS` / `PRECEDES` |
| Table-returning fn | `RETURNS TABLE(...)` | Procedure + OUT params |
| Delimiter | `$$ ... $$` | `DELIMITER //` |
| Transaksi | Implicit via caller | Explicit `START TRANSACTION` |
| Interval aritmatika | String concat + cast | `ADDTIME` + `MAKETIME` |

---

## 8. Ringkasan Perbaikan Efisiensi

| Komponen | Sebelum (PostgreSQL) | Sesudah (MySQL) | Dampak |
|---|---|---|---|
| `sp_Pembatalan_Jadwal_Dokter` | Loop UPDATE satu per satu (O(N) query) | Batch UPDATE tunggal + `ROW_COUNT()` | ~30x lebih cepat untuk jadwal penuh |
| `sp_Get_Statistik_Harian` | 1 query dengan 7 JOIN → cross-join multiplication | 3 query terpisah, masing-masing JOIN minimal | Hasil akurat, tidak ada baris duplikat |
| `fn_Hitung_Estimasi_Tunggu` | String concat + `::INTERVAL` cast setiap panggilan | `ADDTIME` + `MAKETIME` langsung | Tidak ada parsing string, lebih cepat |
| `trg_buka_kembali_jadwal` | Selalu hitung COUNT | Cek status dulu, COUNT hanya jika 'Penuh' | Skip query yang tidak perlu |
| `trg_validasi_metode_bayar_update` | Selalu JOIN (PostgreSQL) | Guard `<=>` skip JOIN jika metode tidak berubah | Skip JOIN yang tidak perlu |
| Transaksi | Bergantung pada caller | Explicit `START TRANSACTION` + handler | Atomisitas terjamin |

---

## 9. Alur Trigger per Skenario

```
DAFTAR PASIEN BARU
  INSERT Pendaftaran
    → [BEFORE] trg_blokir_daftar_ke_jadwal_nonaktif  ← validasi jadwal
    → [BEFORE] trg_set_nomor_antrean (FOLLOWS)        ← assign nomor antrean
    → [AFTER]  trg_cek_kuota_jadwal                   ← kunci jadwal jika penuh

PEMBAYARAN (INSERT)
  INSERT Pembayaran
    → [BEFORE] trg_validasi_metode_bayar_insert        ← validasi BPJS

PEMBAYARAN (UPDATE)
  UPDATE Pembayaran
    → [BEFORE] trg_validasi_metode_bayar_update         ← validasi BPJS
    → [AFTER]  trg_update_status_pendaftaran            ← sinkronisasi status
    → [AFTER]  trg_audit_perubahan_pembayaran (FOLLOWS) ← catat audit log

PEMBATALAN PENDAFTARAN
  UPDATE Pendaftaran SET status = 'Batal'
    → [AFTER]  trg_buka_kembali_jadwal                 ← buka slot jadwal

SKRINING (INSERT)
  INSERT Skrining SET status = 'Perlu Rujukan'
    → [AFTER]  trg_buat_draft_rujukan_insert           ← buat draft rujukan

SKRINING (UPDATE)
  UPDATE Skrining SET status = 'Perlu Rujukan'
    → [AFTER]  trg_buat_draft_rujukan_update           ← buat draft rujukan
```
