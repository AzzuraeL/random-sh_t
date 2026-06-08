
## 1. DDL — Tabel Master & Operasional

```sql
DROP TABLE IF EXISTS Log_Pembayaran;
DROP TABLE IF EXISTS Pembayaran;
DROP TABLE IF EXISTS Layanan;
DROP TABLE IF EXISTS Rujukan;
DROP TABLE IF EXISTS Skrining;
DROP TABLE IF EXISTS Pendaftaran;
DROP TABLE IF EXISTS Pasien;
DROP TABLE IF EXISTS Petugas_Admin;
DROP TABLE IF EXISTS Jadwal_Dokter;
DROP TABLE IF EXISTS Dokter;
DROP TABLE IF EXISTS Poli;

CREATE TABLE Poli (
    id_poli       INT AUTO_INCREMENT PRIMARY KEY,
    nama_poli     VARCHAR(20) NOT NULL,
    nomor_ruangan VARCHAR(20) NOT NULL
) ENGINE=InnoDB;

CREATE TABLE Petugas_Admin (
    id_admin     INT AUTO_INCREMENT PRIMARY KEY,
    nama_petugas VARCHAR(30) NOT NULL,
    shift_tugas  VARCHAR(20) CHECK (shift_tugas IN ('Pagi', 'Siang', 'Malam'))
) ENGINE=InnoDB;

CREATE TABLE Pasien (
    id_pasen        INT AUTO_INCREMENT PRIMARY KEY,
    nim_nik         VARCHAR(20)  UNIQUE NOT NULL,
    nama            VARCHAR(30)  NOT NULL,
    asal_faskes     VARCHAR(30),
    kategori_pasien VARCHAR(30)  CHECK (kategori_pasien IN ('Umum', 'BPJS', 'Asuransi Lain')),
    no_tlp          VARCHAR(15)
) ENGINE=InnoDB;

CREATE TABLE Dokter (
    id_dokter    INT AUTO_INCREMENT PRIMARY KEY,
    nama_dokter  VARCHAR(30) NOT NULL,
    Poli_id_poli INT         NOT NULL,
    CONSTRAINT fk_dokter_poli FOREIGN KEY (Poli_id_poli) REFERENCES Poli(id_poli) ON DELETE RESTRICT
) ENGINE=InnoDB;

CREATE TABLE Jadwal_Dokter (
    id_jadwal        INT AUTO_INCREMENT PRIMARY KEY,
    hari_tanggal     DATE NOT NULL,
    jam_mulai        TIME NOT NULL,
    jam_selesai      TIME NOT NULL,
    status_jadwal    VARCHAR(20) DEFAULT 'Tersedia' CHECK (status_jadwal IN ('Tersedia', 'Penuh', 'Batal')),
    Dokter_id_dokter INT  NOT NULL,
    CONSTRAINT fk_jadwal_dokter FOREIGN KEY (Dokter_id_dokter) REFERENCES Dokter(id_dokter) ON DELETE CASCADE
) ENGINE=InnoDB;

CREATE TABLE Skrining (
    id_skrining      INT AUTO_INCREMENT PRIMARY KEY,
    tanggal_skrining DATE,
    hasil_skrining   VARCHAR(20),
    status_kelayakan VARCHAR(20) CHECK (status_kelayakan IN ('Layak Ditangani', 'Perlu Rujukan', 'Kritis')),
    Pasien_id_pasen  INT NOT NULL,
    CONSTRAINT fk_skrining_pasien FOREIGN KEY (Pasien_id_pasen) REFERENCES Pasien(id_pasen) ON DELETE CASCADE
) ENGINE=InnoDB;

CREATE TABLE Rujukan (
    id_rujukan           INT AUTO_INCREMENT PRIMARY KEY,
    asal_rujukan         VARCHAR(20),
    tujuan_rujukan       VARCHAR(20) NOT NULL,
    alasan_rujukan       VARCHAR(20) NOT NULL,
    tanggal_rujukan      DATE,
    Skrining_id_skrining INT UNIQUE NOT NULL,
    CONSTRAINT fk_rujukan_skrining FOREIGN KEY (Skrining_id_skrining) REFERENCES Skrining(id_skrining) ON DELETE CASCADE
) ENGINE=InnoDB;

CREATE TABLE Pendaftaran (
    id_daftar               INT AUTO_INCREMENT PRIMARY KEY,
    tanggal_pendaftaran     DATE,
    status_pendaftaran      VARCHAR(20) DEFAULT 'Menunggu' CHECK (status_pendaftaran IN ('Menunggu', 'Diperiksa', 'Selesai', 'Batal')),
    nomor_antrean           VARCHAR(10),
    Jadwal_Dokter_id_jadwal INT NOT NULL,
    Pasien_id_pasen         INT NOT NULL,
    Petugas_Admin_id_admin  INT NOT NULL,
    CONSTRAINT fk_daftar_jadwal FOREIGN KEY (Jadwal_Dokter_id_jadwal) REFERENCES Jadwal_Dokter(id_jadwal) ON DELETE RESTRICT,
    CONSTRAINT fk_daftar_pasien FOREIGN KEY (Pasien_id_pasen) REFERENCES Pasien(id_pasen) ON DELETE RESTRICT,
    CONSTRAINT fk_daftar_admin  FOREIGN KEY (Petugas_Admin_id_admin) REFERENCES Petugas_Admin(id_admin)
) ENGINE=InnoDB;

CREATE TABLE Layanan (
    id_layanan            INT AUTO_INCREMENT PRIMARY KEY,
    nama_layanan          VARCHAR(30)   NOT NULL,
    tarif_total_layanan   DECIMAL(12, 2) NOT NULL DEFAULT 0,
    Pendaftaran_id_daftar INT NOT NULL,
    CONSTRAINT fk_layanan_daftar FOREIGN KEY (Pendaftaran_id_daftar) REFERENCES Pendaftaran(id_daftar) ON DELETE CASCADE
) ENGINE=InnoDB;

CREATE TABLE Pembayaran (
    id_pembayaran        INT AUTO_INCREMENT PRIMARY KEY,
    metode_bayar         VARCHAR(30) CHECK (metode_bayar IN ('Tunai', 'Transfer', 'E-Wallet', 'BPJS')),
    status_bayar         VARCHAR(20) DEFAULT 'Pending' CHECK (status_bayar IN ('Pending', 'Lunas', 'Gagal')),
    Pendaftaran_id_dafta INT UNIQUE NOT NULL,
    CONSTRAINT fk_bayar_daftar FOREIGN KEY (Pendaftaran_id_dafta) REFERENCES Pendaftaran(id_daftar) ON DELETE CASCADE
) ENGINE=InnoDB;

```
**Penjelasan langkah demi langkah:**
 1. **DROP TABLE** — Dihapus satu per satu sesuai urutan dependensi (child dulu, parent belakangan). MySQL tidak support multi-table DROP dengan CASCADE seperti PostgreSQL.
 2. **ENGINE=InnoDB** — Wajib untuk support foreign key, transaksi (COMMIT/ROLLBACK), dan row-level locking.
 3. **AUTO_INCREMENT** — Pengganti SERIAL PostgreSQL. Otomatis increment setiap INSERT.
 4. **DECIMAL(12,2)** — Pengganti NUMERIC PostgreSQL untuk presisi bilangan desimal, disesuaikan batasnya berdasarkan kapasitas PDM.
 5. **CHECK constraint** — Didukung penuh di MySQL 8.0.16+. Memvalidasi nilai kolom sebelum INSERT/UPDATE.
 6. **Named FOREIGN KEY** — Setiap FK diberi nama eksplisit (CONSTRAINT fk_...) agar mudah di-debug jika terjadi pelanggaran referensi.
 7. **Struktur tabel disesuaikan dengan PDM** — 4 tabel master (Poli, Layanan, Petugas_Admin, Pasien), 4 tabel operasional medis (Dokter, Jadwal_Dokter, Skrining, Rujukan), dan 3 tabel transaksional (Pendaftaran, Pembayaran, Log_Pembayaran). Tabel perantara Pendaftaran_Layanan dihapus karena Layanan kini merujuk langsung ke Pendaftaran.
## 2. Tabel Audit Log
```sql
CREATE TABLE Log_Pembayaran (
    id_log               INT AUTO_INCREMENT PRIMARY KEY,
    id_pembayaran        INT         NOT NULL,
    Pendaftaran_id_dafta INT         NOT NULL,
    status_lama          VARCHAR(20),
    status_baru          VARCHAR(20),
    metode_lama          VARCHAR(30),
    metode_baru          VARCHAR(30),
    waktu_perubahan      TIMESTAMP   DEFAULT CURRENT_TIMESTAMP,
    keterangan           TEXT
) ENGINE=InnoDB;

```
**Penjelasan langkah demi langkah:**
 1. Menyimpan jejak setiap perubahan pada tabel Pembayaran.
 2. status_lama/status_baru merekam transisi status pembayaran.
 3. metode_lama/metode_baru merekam perubahan metode bayar.
 4. waktu_perubahan otomatis terisi timestamp.
 5. Diisi oleh Trigger audit, bukan manual.
## 3. Functions
### 3.1 fn_Hitung_Total_Tagihan
```sql
DELIMITER //
CREATE FUNCTION fn_Hitung_Total_Tagihan(p_id_daftar INT)
RETURNS DECIMAL(12, 2)
READS SQL DATA
BEGIN
    DECLARE v_total    DECIMAL(12, 2) DEFAULT 0;
    DECLARE v_kategori VARCHAR(30);

    IF NOT EXISTS (SELECT 1 FROM Pendaftaran WHERE id_daftar = p_id_daftar) THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'fn_Hitung_Total_Tagihan: Pendaftaran tidak ditemukan.';
    END IF;

    SELECT p.kategori_pasien INTO v_kategori
    FROM   Pendaftaran dft
    JOIN   Pasien p ON dft.Pasien_id_pasen = p.id_pasen
    WHERE  dft.id_daftar = p_id_daftar;

    IF v_kategori = 'BPJS' THEN
        RETURN 0.00;
    END IF;

    SELECT COALESCE(SUM(tarif_total_layanan), 0) INTO v_total
    FROM   Layanan
    WHERE  Pendaftaran_id_daftar = p_id_daftar;

    RETURN v_total;
END //
DELIMITER ;

```
**Penjelasan langkah demi langkah:**
 1. READS SQL DATA — mendeklarasikan function hanya membaca data, tidak memodifikasi.
 2. NOT EXISTS cek keberadaan pendaftaran. Lebih efisien dari handler NOT FOUND karena satu query langsung evaluasi boolean.
 3. SIGNAL SQLSTATE '45000' — pengganti RAISE EXCEPTION PostgreSQL. SQLSTATE 45000 = user-defined error.
 4. Jika pasien BPJS → return 0 (ditanggung BPJS).
 5. **Penyesuaian PDM**: Jika non-BPJS → SUM tarif langsung diambil dari tabel Layanan berdasarkan Pendaftaran_id_daftar tanpa melalui tabel penghubung.
 6. COALESCE menangani kasus belum ada layanan terdaftar.
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
    WHERE  Jadwal_Dokter_id_jadwal = p_id_jadwal
      AND  status_pendaftaran != 'Batal';

    RETURN v_jumlah_aktif < v_limit_antrean;
END //
DELIMITER ;

```
**Penjelasan langkah demi langkah:**
 1. TINYINT(1) — pengganti BOOLEAN PostgreSQL. MySQL menyimpan boolean sebagai 0/1.
 2. Jika SELECT INTO tidak menemukan baris, variabel tetap NULL → dicek dengan IS NULL.
 3. Return 0 jika jadwal tidak ada, Penuh, atau Batal.
 4. Hitung pendaftaran aktif (bukan Batal) pada jadwal melalui kolom kunci asing baru Jadwal_Dokter_id_jadwal.
 5. Return 1 (true) hanya jika jumlah aktif < 30.
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

    SELECT CAST(nomor_antrean AS UNSIGNED), Jadwal_Dokter_id_jadwal
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
 1. Ambil nomor_antrean dan Jadwal_Dokter_id_jadwal dari Pendaftaran.
 2. **Penyesuaian PDM**: Karena nomor_antrean di PDM merupakan VARCHAR(10), digunakan CAST(... AS UNSIGNED) agar dapat dihitung secara matematis.
 3. Ambil jam_mulai dari Jadwal_Dokter.
 4. **Perbaikan efisiensi**: ADDTIME + MAKETIME langsung menghasilkan TIME tanpa string concatenation dan casting.
 5. MAKETIME(0, menit, 0) membuat interval waktu 00:menit:00 secara langsung.
 6. Contoh: antrean ke-5, jam mulai 09:00 → ADDTIME('09:00:00', '00:40:00') = 09:40:00.
### 3.4 fn_Validasi_Metode_Bayar
```sql
DELIMITER //
CREATE FUNCTION fn_Validasi_Metode_Bayar(
    p_id_daftar   INT,
    p_metode_bayar VARCHAR(30)
)
RETURNS TINYINT(1)
READS SQL DATA
BEGIN
    DECLARE v_kategori VARCHAR(30);

    IF NOT EXISTS (SELECT 1 FROM Pendaftaran WHERE id_daftar = p_id_daftar) THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'fn_Validasi_Metode_Bayar: Pendaftaran tidak ditemukan.';
    END IF;

    SELECT p.kategori_pasien INTO v_kategori
    FROM   Pendaftaran dft
    JOIN   Pasien p ON dft.Pasien_id_pasen = p.id_pasen
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
 1. Cek keberadaan pendaftaran dengan NOT EXISTS.
 2. Ambil kategori pasien via JOIN menggunakan nama kolom relasi PDM (Pasien_id_pasen).
 3. Rule 1: Pasien BPJS wajib bayar via BPJS → return 0 jika tidak.
 4. Rule 2: Pasien non-BPJS tidak boleh klaim BPJS → return 0 jika mencoba.
 5. Return 1 jika lolos kedua rule.
## 4. Stored Procedures
### 4.1 sp_Daftar_Pasien_Baru_Kompleks
```sql
DELIMITER //
CREATE PROCEDURE sp_Daftar_Pasien_Baru_Kompleks(
    IN  p_nim_nik          VARCHAR(20),
    IN  p_nama             VARCHAR(30),
    IN  p_asal_faskes      VARCHAR(30),
    IN  p_no_tlp           VARCHAR(15),
    IN  p_kategori_pasien  VARCHAR(30),
    IN  p_id_jadwal        INT,
    IN  p_id_admin         INT,
    IN  p_hasil_skrining   VARCHAR(20),
    IN  p_status_kelayakan VARCHAR(20),
    OUT p_id_daftar        INT,
    OUT p_pesan            VARCHAR(500)
)
BEGIN
    DECLARE v_id_pasen    INT;
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

    SELECT id_pasen INTO v_id_pasen
    FROM   Pasien
    WHERE  nim_nik = p_nim_nik;

    IF v_id_pasen IS NULL THEN
        INSERT INTO Pasien (nim_nik, nama, asal_faskes, no_tlp, kategori_pasien)
        VALUES (p_nim_nik, p_nama, p_asal_faskes, p_no_tlp, p_kategori_pasien);
        SET v_id_pasen = LAST_INSERT_ID();
    ELSE
        UPDATE Pasien
        SET    no_tlp      = COALESCE(NULLIF(p_no_tlp, ''), no_tlp),
               asal_faskes = COALESCE(NULLIF(p_asal_faskes, ''), asal_faskes)
        WHERE  id_pasen    = v_id_pasen;
    END IF;

    INSERT INTO Skrining (Pasien_id_pasen, hasil_skrining, status_kelayakan, tanggal_skrining)
    VALUES (v_id_pasen, p_hasil_skrining, p_status_kelayakan, CURDATE());
    SET v_id_skrining = LAST_INSERT_ID();

    INSERT INTO Pendaftaran (Pasien_id_pasen, Jadwal_Dokter_id_jadwal, Petugas_Admin_id_admin, tanggal_pendaftaran)
    VALUES (v_id_pasen, p_id_jadwal, p_id_admin, CURDATE());
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
 1. **EXIT HANDLER + ROLLBACK** — Jika terjadi error di mana pun dalam prosedur, semua operasi di-rollback otomatis lalu error diteruskan ke pemanggil via RESIGNAL. Ini menjamin atomisitas.
 2. **START TRANSACTION** — Memulai transaksi eksplisit. Semua INSERT/UPDATE di dalam blok ini bersifat atomik.
 3. **Step 1 — Validasi jadwal**: Panggil fn_Cek_Ketersediaan_Jadwal. Return 0 → SIGNAL error.
 4. **Step 2 — Cek/buat pasien**: SELECT INTO lalu cek NULL pada kolom id_pasen. Pasien baru → INSERT + LAST_INSERT_ID(). Pasien lama → UPDATE kontak terkini.
 5. **Step 3 — INSERT Skrining**: Menggunakan struktur FK Pasien_id_pasen dan fungsi CURDATE() untuk tipe data DATE.
 6. **Step 4 — INSERT Pendaftaran**: Memasukkan relasi kunci asing yang tepat sesuai gambar diagram PDM.
 7. **CONCAT** — Pengganti FORMAT() PostgreSQL untuk membangun pesan output.
 8. **COMMIT** — Jika semua langkah sukses, transaksi di-commit.
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
    WHERE  Jadwal_Dokter_id_jadwal = p_id_jadwal
      AND  status_pendaftaran NOT IN ('Selesai', 'Batal');

    SET p_jumlah_terpengaruh = ROW_COUNT();

    COMMIT;
END //
DELIMITER ;

```
**Penjelasan langkah demi langkah:**
 1. **Perbaikan efisiensi utama**: Versi PostgreSQL menggunakan FOR ... LOOP yang melakukan UPDATE satu per satu. Versi MySQL ini menggunakan **satu batch UPDATE** yang mengubah semua baris sekaligus dalam satu operasi berdasarkan Jadwal_Dokter_id_jadwal.
 2. **ROW_COUNT()** — Menggantikan counter manual. Mengembalikan jumlah baris yang terpengaruh oleh UPDATE terakhir.
 3. Trigger trg_buka_kembali_jadwal tetap aktif per baris karena MySQL trigger FOR EACH ROW tetap fire untuk setiap baris dalam batch UPDATE.
 4. Notifikasi tidak ada di MySQL, dipisahkan di level backend.
 5. Performa: O(1) query vs O(N) query. Untuk jadwal dengan 30 pasien, ini 30x lebih cepat.
### 4.3 sp_Proses_Pembayaran
```sql
DELIMITER //
CREATE PROCEDURE sp_Proses_Pembayaran(
    IN  p_id_daftar     INT,
    IN  p_metode_bayar  VARCHAR(30),
    OUT p_id_pembayaran INT,
    OUT p_total_tagihan DECIMAL(12, 2),
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
        SELECT 1 FROM Pembayaran WHERE Pendaftaran_id_dafta = p_id_daftar AND status_bayar = 'Lunas'
    ) THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'Pembayaran sudah Lunas sebelumnya.';
    END IF;

    IF fn_Validasi_Metode_Bayar(p_id_daftar, p_metode_bayar) = 0 THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'Metode bayar tidak sesuai dengan kategori pasien.';
    END IF;

    SET p_total_tagihan = fn_Hitung_Total_Tagihan(p_id_daftar);

    IF EXISTS (SELECT 1 FROM Pembayaran WHERE Pendaftaran_id_dafta = p_id_daftar) THEN
        UPDATE Pembayaran
        SET    metode_bayar = p_metode_bayar,
               status_bayar = 'Lunas'
        WHERE  Pendaftaran_id_dafta = p_id_daftar;

        SELECT id_pembayaran INTO p_id_pembayaran
        FROM   Pembayaran
        WHERE  Pendaftaran_id_dafta = p_id_daftar;
    ELSE
        INSERT INTO Pembayaran (Pendaftaran_id_dafta, metode_bayar, status_bayar)
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
 1. **Step 1 — Validasi pendaftaran**: SELECT INTO + cek NULL (not found). Tolak jika 'Batal' atau sudah 'Lunas' pada kolom penunjuk Pendaftaran_id_dafta.
 2. **Step 2 — Validasi metode bayar**: Panggil fn_Validasi_Metode_Bayar (guard BPJS).
 3. **Step 3 — Kalkulasi tagihan**: Panggil fn_Hitung_Total_Tagihan. BPJS = Rp 0.
 4. **Step 4 — Simpan pembayaran**: Menggunakan kolom penunjuk relasi Pendaftaran_id_dafta sesuai PDM. Pada INSERT, pakai LAST_INSERT_ID().
 5. Trigger validasi, sinkronisasi, dan audit log akan terpicu otomatis.
 6. CONCAT menggantikan FORMAT PostgreSQL.
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

    SELECT Jadwal_Dokter_id_jadwal, status_pendaftaran
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

    IF fn_CET_Ketersediaan_Jadwal(p_id_jadwal_baru) = 0 THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'Jadwal baru tidak tersedia atau sudah penuh.';
    END IF;

    SELECT COALESCE(MAX(CAST(nomor_antrean AS UNSIGNED)), 0) + 1 INTO v_antrean_baru
    FROM   Pendaftaran
    WHERE  Jadwal_Dokter_id_jadwal = p_id_jadwal_baru
      AND  id_daftar != p_id_daftar;

    UPDATE Pendaftaran
    SET    Jadwal_Dokter_id_jadwal = p_id_jadwal_baru,
           status_pendaftaran      = 'Menunggu',
           tanggal_pendaftaran     = CURDATE(),
           nomor_antrean           = CAST(v_antrean_baru AS CHAR)
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
 1. **Step 1 — Ambil data saat ini**: Ambil jadwal lama (Jadwal_Dokter_id_jadwal) dan status. Cek NULL untuk not-found.
 2. **Validasi status**: Tolak jika 'Selesai' atau 'Diperiksa'.
 3. **Step 2 — Validasi jadwal baru**: Panggil fn_Cek_Ketersediaan_Jadwal.
 4. **Perbaikan efisiensi + Konversi Tipe Data**: Nomor antrean bertipe VARCHAR diubah ke angka lewat CAST(... AS UNSIGNED) untuk mencari nilai maksimal, lalu dikembalikan menjadi karakter lewat CAST(... AS CHAR) sebelum disimpan.
 5. **Step 3 — Pindahkan jadwal**: UPDATE pendaftaran ke jadwal baru, reset status dan tanggal menggunakan CURDATE().
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
   * Menunggu → Diperiksa atau Batal
   * Diperiksa → Selesai atau Batal
   * Selesai dan Batal = terminal state
 3. TINYINT(1) menggantikan BOOLEAN untuk flag validasi.
 4. ELSEIF — MySQL syntax (PostgreSQL = ELSIF).
 5. Tidak perlu transaksi eksplisit karena hanya satu UPDATE statement.
### 4.6 sp_Get_Statistik_Harian
```sql
DELIMITER //
CREATE PROCEDURE sp_Get_Statistik_Harian(
    IN  p_tanggal          DATE,
    OUT p_total_pendaftaran INT,
    OUT p_total_selesai     INT,
    OUT p_total_batal       INT,
    OUT p_total_menunggu    INT,
    OUT p_total_pendapatan  DECIMAL(12, 2),
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
    JOIN   Jadwal_Dokter jd ON p.Jadwal_Dokter_id_jadwal = jd.id_jadwal
    WHERE  jd.hari_tanggal = p_tanggal;

    SELECT COALESCE(SUM(l.tarif_total_layanan), 0) INTO p_total_pendapatan
    FROM   Pendaftaran p
    JOIN   Jadwal_Dokter jd ON p.Jadwal_Dokter_id_jadwal = jd.id_jadwal
    JOIN   Pasien pas       ON p.Pasien_id_pasen         = pas.id_pasen
    JOIN   Pembayaran py    ON p.id_daftar               = py.Pendaftaran_id_dafta
    JOIN   Layanan l        ON p.id_daftar               = l.Pendaftaran_id_daftar
    WHERE  jd.hari_tanggal     = p_tanggal
      AND  py.status_bayar     = 'Lunas'
      AND  pas.kategori_pasien != 'BPJS';

    SELECT COUNT(DISTINCT r.id_rujukan) INTO p_total_rujukan
    FROM   Rujukan r
    JOIN   Skrining sk ON r.Skrining_id_skrining = sk.id_skrining
    WHERE  sk.tanggal_skrining = p_tanggal;
END //
DELIMITER ;

```
**Penjelasan langkah demi langkah:**
 1. **Dikonversi dari function ke procedure** — MySQL function tidak bisa return TABLE. Procedure dengan OUT parameter sebagai gantinya.
 2. **Perbaikan efisiensi utama + Struktur PDM**: 3 query terpisah yang masing-masing hanya JOIN tabel yang dibutuhkan. Pada query kedua, karena tabel perantara telah tiada, tabel Layanan langsung digabungkan ke tabel Pendaftaran via Pendaftaran_id_daftar.
 3. SUM(condition) — MySQL shorthand untuk akumulasi kondisi boolean secara efisien.
 4. INNER JOIN digunakan karena hanya mencari tagihan yang berstatus Lunas dan valid.
 5. Menghitung rujukan harian dicocokkan berdasarkan kaitan FK Skrining_id_skrining.
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
            WHERE  id_daftar = NEW.Pendaftaran_id_dafta;
        END IF;

        IF NEW.status_bayar = 'Lunas' AND OLD.status_bayar = 'Pending' THEN
            UPDATE Pendaftaran
            SET    status_pendaftaran = 'Menunggu'
            WHERE  id_daftar = NEW.Pendaftaran_id_dafta;
        END IF;
    END IF;
END //
DELIMITER ;

```
**Penjelasan langkah demi langkah:**
 1. AFTER UPDATE — Trigger berjalan pasca-perubahan data tabel Pembayaran.
 2. NOT (OLD.status_bayar <=> NEW.status_bayar) — Menggunakan operator <=> (NULL-safe equality) untuk menjamin trigger hanya memproses ketika nilai benar-benar bergeser.
 3. Pembayaran 'Gagal' → pendaftaran otomatis 'Batal' pada rekaman Pendaftaran_id_dafta yang bersangkutan.
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
    WHERE  id_jadwal = NEW.Jadwal_Dokter_id_jadwal;

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
 1. BEFORE INSERT pada Pendaftaran — validasi sebelum data masuk.
 2. Mencari status keandalan dari Jadwal_Dokter_id_jadwal.
 3. Mengembalikan pesan penolakan terstruktur bila kondisi tidak dipenuhi.
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
    WHERE  id_jadwal = NEW.Jadwal_Dokter_id_jadwal
    FOR UPDATE;

    SELECT COALESCE(MAX(CAST(nomor_antrean AS UNSIGNED)), 0) + 1 INTO v_next_antrean
    FROM   Pendaftaran
    WHERE  Jadwal_Dokter_id_jadwal = NEW.Jadwal_Dokter_id_jadwal;

    SET NEW.nomor_antrean = CAST(v_next_antrean AS CHAR);
END //
DELIMITER ;

```
**Penjelasan langkah demi langkah:**
 1. FOLLOWS — Memastikan aturan validasi status berjalan mendahului pembuatan nomor antrean.
 2. FOR UPDATE — Mengunci baris jadwal dokter agar terhindar dari benturan nomor antrean sejenis (*race condition*).
 3. **Penyesuaian PDM**: Menggunakan fungsi CAST untuk memanipulasi tipe data string numerik pada kolom nomor_antrean.
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
    WHERE  Jadwal_Dokter_id_jadwal = NEW.Jadwal_Dokter_id_jadwal
      AND  status_pendaftaran != 'Batal';

    IF v_jumlah_pasien >= 30 THEN
        UPDATE Jadwal_Dokter
        SET    status_jadwal = 'Penuh'
        WHERE  id_jadwal     = NEW.Jadwal_Dokter_id_jadwal
          AND  status_jadwal = 'Tersedia';
    END IF;
END //
DELIMITER ;

```
**Penjelasan langkah demi langkah:**
 1. Mengecek kuota berjalan pasca data pendaftaran terekam.
 2. Memperbarui status jadwal menjadi 'Penuh' apabila angka pendaftaran aktif menyentuh batas kuota (30).
### 5.5 Trigger 5a — Rujukan Otomatis dari Skrining (INSERT)
```sql
DELIMITER //
CREATE TRIGGER trg_buat_draft_rujukan_insert
AFTER INSERT ON Skrining
FOR EACH ROW
BEGIN
    IF NEW.status_kelayakan = 'Perlu Rujukan' THEN
        IF NOT EXISTS (SELECT 1 FROM Rujukan WHERE Skrining_id_skrining = NEW.id_skrining) THEN
            INSERT INTO Rujukan (Skrining_id_skrining, alasan_rujukan, asal_rujukan, tujuan_rujukan, tanggal_rujukan)
            VALUES (
                NEW.id_skrining,
                'Draft Otomatis',
                'Klinik Utama',
                'Belum Ditentukan',
                CURDATE()
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
        IF NOT EXISTS (SELECT 1 FROM Rujukan WHERE Skrining_id_skrining = NEW.id_skrining) THEN
            INSERT INTO Rujukan (Skrining_id_skrining, alasan_rujukan, asal_rujukan, tujuan_rujukan, tanggal_rujukan)
            VALUES (
                NEW.id_skrining,
                'Draft Otomatis',
                'Klinik Utama',
                'Belum Ditentukan',
                CURDATE()
            );
        END IF;
    END IF;
END //
DELIMITER ;

```
**Penjelasan langkah demi langkah (5a & 5b):**
 1. Dipecah menjadi 2 berkas trigger terpisah dikarenakan batas fungsionalitas MySQL.
 2. Mengamankan pembuatan draft rujukan otomatis menggunakan FK Skrining_id_skrining dan tipe data DATE.
### 5.7 Trigger 6a — Validasi Metode Bayar (INSERT)
```sql
DELIMITER //
CREATE TRIGGER trg_validasi_metode_bayar_insert
BEFORE INSERT ON Pembayaran
FOR EACH ROW
BEGIN
    DECLARE v_kategori VARCHAR(30);

    SELECT p.kategori_pasien INTO v_kategori
    FROM   Pendaftaran dft
    JOIN   Pasien p ON dft.Pasien_id_pasen = p.id_pasen
    WHERE  dft.id_daftar = NEW.Pendaftaran_id_dafta;

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
    DECLARE v_kategori VARCHAR(30);

    IF NOT (OLD.metode_bayar <=> NEW.metode_bayar) THEN
        SELECT p.kategori_pasien INTO v_kategori
        FROM   Pendaftaran dft
        JOIN   Pasien p ON dft.Pasien_id_pasen = p.id_pasen
        WHERE  dft.id_daftar = NEW.Pendaftaran_id_dafta;

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
 1. Melakukan pencocokan silang antara metode pembayaran dan kualifikasi data master pasien (Pasien_id_pasen).
 2. Menghalangi penyimpanan data yang tidak sesuai aturan penjaminan faskes melalui respon SIGNAL.
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
        WHERE  id_jadwal = NEW.Jadwal_Dokter_id_jadwal;

        IF v_status_jadwal = 'Penuh' THEN
            SELECT COUNT(*) INTO v_jumlah_aktif
            FROM   Pendaftaran
            WHERE  Jadwal_Dokter_id_jadwal = NEW.Jadwal_Dokter_id_jadwal
              AND  status_pendaftaran != 'Batal';

            IF v_jumlah_aktif < 30 THEN
                UPDATE Jadwal_Dokter
                SET    status_jadwal = 'Tersedia'
                WHERE  id_jadwal     = NEW.Jadwal_Dokter_id_jadwal;
            END IF;
        END IF;
    END IF;
END //
DELIMITER ;

```
**Penjelasan langkah demi langkah:**
 1. Terbakar saat pasien membatalkan janji medis (status_pendaftaran = 'Batal').
 2. Mengecek status keterisian kuota pada tabel induk melalui Jadwal_Dokter_id_jadwal. Jika antrean turun di bawah 30, slot otomatis dibuka kembali menjadi 'Tersedia'.
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
            Pendaftaran_id_dafta,
            status_lama,
            status_baru,
            metode_lama,
            metode_baru,
            keterangan
        )
        VALUES (
            NEW.id_pembayaran,
            NEW.Pendaftaran_id_dafta,
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
 1. Menjamin urutan logging berjalan tertib memanfaatkan perintah eksplisit FOLLOWS.
 2. Menyimpan riwayat perubahan status keuangan secara rinci ke dalam tabel Log_Pembayaran.
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
    ps.no_tlp        AS no_telp,
    p.status_pendaftaran,
    fn_Hitung_Estimasi_Tunggu(p.id_daftar) AS estimasi_dipanggil
FROM   Pendaftaran p
JOIN   Jadwal_Dokter jd ON p.Jadwal_Dokter_id_jadwal  = jd.id_jadwal
JOIN   Dokter d         ON jd.Dokter_id_dokter        = d.id_dokter
JOIN   Poli pl          ON d.Poli_id_poli             = pl.id_poli
JOIN   Pasien ps        ON p.Pasien_id_pasen          = ps.id_pasen
WHERE  p.status_pendaftaran IN ('Menunggu', 'Diperiksa')
ORDER  BY jd.id_jadwal, CAST(p.nomor_antrean AS UNSIGNED);

```
**Penjelasan langkah demi langkah:**
 1. Menampilkan antrean pasien yang aktif berjalan (Menunggu + Diperiksa).
 2. **Penyesuaian PDM**: Penggabungan (JOIN) tabel disesuaikan dengan pola relasi dan kunci asing baru (Jadwal_Dokter_id_jadwal, Dokter_id_dokter, Poli_id_poli, Pasien_id_pasen).
 3. Pengurutan nomor antrean string dipaksa menjadi numerik menggunakan CAST(... AS UNSIGNED) agar urutannya presisi (antrean ke-10 tidak muncul sebelum antrean ke-2).
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
JOIN   Dokter d    ON jd.Dokter_id_dokter = d.id_dokter
JOIN   Poli pl     ON d.Poli_id_poli      = pl.id_poli
LEFT JOIN Pendaftaran p ON p.Jadwal_Dokter_id_jadwal = jd.id_jadwal
GROUP  BY jd.id_jadwal, jd.hari_tanggal, jd.jam_mulai, jd.jam_selesai,
          jd.status_jadwal, d.nama_dokter, pl.nama_poli
ORDER  BY jd.hari_tanggal DESC, jd.jam_mulai;

```
**Penjelasan langkah demi langkah:**
 1. Menyajikan ringkasan kinerja harian klinik per jadwal dokter.
 2. Penggabungan memanfaatkan LEFT JOIN Pendaftaran melalui kolom relasi PDM agar jadwal yang kosong tetap terhitung dalam visualisasi laporan.
