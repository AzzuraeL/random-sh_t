## 2. Tabel Audit Log
```sql
CREATE TABLE Log_Pembayaran (
    id_log          INT AUTO_INCREMENT PRIMARY KEY,
    id_pembayaran   INT         NOT NULL,
    id_daftar       INT         NOT NULL,
    status_lama     VARCHAR(20),
    status_baru     VARCHAR(20),
    metode_lama     VARCHAR(30),
    metode_baru     VARCHAR(30),
    waktu_perubahan TIMESTAMP   DEFAULT CURRENT_TIMESTAMP,
    keterangan      TEXT
) ENGINE=InnoDB;

```
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

    -- Query langsung diarahkan ke tabel Layanan sesuai PDM baru
    SELECT COALESCE(SUM(tarif_total_layanan), 0) INTO v_total
    FROM   Layanan
    WHERE  Pendaftaran_id_daftar = p_id_daftar;

    RETURN v_total;
END //
DELIMITER ;

```
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

    IF v_status_jadwal IS NULL OR v_status_jadwal IN ('Penuh', 'Batal') THEN
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
### 3.3 fn_Hitung_Estimasi_Tunggu
```sql
DELIMITER //
CREATE FUNCTION fn_Hitung_Estimasi_Tunggu(p_id_daftar INT)
RETURNS TIME
READS SQL DATA
BEGIN
    DECLARE v_nomor_antrean_str VARCHAR(10);
    DECLARE v_nomor_antrean_int INT;
    DECLARE v_id_jadwal         INT;
    DECLARE v_jam_mulai         TIME;
    DECLARE v_menit_per_pasien  INT DEFAULT 10;

    SELECT nomor_antrean, Jadwal_Dokter_id_jadwal
    INTO   v_nomor_antrean_str, v_id_jadwal
    FROM   Pendaftaran
    WHERE  id_daftar = p_id_daftar;

    IF v_nomor_antrean_str IS NULL THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'fn_Hitung_Estimasi_Tunggu: Pendaftaran tidak ditemukan/antrean kosong.';
    END IF;

    -- Konversi nomor antrean dari VARCHAR ke integer untuk perhitungan matematika
    SET v_nomor_antrean_int = CAST(v_nomor_antrean_str AS UNSIGNED);

    SELECT jam_mulai INTO v_jam_mulai
    FROM   Jadwal_Dokter
    WHERE  id_jadwal = v_id_jadwal;

    RETURN ADDTIME(v_jam_mulai,
                   MAKETIME(0, (v_nomor_antrean_int - 1) * v_menit_per_pasien, 0));
END //
DELIMITER ;

```
### 3.4 fn_Validasi_Metode_Bayar
```sql
DELIMITER //
CREATE FUNCTION fn_Validasi_Metode_Bayar(
    p_id_daftar    INT,
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
    DECLARE v_id_pasen          INT;
    DECLARE v_id_skrining       INT;
    DECLARE v_next_antrean_int  INT DEFAULT 1;
    DECLARE v_next_antrean_str  VARCHAR(10);

    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        RESIGNAL;
    END;

    START TRANSACTION;

    -- 1. Validasi Sisa Slot Jadwal Dokter
    IF fn_Cek_Ketersediaan_Jadwal(p_id_jadwal) = 0 THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'JADWAL_TIDAK_TERSEDIA: Jadwal penuh atau sudah dibatalkan.';
    END IF;

    -- 2. Cek Keberadaan Pasien Berdasarkan NIK/NIM
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

    -- 3. Input Hasil Skrining
    INSERT INTO Skrining (Pasien_id_pasen, tanggal_skrining, hasil_skrining, status_kelayakan)
    VALUES (v_id_pasen, CURRENT_DATE(), p_hasil_skrining, p_status_kelayakan);
    SET v_id_skrining = LAST_INSERT_ID();

    -- 4. Hitung Nomor Antrean Berikutnya secara Sekuensial
    SELECT COALESCE(MAX(CAST(nomor_antrean AS UNSIGNED)), 0) + 1 INTO v_next_antrean_int
    FROM   Pendaftaran
    WHERE  Jadwal_Dokter_id_jadwal = p_id_jadwal;
    
    SET v_next_antrean_str = LPAD(v_next_antrean_int, 3, '0');

    -- 5. Input Transaksi Pendaftaran Pasien
    INSERT INTO Pendaftaran (Pasien_id_pasen, Jadwal_Dokter_id_jadwal, Petugas_Admin_id_admin, tanggal_pendaftaran, nomor_antrean, status_pendaftaran)
    VALUES (v_id_pasen, p_id_jadwal, p_id_admin, CURRENT_DATE(), v_next_antrean_str, 'Menunggu');
    SET p_id_daftar = LAST_INSERT_ID();

    SET p_pesan = CONCAT(
        'SUKSES: Pasien "', p_nama, '" berhasil didaftarkan. ID Daftar: ', p_id_daftar,
        ' | No Antrean: ', v_next_antrean_str, ' | ID Skrining: ', v_id_skrining, '.'
    );

    COMMIT;
END //
DELIMITER ;

```
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

    -- Set status jadwal utama menjadi Batal
    UPDATE Jadwal_Dokter
    SET    status_jadwal = 'Batal'
    WHERE  id_jadwal     = p_id_jadwal;

    -- Batalkan seluruh pendaftaran aktif pada jadwal terkait secara massal (Batch Update)
    UPDATE Pendaftaran
    SET    status_pendaftaran = 'Batal'
    WHERE  Jadwal_Dokter_id_jadwal = p_id_jadwal
      AND  status_pendaftaran NOT IN ('Selesai', 'Batal');

    SET p_jumlah_terpengaruh = ROW_COUNT();

    COMMIT;
END //
DELIMITER ;

```
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

    IF EXISTS (
        SELECT 1 FROM Pembayaran WHERE Pendaftaran_id_dafta = p_id_daftar AND status_bayar = 'Lunas'
    ) THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'Pembayaran sudah berstatus Lunas sebelumnya.';
    END IF;

    -- Validasi kesesuaian kategori klaim/metode bayar
    IF fn_Validasi_Metode_Bayar(p_id_daftar, p_metode_bayar) = 0 THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'Metode bayar tidak sesuai dengan kategori pasien.';
    END IF;

    -- Hitung total biaya layanan terdaftar
    SET p_total_tagihan = fn_Hitung_Total_Tagihan(p_id_daftar);

    -- Simpan data transaksi ke tabel Pembayaran sesuai PDM
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

    -- Selesaikan status pendaftaran secara otomatis setelah lunas
    UPDATE Pendaftaran
    SET    status_pendaftaran = 'Selesai'
    WHERE  id_daftar = p_id_daftar;

    SET p_pesan = CONCAT(
        'Pembayaran LUNAS. ID Pembayaran: ', p_id_pembayaran,
        ' | Total: Rp ', p_total_tagihan, ' | Metode: ', p_metode_bayar, '.'
    );

    COMMIT;
END //
DELIMITER ;

```
### 4.4 sp_Reschedule_Pendaftaran
```sql
DELIMITER //
CREATE PROCEDURE sp_Reschedule_Pendaftaran(
    IN  p_id_daftar      INT,
    IN  p_id_jadwal_baru INT,
    OUT p_pesan          VARCHAR(500)
)
BEGIN
    DECLARE v_id_jadwal_lama   INT;
    DECLARE v_status_daftar    VARCHAR(20);
    DECLARE v_next_antrean_int INT;
    DECLARE v_next_antrean_str VARCHAR(10);

    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        RESIGNAL;
    END;

    START TRANSACTION;

    -- 1. Validasi Eksistensi Pendaftaran Pasien
    SELECT Jadwal_Dokter_id_jadwal, status_pendaftaran
    INTO   v_id_jadwal_lama, v_status_daftar
    FROM   Pendaftaran
    WHERE  id_daftar = p_id_daftar;

    IF v_id_jadwal_lama IS NULL THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'sp_Reschedule_Pendaftaran: Pendaftaran tidak ditemukan.';
    END IF;

    -- 2. Cegah Reschedule jika Sesi Pemeriksaan Sudah Berjalan/Batal
    IF v_status_daftar IN ('Diperiksa', 'Selesai', 'Batal') THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'Gagal: Pendaftaran sudah diproses, selesai, atau dibatalkan.';
    END IF;

    -- 3. Validasi Kapasitas Slot Jadwal Baru
    IF fn_Cek_Ketersediaan_Jadwal(p_id_jadwal_baru) = 0 THEN
        SIGNAL SQLSTATE '45000'
            SET MESSAGE_TEXT = 'Gagal: Jadwal tujuan penuh atau sudah dibatalkan.';
    END IF;

    -- 4. Hitung Nomor Antrean Baru pada Jadwal Tujuan
    SELECT COALESCE(MAX(CAST(nomor_antrean AS UNSIGNED)), 0) + 1 INTO v_next_antrean_int
    FROM   Pendaftaran
    WHERE  Jadwal_Dokter_id_jadwal = p_id_jadwal_baru;

    SET v_next_antrean_str = LPAD(v_next_antrean_int, 3, '0');

    -- 5. Perbarui Relasi Jadwal dan Nomor Antrean
    UPDATE Pendaftaran
    SET    Jadwal_Dokter_id_jadwal = p_id_jadwal_baru,
           nomor_antrean           = v_next_antrean_str,
           status_pendaftaran      = 'Menunggu'
    WHERE  id_daftar               = p_id_daftar;

    SET p_pesan = CONCAT(
        'SUKSES: Pendaftaran ID ', p_id_daftar, ' dipindahkan dari Jadwal ID ', 
        v_id_jadwal_lama, ' ke Jadwal ID ', p_id_jadwal_baru, 
        '. Nomor antrean baru: ', v_next_antrean_str, '.'
    );

    COMMIT;
END //
DELIMITER ;

```
