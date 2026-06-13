# Bullet and Bluff Online (Liar's Deck)

![Python](https://img.shields.io/badge/Python-3.x-blue?logo=python&logoColor=white)
![GUI](https://img.shields.io/badge/GUI-Pygame-orange)
![Network](https://img.shields.io/badge/Network-Socket%20TCP-brightgreen)
![Status](https://img.shields.io/badge/Status-Completed-brightgreen)

Repositori ini berisi implementasi **Bullet and Bluff Online** (Liar's Deck), sebuah permainan kartu multiplayer online. Dokumentasi ini merinci spesifikasi fitur dan mendokumentasikan seluruh class serta fungsi Python dalam codebase.

---

## Daftar Isi

1. [Pemenuhan Persyaratan & Lokasi Implementasi](#-1-pemenuhan-persyaratan--lokasi-implementasi)
2. [Alasan Pemilihan Protokol TCP](#-2-alasan-pemilihan-protokol-tcp)
3. [Arsitektur Kode: Sisi Client (`client/`)](#-3-arsitektur-kode-sisi-client-client)
4. [Arsitektur Kode: Sisi Server (`server/`)](#-4-arsitektur-kode-sisi-server-server)
5. [Arsitektur Kode: Shared Utilities (`shared/`)](#-5-arsitektur-kode-shared-utilities-shared)

---

## 1. Pemenuhan Persyaratan & Lokasi Implementasi

Berikut adalah rincian lokasi implementasi setiap ketentuan:

### Ketentuan Dasar
| Ketentuan | Lokasi File Terkait | Penjelasan Implementasi |
|-----------|----------------------|-------------------------|
| **Fokus Sinkronisasi Jaringan** | `server/game_engine.py`, `client/game_state.py` | Aplikasi menggunakan model *Server-Authoritative*. Server (`GameEngine`) menghitung kebenaran permainan dan mem-broadcast-nya melalui paket `S_GAME_STATE_UPDATE` yang dirender ulang oleh `GameState` klien. |
| **Minimal 2 Orang** | `server/lobby_manager.py` | Antrean di `LobbyManager` mendukung minimal 2 klien dan maksimal 4 klien. Jika belum mencapai 4 orang, timer menunggu hingga minimal 2 pemain terkumpul sebelum match dimulai. |
| **Room System** | `server/room_manager.py` | Fungsi `RoomManager.create_room` membuat objek `Room` ber-UUID unik dengan `GameEngine` tersendiri sehingga sesi tidak saling tumpang tindih. |
| **Matchmaking** | `server/lobby_manager.py` | Fungsi `LobbyManager.add_to_queue` memasukkan klien ke dalam `deque`, lalu `LobbyManager._check_loop` mengevaluasi dan memasangkan *match* secara otomatis. |
| **Protokol (TCP/UDP)** | `server/main_server.py`, `client/network.py` | Menggunakan `socket.SOCK_STREAM` dari standard library Python untuk koneksi TCP. |
| **Game Engine** | `client/main_client.py` dkk | Seluruh antarmuka dan visual kartu dirender menggunakan *Surface* dan *Blit* dari library `pygame`. |

### Fitur Wajib & Bonus
| Fitur Wajib | Lokasi File Terkait | Penjelasan Implementasi |
|-------------|----------------------|-------------------------|
| **Real-time Update** | `server/client_handler.py` | Setiap aksi klien diterima server dan langsung memicu `ClientHandler.broadcast_room`. |
| **Game State Sync** | `server/client_handler.py` | Pemanggilan `send_game_state()` merangkai paket JSON berisi status tumpukan kartu dan nyawa, kemudian `GameState.update_from_server` pada sisi klien memparsanya. |
| **Reconnect Handling** | `server/client_handler.py` | Setiap klien memiliki `session_token`. Saat koneksi terputus, server mempertahankan sesi selama 30 detik melalui `ClientHandler.reconnect_timeout()`. Jika klien kembali terhubung dan memanggil `_handle_reconnect()`, sesi sebelumnya dipulihkan. |
| **Ping Indicator** | `client/components/ping_display.py` | Komponen merender durasi waktu tempuh dari fungsi `PingDisplay.should_send_ping()` ke `PingDisplay.on_pong_received()`. |
| **Logging Player** | `server/logger.py` | Class `GameLogger` mencatat event seperti `log_connect()`, `log_liar_call()`, dan lainnya ke dalam file di direktori `/logs/`. |
| **Anti-Invalid Packet** | `server/packet_validator.py` | `RateLimiter.check()` menolak request yang melebihi batas frekuensi dari suatu IP, sementara `validate_packet()` memvalidasi struktur payload JSON. |
| **(Bonus) Dedicated Game Server** | `server/main_server.py` | Berjalan secara headless (tanpa UI), tidak bergantung pada klien, sehingga dapat di-deploy pada server cloud. |

---

## 2. Alasan Pemilihan Protokol TCP

**Protokol Utama:** TCP (Transmission Control Protocol)

Permainan *Liar's Deck* dirancang dengan siklus *turn-based* asinkron. Aspek terpenting dalam komputasinya adalah **urutan** dan **keabsahan** paket.

Jika menggunakan UDP (connectionless), ada risiko *packet loss* di mana aksi klien tidak diterima server, sehingga sesi game dapat terhenti karena status kedua pihak tidak sinkron. TCP mengatasi hal ini melalui **Guaranteed Delivery** dan pengurutan paket secara linear (jika 2 aksi dikirim dalam selang 0.1 detik, server memprosesnya secara berurutan). Overhead latensi TCP tidak menjadi masalah pada game berbasis giliran, di mana reliabilitas pengiriman lebih diprioritaskan daripada transmisi instan seperti pada UDP.

---

## 3. Arsitektur Kode: Sisi Client (`client/`)

Berikut adalah dokumentasi seluruh class dan fungsi dalam folder klien.

### Inti Aplikasi Client
**File: `client/config.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `get_custom_font` | Memuat file `.ttf` kustom dari direktori `assets/`. Jika file tidak ditemukan, fungsi melakukan *graceful fallback* ke font default Pygame untuk mencegah crash. |

**File: `client/game_state.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `GameState` | Class representasi status lokal klien atas kondisi server (jumlah kartu, nyawa, giliran, dan tipe kartu aktif di meja). |
| `__init__` | Menginisialisasi seluruh properti lokal dengan nilai `None` atau array kosong. |
| `reset` | Mengosongkan seluruh variabel untuk skenario klien keluar sesi dan memulai matchmaking baru. |
| `update_from_server` | Menerima `dict` state dari paket server dan memperbarui variabel internal lokal sebagai acuan validasi UI. |
| `is_my_turn` | Mengembalikan `True` jika `player_id` lokal sesuai dengan giliran yang sedang aktif. |
| `my_info` | Mencari dan mengembalikan data pemain lokal dari array `players` dalam state room. |
| `opponents` | Menyaring daftar pemain dengan mengecualikan pemain lokal, mengembalikan data lawan berupa nama dan jumlah kartu. |
| `opponent_username` | Mengekstrak nama lawan khusus untuk format tampilan duel 1v1 ("Vs [Nama Lawan]") di bagian atas layar. |
| `my_alive` | Memeriksa nilai integer HP; mengembalikan `True` jika nyawa masih > 0. Jika tidak, tombol aksi di UI dinonaktifkan. |
| `set_status` | Mengatur pesan notifikasi *flash* beserta timer pudar yang ditampilkan di tengah layar. |

**File: `client/main_client.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `LiarsDeckClient` | Class utama yang membungkus Pygame Display dan antarmuka transmisi socket. |
| `main` | Titik masuk `__main__` yang membuat instansi objek klien dan menjalankan loop utamanya. |
| `__init__` | Mengalokasikan kanvas Pygame window, menyiapkan frame-clock, serta mengatur status awal klien ke `STATE_LOGIN`. |
| `run` | Loop utama game (*while-loop*). Menetapkan FPS di 60, memproses event OS (Quit, Alt+F4), memperbarui input, dan memanggil render UI. |
| `toggle_fullscreen` | Mengalihkan mode display Pygame antara mode jendela dan *Borderless Fullscreen*. |
| `_handle_event` | Menerima event Mouse/Key dari Pygame dan meneruskan logikanya ke instansi layar yang sedang aktif (Lobby/Game). |
| `_do_connect` | Mengambil string IP dan port dari field input UI, kemudian menginisiasi koneksi TCP sinkron ke `NetworkClient`. |
| `_process_network` | Mengambil seluruh paket dari antrean `poll_packets()` dan memprosesnya. |
| `_route_packet` | Router paket yang mencocokkan tipe string JSON untuk mengeksekusi transisi logika secara otomatis (seperti memicu animasi roulette atau pergantian state ke GameOver). |
| `_update` | Meneruskan nilai *Delta-Time* dari siklus loop ke class layar yang aktif untuk keperluan kalkulasi animasi. |
| `_draw` | Mengosongkan frame setiap siklus dan mendelegasikan render ke class GUI yang aktif. |
| `_draw_error_overlay` | Menggambar overlay error semi-transparan berwarna merah di bagian bawah layar. |
| `_show_error` | Mengaktifkan overlay error dengan argumen string pesan yang diterima dari server. |

**File: `client/network.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `NetworkClient` | Wrapper socket berbasis thread yang memisahkan eksekusi I/O TCP agar tidak memblokir frame rate Pygame. |
| `__init__` | Menginisialisasi struktur `Deque` thread-safe dan referensi objek `socket`. |
| `is_connected` | Memeriksa flag koneksi untuk memastikan socket masih aktif dan valid. |
| `connect` | Menginisiasi koneksi TCP ke alamat IP server; jika berhasil, men-spawn daemon thread untuk membaca buffer secara asinkron. |
| `disconnect` | Menutup file descriptor socket secara bersih, mengizinkan server mencatat pemutusan koneksi. |
| `send_packet` | Menserialisasi dict Python ke JSON, mengonversinya ke byte ASCII dengan terminator `\n`, dan mengirimkannya melalui `sendall()`. |
| `_receive_loop` | Thread daemon yang membaca chunk 1024-byte secara kontinu, memisahkan payload berdasarkan karakter `\n`, dan mendeserialisasinya menjadi paket. |
| `poll_packets` | Menguras dan mengembalikan seluruh isi antrean paket secara atomik untuk diproses oleh game loop. |

### Komponen GUI (`client/components/`)
**File: `client/components/button.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `Button` | Objek visual berbentuk persegi untuk kontrol klik di Pygame. |
| `get_button_image` | Static method yang membuat dan men-cache bitmap latar tombol untuk efisiensi rendering. |
| `__init__` | Menginisialisasi hit-box (`pygame.Rect`) pada koordinat X, Y berdasarkan ukuran teks yang diberikan. |
| `_get_font` | Mengambil font tebal kustom untuk teks tombol. |
| `draw` | Merender teks dan bitmap tombol, dengan deteksi hover berdasarkan posisi kursor mouse. |
| `is_clicked` | Mengembalikan `True` jika event `MOUSEBUTTONDOWN` terjadi di dalam area rektangular tombol. |
| `set_text` | Mengubah label tombol setelah inisialisasi (contoh: dari "Play" ke "Wait"). |
| `set_pos` | Memindahkan posisi X, Y tombol untuk penyesuaian tata letak setelah perubahan dimensi layar. |
| `center_x` | Menempatkan tombol pada posisi horizontal tengah berdasarkan lebar display. |

**File: `client/components/card_sprite.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `CardSprite` | Representasi visual satu kartu remi dalam tampilan grafis. |
| `_get_font` | Mengambil font tebal untuk merender angka nominal kartu. |
| `get_card_image` | Membuat dan men-cache bitmap kartu dengan warna dasar dan simbol suit (Spades/Hearts/dll.) sebagai aset statis. |
| `get_card_back_image` | Membuat dan men-cache bitmap sisi belakang kartu dengan pola kotak-kotak sebagai aset statis. |
| `__init__` | Mendefinisikan rank dan suit kartu, serta menginisialisasi y-offset untuk animasi hover. |
| `rect` | Getter yang mengembalikan `pygame.Rect` kartu berdasarkan posisi y-offset animasi saat ini. |
| `set_position` | Menerima koordinat awal penempatan kartu dari komponen induk. |
| `update_y` | Menginterpolasi posisi y kartu secara bertahap (delta-time dependent) saat properti `is_hovered` aktif. |
| `draw` | Memilih mode render berdasarkan flag `face-down`, dan menerapkan filter warna emas jika kartu berstatus `is_selected`. |
| `_draw_face_up` | Merender sisi depan kartu menggunakan operasi blit Pygame. |
| `_draw_face_down` | Merender sisi belakang kartu untuk menyembunyikan nilainya dari pemain lain. |
| `contains_point` | Memeriksa apakah titik koordinat `(x, y)` berada di dalam area hit-box kartu. |

**File: `client/components/center_pile.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `CenterPile` | Komponen visual yang menampilkan jumlah kartu dalam tumpukan tengah. |
| `__init__` | Menginisialisasi nilai hitungan awal (N=0). |
| `_get_font` | Menginisialisasi font untuk label "Cards in play". |
| `_get_badge_font` | Menginisialisasi font khusus untuk angka di dalam lencana bulat merah. |
| `set_position` | Menetapkan posisi elemen tumpukan pada area meja di layar game. |
| `set_count` | Memperbarui jumlah kartu yang ditampilkan sesuai data dari paket JSON server. |
| `draw` | Menggambar representasi visual tumpukan kartu beserta lencana merah penunjuk jumlah. |

**File: `client/components/hand_display.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `HandDisplay` | Manajer yang mengelola dan menampilkan kumpulan objek `CardSprite` dalam susunan kipas. |
| `__init__` | Menginisialisasi array kosong sebagai wadah objek `CardSprite`. |
| `set_cards` | Membaca list `(suit, rank)` dari luar dan mengonversinya secara massal menjadi objek `CardSprite`. |
| `set_facedown` | Mengatur semua kartu agar ditampilkan dalam posisi tertutup (face-down), umumnya digunakan saat pemain kehabisan nyawa. |
| `set_position` | Menetapkan batas rektangular area penampung kartu di bagian bawah layar. |
| `_layout` | Menghitung distribusi spasi antar kartu (*fanning algorithm*) agar tumpang tindih kartu terlihat natural saat jumlah kartu melebihi kapasitas layar. |
| `draw` | Merender setiap `CardSprite` dari kiri ke kanan sesuai urutan z-index. |
| `handle_click` | Mendeteksi klik pada kartu dengan hit-test dari layer teratas (kanan) ke kiri, kemudian mengubah status seleksi kartu yang terklik. |
| `get_selected_indices` | Mengembalikan daftar indeks kartu yang sedang terpilih untuk dikirim ke server. |
| `get_selected_cards` | Mengembalikan daftar dict `{"suit": "x", "rank": "y"}` dari kartu yang sedang terpilih. |
| `clear_selection` | Mereset status seleksi semua kartu menjadi tidak terpilih. |

**File: `client/components/ping_display.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `PingDisplay` | Komponen UI yang menampilkan latensi jaringan (ping) dalam milidetik. |
| `__init__` | Menginisialisasi nilai ping default 0 ms dan interval pengiriman ping pertama sebesar 2 detik. |
| `_get_font` | Mengambil font berukuran kecil tanpa serif untuk teks indikator ping. |
| `should_send_ping` | Mengembalikan `True` jika interval waktu antar pengiriman ping sudah terlewati, untuk mencegah pengiriman berlebih. |
| `on_pong_received` | Menghitung selisih waktu antara pengiriman ping dan penerimaan pong, kemudian menyimpannya sebagai nilai latensi dalam milidetik. |
| `draw` | Menampilkan nilai ping di sudut kiri atas layar; teks berubah warna menjadi merah jika latensi melebihi batas normal. |

**File: `client/components/roulette_anim.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `RouletteAnimation` | Komponen animasi visual revolver yang ditampilkan saat pemain yang tertangkap berbohong menerima penalti roulette. |
| `__init__` | Menginisialisasi batas kecepatan rotasi, konstanta perlambatan (*friction*), dan jumlah ruang silinder (1–6). |
| `_get_font` | Mengambil font untuk teks hasil akhir animasi ("BANG!" / "CLICK!"). |
| `start_spin` | Mengaktifkan animasi dengan mengatur state ke aktif dan menetapkan kecepatan rotasi awal. |
| `show_result` | Menerima hasil vonis dari payload JSON server dan menyimpannya untuk ditampilkan sebagai teks akhir animasi. |
| `is_active` | Mengembalikan `True` jika animasi masih berjalan atau fase fade-out belum selesai. |
| `reset` | Mereset seluruh variabel animasi (kecepatan, vonis, visibilitas) ke nilai awal. |
| `draw` | Memilih dan memanggil sub-rutin render yang sesuai berdasarkan state animasi: silinder berputar atau tampilan hasil akhir. |
| `_draw_spinning` | Merender efek silinder berputar dengan penurunan kecepatan bertahap dan *alpha-blending* untuk kesan blur. |
| `_draw_result` | Menampilkan teks hasil akhir ("BANG!" atau "CLICK!") dengan warna merah/hijau di tengah revolver yang berhenti. |
| `_draw_cylinder` | Menggambar silinder revolver berenam lubang secara prosedural menggunakan `pygame.draw.circle`, tanpa aset gambar eksternal. |

### Layar GUI Klien (`client/screens/`)
**File: `client/screens/screen_game.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `GameScreen` | Class utama yang mengelola tampilan layar permainan. Seluruh komponen (kartu, meja, tombol) diinisialisasi di dalamnya. |
| `__init__` | Menginisialisasi objek komponen dan menempatkan dua tombol utama ("Play Cards" & "Call Liar") pada koordinat kanvas yang ditetapkan. |
| `_get_font` | Mengambil font proporsional untuk teks di layar game. |
| `handle_event` | Menangkap event `pygame.MOUSEBUTTONDOWN` dan mencocokkan koordinat klik ke tombol internal. Jika cocok, membentuk `action_dict` yang diteruskan ke router socket sebagai perintah ke server (contoh: `{type: C_PLAY_CARDS}`). |
| `update` | Meneruskan nilai delta-time ke sistem animasi fade-out status, rotasi Roulette, dan pergerakan kartu tangan. |
| `draw` | Merender seluruh elemen layar secara berlapis dari latar belakang hingga elemen terdepan, dengan memanggil sub-fungsi render secara berurutan. |
| `_draw_opponents` | Menampilkan avatar dan jumlah kartu tersisa setiap lawan di bagian atas layar. |
| `_draw_table_card` | Menampilkan rank target ronde saat ini (contoh: "Must play Aces") sebagai teks di area meja. |
| `_draw_last_play` | Menampilkan informasi aksi terakhir (contoh: "Fulan played 2 Jacks") sebagai referensi bagi pemain untuk memutuskan apakah memanggil Liar. |
| `_draw_player_bar` | Menampilkan panel pemain lokal di sudut kanan bawah, berisi nama dan jumlah nyawa. |
| `_draw_turn_indicator` | Menampilkan border visual pada ikon pemain yang sedang mendapat giliran berdasarkan `current_turn_id`. |
| `_draw_status` | Menampilkan pesan status sementara (contoh: "It's your turn!") yang berangsur memudar seiring waktu. |
| `_draw_reveal` | Menampilkan kartu-kartu dari tumpukan tengah setelah seruan "Call Liar" agar semua pemain dapat melihat isinya. |
| `_draw_roulette_overlay` | Menerapkan filter peredupan layar dan merender `RouletteAnimation` di tengah sebagai overlay. |

**File: `client/screens/screen_gameover.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `GameOverScreen` | Layar akhir permainan yang menampilkan hasil dan statistik ketika HP pemain mencapai 0. |
| `__init__` | Mewarisi sisa `GameState` pasca pertandingan dan menginisialisasi dua tombol navigasi (Play Again dan Main Menu). |
| `_get_font` | Memuat font berukuran besar untuk teks pengumuman hasil ("YOU LOSE!" / "WINNER!"). |
| `handle_event` | Mendeteksi klik pada tombol menu dan meneruskan perintah perpindahan state ke manajer klien utama. |
| `draw` | Mengisi latar layar dengan warna hitam dan merender statistik permainan. |
| `_draw_stats` | Menampilkan statistik akhir dari data server: jumlah ronde yang dijalani dan sisa HP. |
| `_draw_particles` | Menampilkan efek partikel konfeti pada layar pemenang menggunakan titik-titik lingkaran Pygame. |

**File: `client/screens/screen_lobby.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `LobbyScreen` | Layar antrian yang ditampilkan saat menunggu server menemukan pemain lain untuk *matchmaking*. |
| `__init__` | Menginisialisasi tombol "Batal" untuk mengirim paket `C_LEAVE_LOBBY`. |
| `_get_font` | Mengambil font untuk teks "Waiting for match". |
| `handle_event` | Mendeteksi klik tombol "Cancel" dan memicu aksi *disconnect* dari sisi klien. |
| `draw` | Merender teks status, indikator ping, dan memanggil `_draw_spinner`. |
| `_draw_spinner` | Menggambar ikon loading berputar menggunakan kalkulasi sinus-kosinus untuk menunjukkan bahwa aplikasi aktif. |

**File: `client/screens/screen_login.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `_InputField` | Sub-class helper untuk field teks yang menangani input keyboard, kedipan kursor, rendering rektangular, dan operasi backspace. |
| `__init__` (InputField) | Menetapkan batas kotak dan placeholder teks abu-abu jika variabel teks kosong. |
| `handle_event` (InputField) | Memfilter event keyboard; menerima karakter alfanumerik dan titik, serta membatasi panjang input maksimum. |
| `draw` (InputField) | Merender kotak putih beserta teks input secara real-time. |
| `LoginScreen` | Layar login yang menyediakan input username dan alamat IP server, serta memicu koneksi TCP. |
| `__init__` (LoginScreen) | Menginisialisasi dua komponen `_InputField` untuk username dan alamat IP server. |
| `_get_font` s/d `_get_err_font` | Menyediakan font dalam berbagai ukuran (judul, sub-judul, dan pesan error) untuk layar login. |
| `_load_title_img` | Memuat aset gambar logo jika tersedia di direktori klien. |
| `handle_event` (LoginScreen) | Mendistribusikan event keyboard ke field input yang aktif, atau memicu koneksi saat tombol Enter/Connect ditekan. |
| `_try_connect` | Mengambil nilai dari field input, menetapkan port default (`8080`), dan meneruskan data koneksi ke `_do_connect` di *main_client*. |
| `_draw_bg_video` | Menampilkan efek partikel bergerak sebagai latar belakang animasi pada form login. |
| `draw` (LoginScreen) | Merender seluruh elemen layar: judul, field input aktif, tombol sambung, dan efek partikel latar. |
| `_draw_bg_motifs` | Menggambar elemen dekoratif geometris (segitiga/garis) sebagai bingkai visual layar login. |

---

## 4. Arsitektur Kode: Sisi Server (`server/`)

### Sistem Konektivitas
**File: `server/main_server.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `LiarsDeckServer` | Class utama server yang menginisialisasi komponen pengelola (*LobbyManager*, *RoomManager*, *GameLogger*, *RateLimiter*) dan membuka TCP socket listener. |
| `main` | Titik masuk command-line; membuat instansi server, menangani sinyal interupsi keyboard (`SIGINT/Ctrl+C`), dan menjalankan server. |
| `__init__` | Menyimpan konfigurasi IP:Port, menginisialisasi dictionary `SessionTokens` untuk klien yang terhubung secara konkuren, serta menyiapkan *Mutex-Lock* thread. |
| `start` | Menjalankan `socket.bind` dan `socket.listen`, kemudian memasuki loop tak terbatas yang menerima koneksi masuk via `socket.accept()` dan men-spawn `ClientHandler` baru per koneksi. |
| `start_match` | Dipanggil oleh LobbyManager saat match terbentuk. Membuat room baru via RoomManager, memperbarui sesi pemain, dan mengirim paket `S_MATCH_FOUND` kepada semua pemain terkait. |
| `shutdown` | Menutup server secara bersih: menutup socket server dan semua koneksi klien aktif untuk mencegah zombie socket. |
| `signal_handler` | Menangkap sinyal interupsi `Ctrl+C` dari OS dan meneruskannya ke metode `shutdown` untuk penghentian yang bersih. |

**File: `server/client_handler.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `ClientHandler` | Thread object yang mengelola siklus hidup satu koneksi klien secara eksklusif hingga koneksi terputus. |
| `__init__` | Menyimpan referensi ke komponen server (*LobbyManager*, *RoomManager*, *RateLimiter*, *GameLogger*), socket klien, dan `Player_ID`. |
| `send_packet` | Mengonversi dict Python ke string UTF-8, menambahkan terminator `\n`, dan mengirimnya ke klien via `sendall()`. |
| `send_to_opponent` | Mengirim paket ke semua pemain di room yang sama kecuali pengirim asli (untuk notifikasi seperti "Enemy Played a Card"). |
| `broadcast_room` | Mengirim paket ke seluruh pemain dalam room yang sama tanpa pengecualian. |
| `send_game_state` | Meminta rekonstruksi state dari GameEngine dengan data kartu lawan yang disembunyikan (`'hidden'`), lalu mengirimkannya secara eksklusif ke klien ini. |
| `send_your_turn` | Mengirim notifikasi giliran kepada pemain yang `Player_ID`-nya sesuai dengan `current_turn_id`, sehingga UI-nya dapat mengaktifkan input. |
| `run` | Loop penerima data utama. Membaca stream 1024-byte dari socket secara kontinu dan meneruskannya ke `_process_buffer`. |
| `_process_buffer` | Memisahkan buffer berdasarkan karakter `\n`, mendeserialisasi setiap segmen sebagai JSON, dan meneruskannya ke `_route_packet`. |
| `_route_packet` | Memeriksa field `type` pada paket, memvalidasi rate limit, lalu mendelegasikan ke handler yang sesuai. |
| `_handle_connect` | Memproses koneksi awal klien: menyimpan username, membuat `session_token`, dan mengirimkan respons sambutan. |
| `_handle_reconnect` | Memvalidasi `session_token` dari permintaan koneksi ulang dan memulihkan sesi pemain di room yang sebelumnya aktif. |
| `_handle_join_lobby` | Mendaftarkan pemain ke antrean matchmaking `LobbyManager`. |
| `_handle_leave_lobby` | Menghapus pemain dari antrean `LobbyManager` ketika pemain menekan Cancel dari UI. |
| `_handle_play_cards` | Memvalidasi dan memproses aksi memainkan kartu: memverifikasi giliran, mengeksekusi aksi di GameEngine, dan mem-broadcast hasilnya. |
| `_handle_call_liar` | Memproses seruan "Call Liar": menghentikan alur giliran dan mendorong GameEngine untuk menjalankan `check_cards_truth()`. |
| `_handle_roulette_pull` | Meneruskan permintaan eksekusi roulette ke GameEngine dan mem-broadcast hasilnya (kena penalti atau selamat). |
| `_handle_ping` | Merespons paket `C_PING` dengan langsung mengirimkan `S_PONG` tanpa melibatkan GameEngine. |
| `_handle_chat` | Mendistribusikan pesan chat pemain ke seluruh anggota room. |
| `_handle_ready` | Menandai klien sebagai siap. Jika semua pemain di room sudah siap, GameEngine mendistribusikan kartu awal. |
| `_start_match` | Mengasosiasikan handler ini ke room yang ditetapkan setelah matchmaking berhasil, agar pengiriman paket tidak salah sasaran. |
| `_handle_disconnect` | Menangani pemutusan koneksi: menonaktifkan profil pemain, menutup buffer, mencatat kejadian ke log, dan memulai hitung mundur `reconnect_timeout`. |
| `cleanup` | Menghapus token dan data pemain dari penyimpanan sesi server dan RateLimiter setelah timeout diskoneksi berakhir. |
| `reconnect_timeout` | Menunggu 30 detik setelah diskoneksi sebelum memanggil `forfeit()` pada GameEngine, memberikan waktu bagi pemain untuk reconnect. |

### Sistem Logika & Aturan
**File: `server/game_engine.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `PlayerState` | Struktur data in-memory yang menyimpan identitas, kartu di tangan, dan HP satu pemain. |
| `GameEngine` | Class inti yang mengelola seluruh logika permainan (aturan, distribusi kartu, giliran, dan validasi klaim) secara independen dari lapisan jaringan (*socket-agnostic*). |
| `__init__` | Menginisialisasi wadah data pemain, tumpukan kartu tengah (`center_pile`), rank target konstan di meja, dan pointer giliran. |
| `create_deck` | Membuat satu deck 52 kartu dari kombinasi suit dan rank, kemudian mengacak urutannya. |
| `deal_cards` | Mendistribusikan kartu dari deck ke masing-masing pemain sesuai jumlah yang ditentukan. |
| `choose_table_card` | Memilih rank target secara acak (contoh: '8', 'Jack', 'King') sebagai kartu yang harus dimainkan sepanjang ronde. |
| `current_turn_id` | Mengembalikan `Player_ID` pemain yang sedang mendapat giliran berdasarkan pointer urutan. |
| `_advance_turn` | Memajukan pointer giliran (+1) ke pemain berikutnya secara sirkular, melewati pemain dengan HP 0. |
| `play_cards` | Memindahkan kartu dari tangan pemain ke `center_pile`, memvalidasi giliran, dan menyimpan klaim kartu untuk verifikasi liar. |
| `call_liar` | Menghentikan siklus giliran dan memicu `check_cards_truth()` untuk memeriksa klaim kartu terakhir. |
| `check_cards_truth` | Memeriksa apakah kartu yang dimainkan sesuai dengan klaim. Jika semua kartu cocok, penantang dinyatakan salah; jika ada satu saja yang tidak cocok, pemain sebelumnya dinyatakan berbohong. |
| `pull_roulette` | Menentukan hasil roulette secara probabilistik dan mengurangi HP pemain yang terkena penalti. |
| `check_round_over` | Memeriksa jumlah pemain yang tersisa. Jika hanya satu yang bertahan, menetapkan status GameOver; jika tidak, mereset ronde. |
| `reset_round` | Mengosongkan `center_pile`, mengambil kembali kartu pemain, membuat deck baru, dan menentukan rank target baru untuk memulai ronde berikutnya. |
| `start_game` | Menginisialisasi state permainan: mengatur ronde ke satu, menonaktifkan roulette, menonaktifkan fase reinkarnasi, dan menjalankan `deal_cards()`. |
| `build_state_for_player` | Membangun objek state permainan yang disesuaikan untuk klien tertentu, dengan mengganti data kartu lawan menjadi `'hidden'` untuk mencegah kecurangan. |
| `get_winner` | Mengembalikan `Player_ID` pemain yang masih memiliki HP lebih dari 0 sebagai pemenang. |
| `get_loser` | Mengembalikan daftar pemain yang HP-nya sudah mencapai 0. |
| `set_player_connected` | Mengatur status koneksi pemain. Pemain yang terputus dilewati saat giliran berpindah untuk mencegah game terhenti. |
| `forfeit` | Menetapkan HP pemain menjadi 0 setelah timeout diskoneksi, sehingga pertandingan dapat dilanjutkan atau diakhiri. |

### Manajer Sistem Lainnya
**File: `server/lobby_manager.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `LobbyManager` | Mengelola antrean pemain yang mencari pertandingan dan mengelompokkan mereka saat jumlah memenuhi syarat. |
| `__init__` | Menginisialisasi struktur deque kosong dan menjalankan background thread untuk memantau kelayakan match melalui `_check_loop`. |
| `add_to_queue` | Menambahkan profil klien ke antrean beserta cap waktu masuk sebagai acuan FIFO dan penghitungan waktu tunggu. |
| `_pop_match` | Mengambil dan mengembalikan kelompok 2–4 profil dari bagian depan antrean untuk dijadikan satu match. |
| `_check_loop` | Loop permanen di background thread yang memeriksa kelayakan match secara berkala: jika ada 4 pemain, atau 2 pemain dengan waktu tunggu > 10 detik, match langsung dibentuk. |
| `remove_from_queue` | Menghapus pemain dari antrean saat pemain membatalkan pencarian match. |
| `queue_size` | Mengembalikan jumlah pemain yang saat ini berada dalam antrean. |
| `is_in_queue` | Memeriksa apakah pemain sudah terdaftar dalam antrean untuk mencegah duplikasi. |
| `get_wait_time_remaining` | Mengembalikan sisa waktu tunggu sebelum match dipaksa dibentuk, berdasarkan waktu masuk pemain paling lama dalam antrean. |
| `stop` | Menghentikan background thread matchmaking dengan mengubah flag loop menjadi `False`, memungkinkan server shutdown dengan aman. |

**File: `server/room_manager.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `Room` | Struktur data yang menyimpan instansi `GameEngine` dan daftar `Client ID` untuk satu sesi pertandingan yang terisolasi. |
| `RoomManager` | Mengelola registrasi dan pencarian `Room` aktif melalui kamus berbasis UUID, menjadi titik referensi bagi handler socket. |
| `__init__` | Menginisialisasi kamus kosong sebagai penyimpan data room. |
| `create_room` | Membuat room baru dengan UUID unik, menginisialisasi `GameEngine` baru, mendaftarkan pemain ke dalamnya, dan menyimpannya di kamus RoomManager. |
| `remove_room` | Menghapus room dari kamus setelah pertandingan selesai untuk membebaskan memori. |
| `get_room` | Mengembalikan objek `Room` berdasarkan UUID yang diberikan oleh handler klien. |
| `get_room_by_player` | Mencari room berdasarkan `Player_ID` tanpa mengetahui UUID room, berguna saat pemain reconnect tanpa informasi room. |
| `room_count` | Mengembalikan jumlah room yang sedang aktif sebagai indikator beban server. |
| `all_rooms` | Mengembalikan iterator atas semua room aktif untuk keperluan monitoring dan logging. |

**File: `server/packet_validator.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `validate_packet` | Memvalidasi struktur paket JSON masuk: mengembalikan `False` jika field `type` tidak ada atau tipe propertinya tidak sesuai. |
| `RateLimiter` | Middleware keamanan yang melacak frekuensi request per `Player_ID` menggunakan cap waktu, untuk membatasi pengiriman paket berlebih. |
| `__init__` | Menetapkan batas `requests_per_second` yang diizinkan per koneksi dan menginisialisasi kamus pelacak. |
| `check` | Membandingkan cap waktu terkini dengan riwayat request pemain. Mengembalikan `False` jika frekuensi request melebihi batas yang ditetapkan. |
| `remove_player` | Menghapus data pelacak pemain dari kamus setelah koneksi terputus. |

**File: `server/logger.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `GameLogger` | Class logging sentral yang mencetak dan menyimpan aktivitas server ke file `.log` dengan stempel waktu OS. |
| `__init__` | Membuat direktori `/logs` jika belum ada, kemudian menginisialisasi file log dengan nama berdasarkan waktu. |
| `log_connect` | Mencatat event koneksi baru beserta alamat IP dan `Player_ID` yang diberikan. |
| `log_disconnect` | Mencatat event pemutusan koneksi beserta penyebabnya (Normal Quit, Force Close, atau error socket). |
| `log_play_cards` | Mencatat aksi pemain memainkan kartu beserta klaim nominalnya. |
| `log_liar_call` | Mencatat event seruan "Call Liar" beserta identitas pemain yang menantang. |
| `log_reveal` | Mencatat hasil verifikasi kartu setelah "Call Liar": apakah klaim pemain terbukti jujur atau berbohong. |
| `log_roulette` | Mencatat hasil putaran roulette: pemain selamat atau terkena penalti. |
| `log_game_over` | Mencatat akhir pertandingan beserta `Player_ID` pemenang. |
| `log_invalid_packet` | Mencatat paket tidak valid yang diterima beserta tipe kesalahan strukturnya. |
| `log_info` | Mencetak pesan informasi umum ke stdout. |
| `log_error` | Mencetak pesan error ke stderr saat terjadi kesalahan kritis (contoh: socket error). |
| `log_debug` | Mencetak pesan debug yang hanya aktif di lingkungan pengembangan. |

---

## 5. Arsitektur Kode: Shared Utilities (`shared/`)

Modul utilitas bersama yang digunakan oleh klien maupun server untuk menyeragamkan konstanta, tipe paket, dan fungsi bantu.

**File: `shared/constants.py`**
*(Hanya Variabel Konstanta)*
| Referensi / Enumerasi | Deskripsi |
|----------------------|-----------|
| Variabel Konstanta (e.g. `PHASE_GAME_OVER`) | Mendefinisikan konstanta label status (fase permainan, jenis kartu, kapasitas silinder). Menyeragamkan nilai antara klien dan server untuk mencegah kesalahan akibat *typo* pada komparasi string. |

**File: `shared/packet_types.py`**
*(Hanya Variabel Konstanta)*
| Referensi / Enumerasi | Deskripsi |
|----------------------|-----------|
| `C_CONNECT`, `S_MATCH_FOUND`, dll. | Mendefinisikan konstanta tipe paket untuk sistem routing JSON. Prefiks `C_` menandakan paket dari klien ke server, prefiks `S_` menandakan paket dari server ke klien. |

**File: `shared/utils.py`**
| Class / Fungsi | Deskripsi |
|----------------|-----------|
| `serialize` | Mengonversi dict Python menjadi string JSON tanpa spasi, kemudian mengodekannya sebagai bytes UTF-8 untuk dikirim melalui TCP socket. |
| `deserialize` | Mendekode bytes dari socket menjadi string Unicode, kemudian mengurainya kembali ke dict Python. Mengembalikan `None` jika data tidak valid. |
| `generate_id` | Menghasilkan string ID unik menggunakan UUID v4 berbasis `os.urandom` untuk menghindari tabrakan nama room di server. |
| `generate_session_token` | Menghasilkan token string hexadecimal 32 karakter yang aman secara kriptografis, digunakan sebagai identitas sesi klien untuk keperluan reconnect. |
