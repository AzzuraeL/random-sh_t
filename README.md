# Socket Server - Multi-Method Implementation

Proyek ini adalah implementasi server TCP yang mendukung fitur group chat dan transfer file (upload/download). Server diimplementasikan dalam empat metode berbeda: sync, thread, select, dan poll. Semua server kompatibel dengan satu file client yang sama.

---

## Daftar File

| File | Deskripsi |
|---|---|
| `client.py` | client untuk semua varian server |
| `server-sync.py` | Server sync, satu client sekaligus |
| `server-thread.py` | Server multi-client berbasis thread |
| `server-select.py` | Server multi-client berbasis `select` |
| `server-poll.py` | Server multi-client berbasis `poll` |

---

## Protokol Komunikasi

Semua pesan JSON dikemas dengan format berikut:

```
[4 bytes big-endian: panjang payload JSON] [N bytes: payload JSON]
```

Header 4 byte dikemas menggunakan `struct.pack('>I', length)` dan dibuka kembali dengan `struct.unpack('>I', header)`. Pendekatan ini diperlukan karena TCP adalah stream protocol, bukan message protocol, sehingga tanpa header panjang tidak ada cara untuk mengetahui batas antar pesan.

Transfer file (upload/download) dilakukan dengan mengirim raw bytes langsung ke socket setelah kedua pihak sepakat melalui pesan JSON. Tidak ada framing tambahan saat transfer file berlangsung.

---

## Struktur Pesan JSON

### Dari client ke server

| type | Field tambahan | Keterangan |
|---|---|---|
| `join` | `username` | Pesan pertama setelah koneksi |
| `chat` | `message` | Pesan teks ke semua client |
| `command` | `command: "list"` | Minta daftar file |
| `command` | `command: "upload"`, `filename`, `filesize` | Mulai upload |
| `command` | `command: "download"`, `filename` | Minta download |

### Dari server ke client

| type | Field tambahan | Keterangan |
|---|---|---|
| `info` | `message` | Notifikasi sistem |
| `error` | `message` | Pesan error |
| `chat` | `sender`, `message` | Pesan dari client lain |
| `list_result` | `files` | Daftar file di server |
| `upload_ready` | `filename` | Server siap terima bytes |
| `upload_done` | `filename`, `message` | Konfirmasi upload selesai |
| `download_ready` | `filename`, `filesize` | Server siap kirim bytes |

---

## client.py

### Konstanta dan Direktori

```python
HOST = '127.0.0.1'
PORT = 5000
CLIENT_FILES_DIR = 'client_files'
BUFFER_SIZE = 4096
```

Semua file yang akan diupload harus berada di folder `client_files/`. File yang didownload juga disimpan ke folder yang sama. Folder dibuat otomatis jika belum ada melalui `ensure_client_dir()`.

### send_json dan recv_json

`send_json` mengonversi dict Python ke JSON string, encode ke UTF-8, lalu mrnyiapkan juga header 4 byte berisi panjang payload. Keduanya digabung dan dikirim sekaligus dengan `sendall`.

`recv_json` membaca tepat 4 byte header terlebih dahulu, mengekstrak panjang payload, lalu masuk ke loop yang membaca chunk dari socket sampai akumulasi byte mencapai panjang yang diharapkan. Loop ini penting karena satu panggilan `recv` tidak menjamin seluruh payload langsung tersedia.

### send_file_bytes dan recv_file_bytes

`send_file_bytes` membuka file dalam mode binary, membaca per chunk sebesar `BUFFER_SIZE` (4096 bytes), dan mengirimkan masing-masing chunk dengan `sendall`. Loop berakhir saat `f.read` mengembalikan bytes kosong.

`recv_file_bytes` menerima bytes dari socket dan menulis ke file. Counter `received` dilacak dan setiap chunk yang diterima dikurangi dari total. Parameter `remaining = filesize - received` digunakan sebagai batas maksimum pada setiap `recv` agar client tidak membaca lebih dari ukuran file yang diharapkan, mencegah terbacanya byte awal dari pesan JSON berikutnya.

### handle_server_messages

Thread ini berjalan sebagai daemon dan terus memanggil `recv_json` dalam loop. Setiap pesan yang diterima diproses berdasarkan field `type`:

- `chat`: Mencetak pesan beserta nama pengirim.
- `info` / `error`: Mencetak info sistem atau error.
- `list_result`: Mencetak daftar file. Jika list kosong, ditampilkan pesan khusus.
- `upload_ready`: Membaca file dari `client_files/` dan mengirim raw bytes-nya ke server menggunakan `send_file_bytes`. Ini dipicu oleh respons server, bukan langsung setelah `/upload` diketik, karena client harus menunggu server siap terlebih dahulu.
- `upload_done`: Mencetak konfirmasi bahwa file berhasil disimpan di server.
- `download_ready`: Memanggil `recv_file_bytes` untuk menerima byte file dan menyimpannya ke `client_files/`. Ukuran file diketahui dari field `filesize` dalam pesan ini.

Jika `recv_json` mengembalikan `None` (koneksi putus), loop berhenti dan thread berakhir.

### input_loop

Membaca input dari terminal dalam loop tak terbatas menggunakan `input()`. Input kosong di-skip. Logika pengecekan perintah:

- `/list`: Mengirim pesan command `list` tanpa argumen tambahan.
- `/upload <filename>`: Memvalidasi bahwa file ada di `client_files/` secara lokal sebelum mengirim apapun ke server. Jika file tidak ada, error ditampilkan dan tidak ada yang dikirim ke server. Jika ada, ukuran file diambil dengan `os.path.getsize` dan dikirim bersama nama file dalam pesan command `upload`.
- `/download <filename>`: Mengirim command `download` dengan nama file. Tidak ada validasi lokal karena file ada di sisi server.
- Input lain: Dikirim sebagai pesan `chat`.

### main

Membuat satu socket TCP, terhubung ke server, lalu mengirim pesan `join` dengan username. Thread daemon `handle_server_messages` distart, kemudian `input_loop` dijalankan di thread utama. Karena thread penerima bersifat daemon, thread ini otomatis berhenti saat thread utama selesai.

---

## server-sync.py

Server paling sederhana. Seluruh operasi blocking, tidak ada konkurensi. Hanya mampu melayani satu client dalam satu waktu karena `listen(1)` membatasi antrian koneksi dan loop utama tidak kembali ke `accept()` sebelum client sebelumnya selesai.

### send_json dan recv_json

`send_json` membungkus `sendall` dalam try/except tanpa aksi pada exception. `recv_json` membaca header 4 byte lalu payload dalam loop, mengembalikan `None` jika terjadi error atau koneksi terputus.

### send_file_bytes dan recv_file_bytes

`send_file_bytes` membaca dan mengirim file per chunk. `recv_file_bytes` menerima byte dari socket sampai total `filesize` byte terpenuhi, menulis ke file secara bertahap, dan mengembalikan `True` atau `False` tergantung keberhasilan.

### handle_command

Dipanggil setiap kali pesan bertipe `command` diterima. Memvalidasi socket ada di `clients` sebelum melanjutkan.

- `list`: Membaca isi direktori `server_files/` dengan `os.listdir`, mengurutkannya, dan mengirim hasilnya.
- `upload`: Mengambil nama file dengan `os.path.basename` untuk mencegah path traversal/nama yang tidak aman (misal: `../../etc/passwd` jadi `passwd`). Mengirim `upload_ready`, lalu langsung memanggil `recv_file_bytes` secara blocking. Jika gagal, file parsial dihapus.
- `download`: Mengecek keberadaan file dengan `os.path.exists`. Jika ada, mengirim `download_ready` beserta ukuran file, lalu memanggil `send_file_bytes`. Semua berlangsung secara blocking sebelum fungsi kembali.

### start_server

Membuat socket dengan `SO_REUSEADDR` agar port bisa langsung dipakai ulang setelah server restart. Loop luar memanggil `accept()` yang memblokir hingga client terhubung. Setelah terhubung, client didaftarkan ke dict `clients` dengan `username: None`.

Loop dalam membaca pesan JSON satu per satu. Pesan pertama selalu diperlakukan sebagai registrasi: jika `username` masih `None`, username diekstrak dan disimpan, `Welcome` dikirim, lalu `continue` kembali ke atas loop. Pesan selanjutnya diproses normal sebagai `chat` atau `command`.

Blok `finally` pada loop dalam memastikan socket client selalu ditutup dan dihapus dari `clients` meskipun terjadi exception, sebelum server kembali ke `accept()`.

---

## server-thread.py

Setiap client ditangani oleh thread terpisah, sehingga server dapat melayani banyak client simultan. Dictionary `clients` diakses dari banyak thread sehingga semua operasi baca-tulis ke `clients` menggunakan `clients_lock`.

### send_json, recv_json, send_file_bytes, recv_file_bytes

`send_json` dan `recv_json` dibungkus try/except yang mengembalikan `None` pada error, bukan raise exception, agar thread client bisa mendeteksi koneksi putus dengan memeriksa nilai return saja.

`send_file_bytes` dan `recv_file_bytes` identik dengan server-sync, namun masing-masing berjalan di thread yang berbeda sehingga pemblokiran di satu thread tidak mempengaruhi client lain.

### add_client dan remove_client

`add_client` mendaftarkan socket ke `clients` dengan lock. `remove_client` melakukan kebalikannya: mengambil username dengan lock, menghapus dari dict, menutup socket, lalu jika `announce=True` memanggil `broadcast` untuk memberitahu client lain bahwa user telah keluar.

### broadcast

Mengambil snapshot list socket dari `clients` dengan lock, lalu melepas lock sebelum mulai mengirim. Ini penting karena `send_json` bisa memakan waktu dan memegang lock terlalu lama akan memblokir thread lain. Socket yang gagal dikumpulkan dalam list `disconnected` dan dikembalikan ke pemanggil untuk diproses, bukan langsung dihapus di dalam `broadcast` agar tidak terjadi rekursi lock.

### start_upload dan finish_upload

`start_upload` mengambil username dari `clients` dengan lock di awal, lalu mengirim `upload_ready` dan langsung memanggil `recv_file_bytes` secara blocking. Thread client ini diblokir selama durasi upload berlangsung, tapi client lain tidak terpengaruh karena masing-masing punya thread sendiri.

`finish_upload` mengirim `upload_done` ke client pengunggah dan memanggil `broadcast` untuk memberitahu semua client lain bahwa file baru tersedia.

### handle_command

Sama seperti server-sync untuk `list`. Untuk `upload`, memanggil `start_upload`. Untuk `download`, mengambil ukuran file, mengirim `download_ready`, lalu memanggil `send_file_bytes` secara blocking di thread client yang bersangkutan.

### client_thread

Entry point setiap thread client. Pesan pertama yang diterima adalah pesan `join`, username diambil, client didaftarkan, `Welcome` dikirim, dan `broadcast` memberitahu client lain. Kemudian masuk ke loop yang memanggil `recv_json` blocking. Pesan `chat` di-broadcast, pesan `command` diteruskan ke `handle_command`. Jika `recv_json` mengembalikan `None`, loop berhenti dan `remove_client` dipanggil untuk membersihkan state dan memberitahu client lain.

### start_server

Membuat socket dan masuk ke loop `accept()`. Setiap koneksi baru membuat thread daemon baru dengan target `client_thread`. Thread utama hanya bertugas menerima koneksi baru.

---

## server-select.py

Server single-threaded yang menangani banyak client menggunakan `select.select`. Tidak ada blocking I/O; semua socket diset ke mode non-blocking dengan `setblocking(False)`.

### Struktur State Per client

```python
clients[sock] = {
    "username": None,
    "address": addr,
    "json_buffer": b"",
    "expected_json_length": None,
    "state": "json",          # "json" | "upload" | "download"
    "upload_file": None,
    "upload_filename": None,
    "upload_remaining": 0,
    "download_file": None,
    "download_remaining": 0
}
```

`json_buffer` menampung byte yang sudah diterima tapi belum membentuk pesan JSON lengkap. `expected_json_length` menyimpan panjang payload yang sedang ditunggu. `state` menentukan bagaimana byte mentah dari socket diperlakukan. `upload_remaining` dan `download_remaining` melacak sisa byte yang masih perlu diproses.

### add_client, cleanup_upload_state, cleanup_download_state

`add_client` mendaftarkan state awal client. `cleanup_upload_state` menutup file handle upload yang terbuka, mereset semua field upload, dan mengembalikan `state` ke `"json"`. `cleanup_download_state` melakukan hal yang sama untuk sisi download.

### remove_client

Dipanggil saat client disconnect atau error. Memanggil `cleanup_upload_state` jika sedang upload dan menghapus file parsial. Memanggil `cleanup_download_state` jika sedang download. Menghapus socket dari list `inputs` dan `outputs`. Jika client sudah punya username, `broadcast` dijalankan untuk memberitahu client lain.

### broadcast

Iterasi semua socket di `clients` dan mengirim JSON. Hanya mengirim ke client yang sudah punya username (sudah login). Socket yang gagal dikumpulkan dan dikembalikan sebagai list untuk dibersihkan oleh pemanggil.

### parse_json_messages

Dipanggil setiap kali ada data baru di `json_buffer`. Bekerja dalam dua tahap berulang:

Tahap pertama: jika `expected_json_length` belum diset dan buffer sudah punya minimal 4 byte, 4 byte pertama diambil sebagai header dan panjang payload disimpan ke `expected_json_length`. Buffer dipotong 4 byte dari depan.

Tahap kedua: jika `expected_json_length` sudah diset dan buffer sudah punya cukup byte, payload sebesar `expected_json_length` diambil dari depan buffer, di-decode sebagai JSON, dan ditambahkan ke list hasil. `expected_json_length` direset ke `None` dan loop kembali ke tahap pertama untuk memeriksa apakah ada pesan berikutnya di buffer.

Loop berhenti ketika buffer tidak cukup untuk salah satu tahap. Ini menangani kasus di mana satu `recv` mengandung beberapa pesan sekaligus, atau pesan yang datang terpotong-potong.

### start_upload

Membuka file di `server_files/` untuk ditulis, menyimpan handle-nya di state client, mengeset `state` ke `"upload"`, dan menyimpan `filesize` sebagai `upload_remaining`. Mengirim `upload_ready` ke client sebagai sinyal untuk mulai mengirim bytes.

### finish_upload

Memanggil `cleanup_upload_state` yang menutup file, mengirim `upload_done` ke uploader, dan memanggil `broadcast` ke semua client. Mengembalikan list client yang gagal dibroadcast.

### handle_command

- `list`: Mengirim isi direktori yang sudah diurutkan.
- `upload`: Memanggil `start_upload`.
- `download`: Membuka file, menyimpan handle di state client, mengeset `state` ke `"download"`, mengirim `download_ready`, dan menambahkan socket ke list `outputs` agar `select` mulai memantau kesiapan kirim socket ini.

### handle_message

Dispatcher pesan JSON yang sudah diparsing. Jika `username` masih `None`, pesan pertama dianggap sebagai `join` dan username diekstrak. Setelah login, memproses `chat` (broadcast ke semua) atau `command` (ke `handle_command`).

### handle_upload_bytes

Dipanggil saat `state` client adalah `"upload"` dan ada data mentah dari socket. Menghitung berapa byte yang boleh ditulis dengan `min(len(data), upload_remaining)`. Byte kelebihan (`leftover`) di luar ukuran file disimpan kembali ke `json_buffer` karena sudah merupakan awal dari pesan JSON berikutnya.

Setelah menulis ke file, `upload_remaining` dikurangi. Jika sudah nol, `finish_upload` dipanggil. Kemudian jika ada `leftover` dan `state` sudah kembali ke `"json"`, `parse_json_messages` dipanggil untuk memproses pesan yang sudah ada di buffer.

### handle_download_bytes

Dipanggil saat socket client masuk ke daftar `writable` dari `select`. Membaca satu chunk dari file handle dan mengirimkannya ke socket. Jika `send` mengirim lebih sedikit dari yang dibaca (partial send), file pointer diputar mundur sebesar selisihnya agar data yang belum terkirim dikirim ulang pada pemanggilan berikutnya:

```python
client["download_file"].seek(
    client["download_file"].tell() - (len(chunk) - sent)
)
```

Jika file sudah habis atau `download_remaining` mencapai nol, `cleanup_download_state` dipanggil dan socket dikeluarkan dari `outputs`. `BlockingIOError` di-catch dan di-ignore karena socket non-blocking dapat mengembalikan error ini saat buffer kirim sedang penuh, dan cukup ditunggu sampai `select` melaporkan socket siap kembali.

### start_server dan event loop

Socket server diset non-blocking. List `inputs` berisi server socket dan semua client socket. List `outputs` hanya berisi socket yang sedang dalam mode download.

`select.select(inputs, outputs, inputs)` memblokir sampai minimal satu socket siap. Untuk setiap socket di `readable`:

- Jika itu server socket, `accept()` client baru, set non-blocking, daftarkan ke `clients`, dan tambahkan ke `inputs`.
- Jika client socket, baca data dengan `recv`. Jika `state` adalah `"upload"`, data diteruskan ke `handle_upload_bytes`. Jika tidak, data ditambahkan ke `json_buffer` dan `parse_json_messages` dijalankan. Ada pengecekan setelah setiap pesan diproses: jika `state` berubah menjadi `"upload"` dan buffer tidak kosong, sisa buffer langsung diproses sebagai upload bytes tanpa menunggu `recv` berikutnya.

Untuk setiap socket di `writable`, jika `state`-nya `"download"`, `handle_download_bytes` dipanggil. Untuk socket di `exceptional`, `remove_client` dipanggil.

---

## server-poll.py

Fungsionalitas identik dengan `server-select.py`. Perbedaannya hanya pada mekanisme event notification yang digunakan: `select.poll` sebagai pengganti `select.select`. `poll` hanya tersedia di Unix/Linux.

### Perbedaan Arsitektur dari select

`select.select` menerima list object socket Python secara langsung. `select.poll` bekerja dengan file descriptor integer, sehingga diperlukan dictionary tambahan `fd_to_socket` untuk memetakan file descriptor ke object socket:

```python
fd_to_socket = {}
```

Saat client terhubung, `client_sock.fileno()` digunakan untuk mendapatkan file descriptor, disimpan ke `fd_to_socket`, lalu didaftarkan ke poller:

```python
poller.register(client_fd, select.POLLIN | select.POLLHUP | select.POLLERR)
```

`select.select` menggunakan tiga list terpisah (readable, writable, exceptional). `poll` menggunakan satu bitmask event per socket:

| Flag | Keterangan |
|---|---|
| `POLLIN` | Data tersedia untuk dibaca |
| `POLLOUT` | Socket siap menerima data kiriman |
| `POLLHUP` | Koneksi terputus di sisi remote |
| `POLLERR` | Error pada socket |
| `POLLNVAL` | File descriptor tidak valid |

### Modifikasi Event Saat Download

Pada `select`, socket ditambahkan ke list `outputs` saat download dimulai dan dihapus saat selesai. Pada `poll`, event mask socket dimodifikasi dengan `poller.modify`:

```python
# saat download dimulai (di handle_command)
poller.modify(fd, select.POLLIN | select.POLLOUT | select.POLLHUP | select.POLLERR)

# saat download selesai (di handle_download_bytes)
poller.modify(fd, select.POLLIN | select.POLLHUP | select.POLLERR)
```

### remove_client

Selain menghapus dari `clients`, juga harus memanggil `poller.unregister(fd)` secara eksplisit dan menghapus entri dari `fd_to_socket`. Jika tidak, poller akan terus melaporkan event untuk file descriptor yang sudah tidak valid.

### Event Loop

`poller.poll()` memblokir sampai ada event dan mengembalikan list `(fd, event)`. Setiap fd di-lookup ke socket melalui `fd_to_socket`. Pengecekan dilakukan dengan bitwise AND terhadap bitmask:

```python
if event & (select.POLLHUP | select.POLLERR | select.POLLNVAL):
    remove_client(...)

if event & select.POLLIN:
    # baca data

if event & select.POLLOUT:
    # kirim data download
```

Logika setelah pengecekan event identik dengan `server-select.py`: baca data ke buffer, parse JSON, atau proses upload/download bytes.

---
