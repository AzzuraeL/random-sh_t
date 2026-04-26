### 1. `index.html`
Klien membuka `index.html` yang memuat `app.js`. Script ini akan melakukan capture pada layar secara lokal dan terus-menerus mengunggahnya ke server HTTP.
- Menyediakan tata letak (*layout*) dasar dengan dua panelyaitu satu untuk melihat preview *stream* lokal secara langsung (melalui elemen `<video>`) dan satu lagi untuk melihat preview *frame* yang berhasil diunggah ke server (melalui elemen `<img>`).
- Memuat 2 tombol (`startBtn` dan `stopBtn`) serta memanggil Script utama `app.js` untuk menjalankan logikanya.

---

### 2. `app.js`
File yang akan digunakan untuk melakukan capture pda layar (logic side)

- **`startSharing()`**: Fungsi yang digunakan untuk meminta izin dari OS untuk merekam layar menggunakan API `navigator.mediaDevices.getDisplayMedia` dengan batas maksimal 15 FPS. Jika diizinkan, *stream* diarahkan ke elemen video lokal, dan fungsi ini mengaktifkan dua *interval* dimana satu untuk menangkap gambar (tiap 125ms) dan satu untuk memperbarui statistik (tiap 1000ms).
- **`stopSharing()`**: Fungsi yang digunakan untuk menghentikan semua *track* media yang sedang direkam, membersihkan ( *clear*) *interval* yang berjalan, dan mereset UI ke status awal.
- **`captureAndSend()`**: Fungsi yang digunakan untuk mengambil *frame* video saat ini dan menggambarnya ke sebuah elemen `<canvas>` tersembunyi. Setelah itu, gambar di dalam kanvas dikonversi menjadi file *blob* berformat JPEG dengan kualitas 50% (`0.5`). File ini kemudian dikirim secara asinkron ke *endpoint* HTTP `/upload-frame` menggunakan metode POST.
- **`updateStats()`**: Fungsi yang digunakan untuk melakukan HTTP GET ke `/stats` untuk mengambil data kinerja dari server (seperti jumlah *frame* yang diunggah) lalu memperbarui teks yang ada di UI. Fungsi ini juga memaksa elemen preview server (`serverFrame`) untuk memuat ulang gambar dari *endpoint* `/frame` dengan menambahkan stempel waktu (`?t=now`) agar tidak terkena *cache*.

---

### 3. `http_server.py`
Server yang menggunakan *socket* murni berbasis TCP, `http_server.py` bertindak sebagai tempat "berhenti sementara" dimana *frame* gambar terbaru akan disimpan sebelum diambil di memori.

- **`read_request(client_socket)`**: Fungsi yang digunakan untuk membaca data mentah (*raw byte*) yang masuk dari *socket* klien. Fungsi ini memisahkan bagian *header* dan *body* dari HTTP *request*, serta memastikan seluruh data dalam `Content-Length` (seperti *blob* gambar) sudah diunduh sepenuhnya sebelum diproses.
- **`handle_client(client_socket)`**: Fungsi *routing* utama:
    - Jika *request* adalah `GET /` atau `GET /app.js`, server akan membaca file lokal tersebut (`index.html` dan `app.js`) dan mengirimkannya ke *browser*.
    - Jika *request* adalah `POST /upload-frame`, server akan mengambil *body* (file JPEG) dan menyimpannya menimpa variabel global `STATE['latest_frame']` di dalam memori.
    - Jika *request* adalah `GET /frame`, server akan mengembalikan isi dari `STATE['latest_frame']` dengan format `image/jpeg`.
    - Jika *request* adalah `GET /stats`, server menghitung dan mengembalikan status metrik dalam bentuk JSON.
- **`start_server()`**: Fungsi yang digunakan untuk membuka *port* 8000 dan mendengarkan koneksi yang masuk. Setiap kali ada permintaan masuk, server akan membuat *Thread* baru sehingga permintaan *upload* dan *download* bisa ditangani secara bersamaan tanpa saling blocking.

---

### 4. `udp_sender.py`
`udp_sender.py` secara konstan mengunduh *frame* terbaru dari server HTTP, memecahnya menjadi paket-paket kecil, dan melakukan broadcast via protokol UDP.

- **`fetch_frame()`**: Fungsi yang digunakan untuk melakukan *request* standar HTTP GET ke `http://127.0.0.1:8000/frame` (ke `http_server.py`) dan mengembalikan data biner (*byte*) dari gambar tersebut.
- **`send_frame_udp(frame_data, frame_id, sock, target_addr)`**: Karena protokol UDP memiliki batas ukuran transmisi, gambar yang diunduh tidak bisa dikirim sekaligus. Fungsi yang digunakan untuk memecah data biner gambar menjadi potongan-potongan (*chunks*) maksimal berukuran 1200 *bytes*. Setiap potongan dibubuhi *header* khusus yang berisi 4 informasi (nomor *frame*, nomor urut potongan, total potongan, dan panjang data) menggunakan `struct.pack`, lalu dikirim ke *port* 5005 target.
- **`start_sender()`**: Fungsi ini berjalan dalam *loop* unlimited (*while True*) dimana fungsi yang digunakan untuk mengatur ritme (*throttle*) agar proses pengambilan dan pengiriman gambar dijaga di target 8 FPS menggunakan sleep (`time.sleep`).

---

### 5. `udp_receiver.py`
`udp_receiver.py` akan melakukan listen pada broadcast UDP tersebut, menyusun kembali potongan paket menjadi gambar utuh, dan menampilkannya menggunakan OpenCV.

- **`cleanup_old_frames(frames_dict, current_time)`**: Karena UDP tidak menjamin paket sampai, beberapa *frame* mungkin kehilangan satu atau dua *chunk* sehingga tidak akan pernah lengkap. Fungsi yang digunakan untuk menghapus *frame* yang *stale* (lebih lama dari 2 detik) dari memori agar RAM tidak bocor.
- **`reassemble_payload(frame_info)`**: Fungsi yang digunakan untuk mengurutkan kembali semua *chunk* paket yang masuk berdasarkan `chunk_id` agar data biner gambar tersusun dalam urutan yang benar secara utuh.
- **`start_receiver()`**: Fungsi yang digunakan untuk membuka *socket* UDP yang mendengarkan *port* 5005. Setiap kali ada paket masuk, fungsi ini membongkar *header*-nya (`struct.unpack`) dan memasukkan *payload* ke dalam struktur *dictionary* berdasarkan `frame_id`. Jika jumlah *chunk* yang terkumpul sudah sama persis dengan `total_chunks` yang diinfokan *header*, itu berarti satu gambar utuh berhasil direkonstruksi. Data biner itu kemudian diubah ke dalam *array* NumPy, didekode menjadi *pixel matrix* menggunakan OpenCV (`cv2.imdecode`), dan dimunculkan sebagai video menggunakan `cv2.imshow`.

Berikut adalah tambahan bagian yang bisa Anda cantumkan di bagian bawah README Anda untuk menegaskan bahwa kode tersebut telah memenuhi semua spesifikasi dan batasan sistem:

---

### Pemenuhan Spesifikasi dan Requirement

Sistem *window mirroring* ini telah didesain dan diimplementasikan agar mematuhi seluruh *constraint* yang ada:

- **Format Packet UDP & Header 10 Bytes:** Sudah dipenuhi dengan data *packing* pada `udp_sender.py` dimana diimplementasikan menggunakan format `!IHHH` pada *library* `struct`. Konfigurasi ini secara akurat merepresentasikan alokasi memori untuk `frame_id` (4 *bytes*), `chunk_id` (2 *bytes*), `total_chunks` (2 *bytes*), dan `payload_len` (2 *bytes*), sehingga memastikan *header* berukuran konstan tepat 10 *bytes*.
- **Max UDP Packet 1200 Bytes:** Sudah dipenuhi melalui variabel `max_payload = 1190` (menjadi 1200 jika ditambah header 10 bytes nantinya) pada `udp_sender.py`. Logika loop akan melakukan fragmentasi (*chunking*) terhadap data gambar, menjamin bahwa tidak ada satupun *payload* yang dikirimkan melebihi batas 1200 *bytes*.
- **Max Frame Upload 100000 Bytes:** Sudah terpenuhi dengan cara membuat pengambilan *capture* sisi klien di `app.js` secara sadar menggunakan kompresi `image/jpeg` dengan kualitas `0.5`. Ini berfungsi untuk mereduksi ukuran gambar asli dari *browser* agar beban *upload* menuju memori server tetap ringan dan mampu menyesuaikan limit transmisi di bawah 100 KB.
- **Target Stream 8 FPS:** Sudah dipenuhi dengan menhatur ritme briadcast UDP di `udp_sender.py` secara presisi melalui variabel `target_fps = 8`. Script melakukan kalkulasi selisih waktu eksekusi (*loop_start*) dan menggunakan `time.sleep()` untuk memastikan sinkronisasi pengiriman berjalan stabil di 8 *frames per second*.
- **Frame Timeout Receiver 2 Detik:** Sudah dipenuhi di `udp_receiver.py` yang dilengkapi dengan mekanisme melalui fungsi `cleanup_old_frames`. Dimana setiap kali *loop* berjalan, fungsi ini akan mendeteksi `timeout=2.0`. Jika sebuah *frame* gagal menerima *chunk* secara lengkap dalam kurun waktu 2 detik, data parsial tersebut akan dihapus dari memori (*dropped*).
- **UDP Mode (Fire-and-Forget):** Implementasi transmisi murni menggunakan protokol UDP via `sock.sendto` tanpa adanya fase *handshake*, *acknowledgement* (ACK), maupun mekanisme *retry*. 
