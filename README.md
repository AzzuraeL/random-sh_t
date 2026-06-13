# Bullet and Bluff Online (Liar's Deck)

![Python](https://img.shields.io/badge/Python-3.x-blue?logo=python&logoColor=white)
![GUI](https://img.shields.io/badge/GUI-Pygame-orange)
![Network](https://img.shields.io/badge/Network-Socket%20TCP-brightgreen)
![Status](https://img.shields.io/badge/Status-Completed-brightgreen)

Repositori ini berisi implementasi **Bullet and Bluff Online** (Liar's Deck), permainan kartu multiplayer online. Dokumentasi ini merincikan spesifikasi fitur secara komprehensif dan membedah struktur internal dari **SETIAP** class dan fungsi Python di dalam codebase secara menyeluruh dan lengkap.

---

## Daftar Isi

1. [Pemenuhan Persyaratan & Lokasi Implementasi](#-1-pemenuhan-persyaratan--lokasi-implementasi)
2. [Alasan Pemilihan Protokol TCP](#-2-alasan-pemilihan-protokol-tcp)
3. [Arsitektur Kode: Sisi Client (`client/`)](#-3-arsitektur-kode-sisi-client-client)
4. [Arsitektur Kode: Sisi Server (`server/`)](#-4-arsitektur-kode-sisi-server-server)
5. [Arsitektur Kode: Shared Utilities (`shared/`)](#-5-arsitektur-kode-shared-utilities-shared)

---

## 1. Pemenuhan Persyaratan & Lokasi Implementasi

Berikut adalah rincian mutlak di mana setiap Ketentuan diimplementasikan:

### Ketentuan Dasar
| Ketentuan | Lokasi File Terkait | Penjelasan Implementasi |
|-----------|----------------------|-------------------------|
| **Fokus Sinkronisasi Jaringan** | `server/game_engine.py`, `client/game_state.py` | Aplikasi mengusung model *Server-Authoritative*. Server (`GameEngine`) menghitung kebenaran permainan dan mem-broadcastnya lewat paket `S_GAME_STATE_UPDATE` yang dirender ulang oleh `GameState` klien. |
| **Minimal 2 Orang** | `server/lobby_manager.py` | Antrean di `LobbyManager` didesain untuk menampung minimal 2 klien dan maksimal 4 klien. Jika belum 4 orang, timer menunggu agar memastikan minimal 2 orang terkumpul sebelum match dieksekusi. |
| **Room System** | `server/room_manager.py` | Fungsi `RoomManager.create_room` menciptakan objek `Room` ber-UUID mandiri dengan `GameEngine` masing-masing sehingga sesi tidak akan tumpang tindih. |
| **Matchmaking** | `server/lobby_manager.py` | Fungsi `LobbyManager.add_to_queue` memasukkan klien ke dalam `deque`, lalu `LobbyManager._check_loop` mengevaluasi dan memasangkan *match* secara otomatis. |
| **Protokol (TCP/UDP)** | `server/main_server.py`, `client/network.py` | Murni memanfaatkan standard library `socket.SOCK_STREAM` untuk mentransfer koneksi TCP port. |
| **Game Engine** | `client/main_client.py` dkk | Keseluruhan antarmuka dan visual kartu dirender manual menggunakan *Surface* dan *Blit* dari library pihak ketiga `pygame`. |

### Fitur Wajib & Bonus
| Fitur Wajib | Lokasi File Terkait | Penjelasan Implementasi |
|-------------|----------------------|-------------------------|
| **Real-time Update** | `server/client_handler.py` | Setiap klien merespons aksi ke server, server segera mengintersep dan memicu `ClientHandler.broadcast_room`. |
| **Game State Sync** | `server/client_handler.py` | Pemanggilan `send_game_state()` merangkaikan paket JSON raksasa berisi tumpukan kartu dan nyawa, lalu klien `GameState.update_from_server` mencernanya mutlak tanpa negosiasi. |
| **Reconnect Handling** | `server/client_handler.py` | Klien memegang objek acak `session_token`. Saat putus, server menahan data di dalam fungsi asinkron `ClientHandler.reconnect_timeout()`. Jika klien masuk lagi memanggil `_handle_reconnect()`, sesi tersambung sempurna. |
| **Ping Indicator** | `client/components/ping_display.py` | Komponen merender durasi waktu tempuh dari fungsi `PingDisplay.should_send_ping()` ke `PingDisplay.on_pong_received()`. |
| **Logging Player** | `server/logger.py` | Kelas sentral `GameLogger` melacak fungsi seperti `log_connect()`, `log_liar_call()`, dst. ke dalam file `/logs/`. |
| **Anti-Invalid Packet** | `server/packet_validator.py` | `RateLimiter.check()` membuang request berfrekuensi sangat cepat dari IP usil, sedangkan tipe payload JSON salah digagalkan lewat fungsi `validate_packet()`. |
| **(Bonus) Dedicated Game Server** | `server/main_server.py` | Dieksekusi secara asinkron (head-less) yang sepenuhnya tak bergantung pada UI klien sehingga ideal ditaruh pada server cloud. |

---

## 2. Alasan Pemilihan Protokol TCP

**Protokol Utama:** TCP (Transmission Control Protocol)

Permainan *Liar's Deck* didesain memiliki tipe siklus asinkronous *turn-based*. Hal terpenting dalam komputasinya adalah **Urutan** dan **Keabsahan**.
Apabila program mengimplementasikan UDP (koneksi nirsambung), akan ada risiko mutlak saat Klien melempar kartu, data paket (*packet loss*) tidak sampai di Server, lalu Klien terkunci karena ia berpikir sudah memencet tombol play sementara server tetap diam menanti. TCP mengatasi ini dengan fitur **Guaranteed Delivery** dan antrean paket berurutan (jika 2 tombol ditekan 0.1 detik bergantian, server akan memprosesnya linear). Overhead ping pada TCP tak bermasalah bagi game berbasis *turn-based* di mana reliabilitas komputasi jauh lebih berharga daripada transmisi instan *jitter-free* dari UDP.

---

## 3. Arsitektur Kode: Sisi Client (`client/`)

Keseluruhan folder klien dibedah sebagai berikut. **Setiap** class dan fungsi dicantumkan di bawah tanpa pengecualian.

### Inti Aplikasi Client
**File: `client/config.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `get_custom_font` | Berfungsi merangkul file `.ttf` khusus pada `assets/`. Bila tidak ditemukan, akan gagal anggun (*graceful fallback*) memakai default py-font untuk menghindari *crash* layar. |

**File: `client/game_state.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `GameState` | Class raksasa representasi memori lokal atas status server (berapa kartu nyawa, giliran siapa, tipe kartu tengah). |
| `__init__` | Inisialisasi awal properti lokal dengan variabel *None* atau array kosong. |
| `reset` | Mengosongkan memori variabel untuk skenario klien keluar sesi dan mulai ulang Matchmaking baru. |
| `update_from_server` | Mesin penelan parser; mengambil `dict` state dari paket server, mereplikasi ulang ke variabel internal lokal agar validasi UI bisa bekerja jujur. |
| `is_my_turn` | Helper perbandingan identitas; mengembalikan boolean *True* jika sesi giliran berhak atas `player_id` milik kita. |
| `my_info` | Menelusuri kamus *players array* di dalam kamar, mengembalikan cuplikan data hit-points murni milik pemain lokal semata. |
| `opponents` | Filter negasi; menyaring keluar pengguna lokal dan mengembalikan daftar nama profil serta sisa kartu dari musuh. |
| `opponent_username` | Ekstraksi String khusus untuk format duel 1v1 demi memampangkan "Vs [Nama Musuh]" di palang atas layar. |
| `my_alive` | Peninjauan integer Health Points (HP); me-return *True* jika nyawa masih > 0. Jika mati, panel tombol UI dinonaktifkan permanen. |
| `set_status` | Fungsional pemicu string notifikasi *flash*, memasangkan pesan status teks beserta timer *countdown* pudar di tengah meja. |

**File: `client/main_client.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `LiarsDeckClient` | Class fasad tertinggi pembungkus Pygame Display dan antarmuka transmisi Sockets. |
| `main` | Entitas `__main__` yang memicu instansiasi objek app klien dan memanggil pemblokir run-loop nya. |
| `__init__` | Mengalokasikan kanvas Pygame window, meyiapkan frame-clock, serta men-setup status awal klien ke `STATE_LOGIN`. |
| `run` | Pusaran utama game *while-loop*. Menetapkan FPS di 60, mengekstrak event OS (Quit, alt-f4), mendorong input, serta memanggil render UI. |
| `toggle_fullscreen` | Penukar boolean saklar Display Mode Pygame sehingga game dapat melompat menjadi *Borderless Fullscreen*. |
| `_handle_event` | Menerima Pygame Mouse/Key *Events* dan mendorong aliran logikanya jatuh ke instansi Screen apapun yang tengah berkuasa saat ini (Lobby/Game). |
| `_do_connect` | Menangkap string teks IP Port dari field form input UI, melempar request koneksi TCP sinkron kepada Socket `NetworkClient`. |
| `_process_network` | Iterasi pasif yang membakar isi daftar `poll_packets()`, memanen semua respon Server antrean baru. |
| `_route_packet` | Router Switchboard masif; mencocokan tipe JSON String untuk mengeksekusi transisi logika secara otomatis (seperti memicu *animasi roulette* atau pergantian state ke GameOver). |
| `_update` | Mengirim angka matematis *Delta-Time* sekon dari siklus Loop kepada kelas Screen saat ini agar dapat digunakan memutar kalkulus *kinematic* animasi. |
| `_draw` | Menimpa frame piksel monitor dengan rona warna kosong, membiarkan kelas GUI terkait mencetak lukisannya secara berkesinambungan. |
| `_draw_error_overlay` | Pelukis spanduk *error* transparansi warna merah bawah layar. |
| `_show_error` | Memanggil overlay ke luar dengan argumen string pesan yang bisa berubah dari server. |

**File: `client/network.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `NetworkClient` | Wadah soket transisi ber-threaded yang menjauhkan eksekusi I/O TCP agar tak membekukan frame per detik Pygame klien. |
| `__init__` | Meng-inisiasi *Queue* struktur Deque Thread-safe serta referensi objek `socket`. |
| `is_connected` | Penguji flag eksistensi *socket* untuk memastikan sambungan masih bernapas dan valid. |
| `connect` | Mencoba *blocking bind* ke IP alamat peladen, apabila berhasil segera men-spawn *Thread Daemon* yang membajak kontrol pembacaan RX buffer. |
| `disconnect` | Pemutus anggun soket yang menutup paksa *file descriptor*, mengizinkan thread peladen server merekam kehilangan (*exit_gracefully*). |
| `send_packet` | Menyatukan kamus JSON Python, mengubah enkripsi ke array byte ASCII, diakhiri dengan tag `\n`, memuntahkannya langsung via TCP `sendall()`. |
| `_receive_loop` | Roda gila thread daemon: Mengonsumsi terus-menerus chunk 1024-byte asinkron, memburu indeks \n untuk mencincang payload lengkap menjadi paket-paket rasional (Deserialisasi). |
| `poll_packets` | Pemeras de-queue: menarik dan membersihkan seluruh daftar Inbox array secara atomik bagi *GameLoop* utama. |

### Komponen GUI (`client/components/`)
**File: `client/components/button.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `Button` | Objek visual persegi empat untuk kontrol klik Pygame. |
| `get_button_image` | Pencipta statis yang membuat Cached Bitmap untuk gambar latar belakang Button guna penghematan rendering komputasi RAM. |
| `__init__` | Inisialisasi hit-box (`pygame.Rect`) pada koordinat X Y spesifik layar berdasarkan argumen inisial ukuran teksnya. |
| `_get_font` | Aksesor pemanggil font custom tebal eksklusif tombol teks. |
| `draw` | Menempatkan teks dan panel bitmap, dengan pendeteksi irisan piksel panah kursor mouse (Hover highlight filter). |
| `is_clicked` | Merespon objek *MOUSEBUTTONDOWN* dengan me-return boolean mutlak tatkala koordinat sentuh selaras di dalam rektangulernya. |
| `set_text` | Pemanipulasi label pasca-inisiasi (misal mengganti kata "Play" ke "Wait"). |
| `set_pos` | Pemindah vektor X Y button untuk re-alignment pasca pergantian dimensi layar. |
| `center_x` | Korektor geometri yang secara otomatis meluruskan kordinat rektangular tepat ke ekuator poros X Display Width. |

**File: `client/components/card_sprite.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `CardSprite` | Objek representasi matematis selembar figur kartu remi di jagat grafik. |
| `_get_font` | Akses font tebal untuk mengeja angka nominal kartu pada kanvas. |
| `get_card_image` | Komputer kanvas bitmap pembentuk warna putih tulang dan logo *Suit* (Spades/Hearts/dsb) ke dalam cache global statis sekali buat. |
| `get_card_back_image`| Pembangunan corak vektor kotak-kotak penutup bagian belakang muka kartu yang diakses secara rekursif statis cache. |
| `__init__` | Pendefinisian rank dan bentuk, serta inisiasi y-offset untuk interaksi pop-up. |
| `rect` | Getter turunan yang me-return `pygame.Rect` kartu berdasarkan sumbu offset Y animasinya saat ini. |
| `set_position` | Menerima kordinat awal perancangan *Card-Spread* (penyusunan tangan) dari Parent. |
| `update_y` | Interpolator animatif (delta-time dependent) yang menaikkan kartu piksel demi piksel saat properti *is_hovered* menempel. |
| `draw` | Delegator keputusan render: memeriksa flag *face-down* demi mengarahkan kanvas mana yang harus diproyeksikan, serta mengaktifkan warna penyaring rona Emas jika *is_selected*. |
| `_draw_face_up` | Perintah primitif Pygame *Blit* dengan permukaan grafis angka depan. |
| `_draw_face_down` | Perintah primitif Pygame *Blit* atas pola belakang kartu demi proteksi informasi curang visual. |
| `contains_point` | Pembanding tabrakan titik `x,y` terhadap hit-box `rect()` dari sudut Sprite ini. |

**File: `client/components/center_pile.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `CenterPile` | Simbol dekorator visual pemegang jumlah integer kartu-kartu yang dimainkan tumpukan tengah secara kolektif di server. |
| `__init__` | Penyusunan referensi hitungan awal (N=0). |
| `_get_font` | Inisialisasi font judul papan bertulis "Cards in play". |
| `_get_badge_font` | Inisialisasi font angka khusus untuk penomeran yang merapat pas di dalam bulatan *red badge* tumpukan. |
| `set_position` | Memakukan properti poros geometri elemen tumpukan ini pada area taplak meja maya layar game. |
| `set_count` | Re-alokator memori integer untuk mengganti angka stempel lencana sesuai *Payload JSON* update terbaru. |
| `draw` | Membangun elemen figur kotak kartu pasif berjejer palsu dan mengeksekusi lukisan lencana bulat merah penampang teks *integer count*. |

**File: `client/components/hand_display.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `HandDisplay` | Manajer Koleksi jajaran objek `CardSprite` yang diposisikan seperti lembaran kipas kartu tradisional pemain. |
| `__init__` | Menyesuaikan kerangka memori Array kosong tempat memuat Sprite-sprite kelak. |
| `set_cards` | Membaca list Python (suit, rank) dari luar dan merancang konversi massal menjadi lusinan wujud entitas visual Sprite `CardSprite`. |
| `set_facedown` | Saklar global array yang mengkomando tiap-tiap Sprite individu kartu untuk menutupi permukaan aslinya, kerap dieksekusi bila *owner* mati nyawa habis. |
| `set_position` | Mentransfer batas rektangular induk lokasi penampungan di bawah frame layar. |
| `_layout` | Matematik kalkulator algoritma spasi *fanning*. Meratakan total jarak layar sedia dibagi jumlah kartu, agar *overlap* (tumpuk pinggiran kartu) merapat natural ke tengah jika jumlah kartunya overkapasitas. |
| `draw` | Iterasi lukis `CardSprite.draw()` ke setiap list array anak dari poros Kiri ke Kanan z-index secara hierarki. |
| `handle_click` | Pelacak kursor *hit-test* terbalik. Menganalisa dari lapis paling kanan (Top layer) ke kiri untuk me-nyelidiki bagian ujung kartu siapa yang terklik dan mengubah seleksi tumpul tersebut *True*/*.False*. |
| `get_selected_indices` | Agregator daftar list index (e.g. `[1, 3]`) yang sedang memancarkan status disorot (*selected*), dikemas guna dipasrahkan via Network ke Server. |
| `get_selected_cards` | Pembantu internal merangkum Dictionary `{"suit": "x", "rank": "y"}` spesifik dari identitas Sprite ter-seleksi. |
| `clear_selection` | Prosedur sterilisasi saklar massal yang menjatuhkan paksa semua status *is_selected* semua anggota himpunan Sprite hand. |

**File: `client/components/ping_display.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `PingDisplay` | Objek utilitas stempel tulisan milidetik yang mengekspos angka latensi jaringan *Client-Server Ping* secara transparan ke sisi pemain. |
| `__init__` | Parameterisasi nilai ping statis 0 ms default serta penempatan interval 2 detik di awal waktu untuk letupan paket tes ping perdana. |
| `_get_font` | Kolektor sumber sistem teks khusus layar FPS ping (ukuran kecil tanpa serif). |
| `should_send_ping` | Analis jam lokal `time.time()`. Me-return konfirmasi logikal true kala siklus detik melampaui timer internal jeda sehingga spam PING dapat dihindari. |
| `on_pong_received` | Matematika diferensial waktu lampau dengan jam internal terkini sesaat `S_PONG` diregistrasi untuk dirubah secara absolut menjadi *integer ms*. |
| `draw` | Eksekusi cetak teks angka pada pojok tajam kiri atas dimensi Layar, memberlakukan variasi warna Merah Terang bila limit hitungan ms melambung (indikasi sinyal lelet). |

**File: `client/components/roulette_anim.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `RouletteAnimation` | Kinematika visual tabung revolver penalti yang memberikan efek dramatis suspense kepada pecundang *Liar* sebelum mendelegasikan vonis maut. |
| `__init__` | Inisialisasi limitator frame kecepatan angular (*rotational velocity*), konstanta gaya perlambatan (*friction*), serta penampung angka ruang laras revolver (1-6). |
| `_get_font` | Konseptor pencari font khusus tajuk letusan ("BANG!" / "CLICK!") di layar. |
| `start_spin` | Sakelar pengaktif booelean state dan pemicu putaran kilat angular kecepatan masif putaran drum revolver awal (*wind-up*). |
| `show_result` | Titik simpul konfirmasi vonis dari payload JSON server yang menentukan properti *Survive* atau sebaliknya, sehingga visual akhir tahu string kematian apa yang dicetak ke wajah layar. |
| `is_active` | Pengukur state yang memvalidasi ke app jika sinema grafika ini masi murni berjalan atau pudarannya (fade timer) rampung lenyap. |
| `reset` | Penyapu bersih (*scrubber*) seluruh variabel durasi, visibilitas string vonis dan kecepatan putaran silinder untuk merestorasi penampakan revolver ke setelan pabrik murni. |
| `draw` | Pemilih percabangan rutinitas (*Branch selector*) yang berdasarkan boolean status merutekan perintah gambar apakah harus memproyeksikan animasi silinder sedang bergasing atau silinder pasif berhenti yang dijatuhi vonis teks final. |
| `_draw_spinning` | Komputasi efek kejut inersia. Menurunkan nilai rotasi per deltasekon diiringi perkalian grafika *alpha-blending* ilusi kabur cincin-silinder berputar kencang. |
| `_draw_result` | Papan penayang peluru macet (Selamat) atau memuntahkan Letusan yang mengandalkan variasi kecerahan pelafalan huruf dari kroma Merah/Hijau di pusat revolver pasif. |
| `_draw_cylinder` | Rutin poligonal primitif `pygame.draw.circle`. Melukiskan bentuk top-down laras senapan berlubang enam secara murni komputasi vektor geometrik, tidak butuh gambar asli. |

### Layar GUI Klien (`client/screens/`)
**File: `client/screens/screen_game.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `GameScreen` | *Class Core* yang mencaplok status layar mutlak. Seluruh komponen (Tangan, Meja, Tombol) diinstansiasi ke dalam anggota kepemilikannya. |
| `__init__` | Pendirian objek-objek bawahan. Membakukan 2 bilah Tombol GUI utama ("Play Cards" & "Call Liar") di dalam memori kordinat kanvas khusus. |
| `_get_font` | Instansi lokal ekstraktor teks proporsional. |
| `handle_event` | Tangkapan terstruktur `pygame.MOUSEBUTTONDOWN`. Me-referensikan koordinat *Click* kepada tombol internal. Jika cocok, mentransisikan `action_dict` untuk diubah manajer socket menjadi pesanan server resmi (seperti `{type: C_PLAY_CARDS}`). |
| `update` | Mengadopsi pergeseran detik. Merutekannya untuk merawat animasi *fade-out* dari status melayang, rotasi Roulette, dan slide-kartu tangan secara serentak. |
| `draw` | Perangkai kanvas besar dari lapisan terbawah (*Background*) hingga ke lapisan teratas tumpukan (*Roulette/Status Text*). Ia memanggil *sub-draw* helper berurutan. |
| `_draw_opponents` | Pengekstrasi profil array *opponents*. Melukis deretan ikon musuh beserta badge indikator kartu sisa mereka di deretan atap meja Pygame (Top-Layout). |
| `_draw_table_card` | Penerjemah informasi JSON "target Rank" tabel ke dalam emblem tulisan raksasa bertuliskan jenis pangkat paksaan bulat ini (Misalnya memancing tulisan: Must play 'Aces'). |
| `_draw_last_play` | Pengintip riwayat tumpukan tengah; men-generate tulisan kecil yang menuding *"Fulan Played 2 Jacks"* untuk menjustifikasi keputusan apakah panggil *Liar* adalah tindakan beralasan. |
| `_draw_player_bar` | Desainer visual panel navigasi sudut kanan bawah penampang nyawa avatar (*Life counter*) dan stempel profil nama lokal Klien terkait. |
| `_draw_turn_indicator` | Pemberi sinyal grafis bercahaya berwujud *border neon* yang mengurung ikon musuh tatkala `current_turn_id` diidentifikasi menjadi miliknya untuk navigasi alur perhatian. |
| `_draw_status` | Memproyeksikan tulisan pengumuman fana (sementara) yang bersarang di memori GameState yang meleleh transparansi *alpha*-nya per tick waktu (contoh: "It's your turn!"). |
| `_draw_reveal` | Lukisan dramatis paska-insiden "Call Liar". Memuntahkan wujud figur kartu fisik tumpukan tengah yang aslinya rahasia itu kepada publik untuk dinilai bersama keaslian kebohongannya. |
| `_draw_roulette_overlay` | Pewarna filter peredupan (*dimming rect*) sekujur resolusi monitor disusul oleh trigger peletakan *RouletteAnimation* pas di tengah siluet muka pemain yang tepergok berdusta. |

**File: `client/screens/screen_gameover.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `GameOverScreen` | Intermisi absolut yang memajang papan peringkat kematian sejenak ketika nyawa pemain berstatus tandas (0 HP). |
| `__init__` | Mewarisi sisa *GameState* pasca maut, serta merumuskan tombol dua cabang jalan (Play Again atau Main Menu). |
| `_get_font` | Merampas aset huruf jumbo raksasa pengumuman kemenangan ("YOU LOSE!" / "WINNER!"). |
| `handle_event` | Pendeteksi ketukan di atas blok tombol menu penentu nasib dan memberikan perintah pelarian state keluar layar kepada manajer klien utama. |
| `draw` | Menyiram kanvas dengan warna hitam berkabung kemudian meletakkan tulisan statistik. |
| `_draw_stats` | Pemecah kamus ringkasan *Game Over Data* peladen; mencetak berapa ronde pertahanan dan nominal HP surplus sisa pertempuran pada baris tengah rekapan UI. |
| `_draw_particles` | Estetika debu konfeti fana pelipur lara untuk layar pemenang, direalisasikan komputasi titik sirkel primitif mengapung Pygame. |

**File: `client/screens/screen_lobby.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `LobbyScreen` | Proyeksi visual pasif *waiting state*. Kamar persinggahan selagi soket nganggur karena memori *LobbyManager* server belum rampung merakit pasangan match. |
| `__init__` | Inisiasi UI Tombol sakelar Batal untuk mengirim `C_LEAVE_LOBBY`. |
| `_get_font` | Helper ukuran spesifik font teks "Waiting for match". |
| `handle_event` | Pemeriksa tunggal klik tombol 'Cancel' yang merantai aksi *disconnect* sepihak dari klien. |
| `draw` | Penggambaran teks, memanggil penggambaran Ping antarmuka di sudut, menyelingi *draw_spinner* untuk keindahan visual penantian. |
| `_draw_spinner` | Mesin perhitungan kalkulus sinus-cosinus pergerakan busur berputar lingkaran (Loading Icon Ring) Pygame guna menunjukkan bahwasannya program tidak mengalami *Not Responding*. |

**File: `client/screens/screen_login.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `_InputField` | *Sub-class helper* pengetikan keyboard. Merakam huruf masukan, mengakomodasi kedipan kursor, merender ketebalan rektangular, dan menterjemahkan backspace *unicode* OS lokal. |
| `__init__` (InputField) | Penetapan kotak batas dan placeholder abu-abu jika variabel teks string kosongan. |
| `handle_event` (InputField) | Filter aktif py-key-events; mengambil karakter alfabet atau titik, menolak panjang berlebih (max length limitter). |
| `draw` (InputField) | Konstruktor pigura *rect* putih serta cetakan teks hitam *realtime* di dalam form masukan terkait. |
| `LoginScreen` | Panel masuk sistem. Penghimpun komponen input ganda beserta eksekutor uji koneksi ke IP TCP terdaftar. |
| `__init__` (LoginScreen) | Menyatukan variabel `_InputField` menjadi komponen nama (`username`) dan portal server (`IP Address`). |
| `_get_font` s/d `_get_err_font` | Modul hirarki ukuran estetika teks log masuk berundak, mulai raksasa, sub, dan huruf penunjuk pesan fatal kecil. |
| `_load_title_img` | Pemuat lambang aset judul bitmap jika grafika logo tersedia di arsip klien. |
| `handle_event` (LoginScreen) | Mengalokasikan sinyal papan ketik turun ke dua kolom InputField, atau memicu transisi Enter/Klik tombol *Connect*. |
| `_try_connect` | Penghimpun properti String, mendefinisikan batas port default (`8080`), kemudian memoles struktur Dictionary aksi login menuju `_do_connect` router *main_client*. |
| `_draw_bg_video` | Efek visual fana. Simulator partikel bergerak lembut asimetris belakang form untuk meramaikan monitor. |
| `draw` (LoginScreen) | Konduktor orkestrasi render dari elemen statis judul, input kolom yang di-fokus, tombol sambung dan partikel efek melayang di lapis dasar. |
| `_draw_bg_motifs` | Geometri arsitektur tambahan pola-pola segitiga/garis pasif pembingkai tampilan UI muka Login screen di ujung bidang layar. |

---

## 4. Arsitektur Kode: Sisi Server (`server/`)

### Sistem Konektivitas
**File: `server/main_server.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `LiarsDeckServer` | Super-class peladen akar. Mengorkestrasi pembibitan variabel pengurus (*Lobby, Rooms, Logger, Security Limiter*) lalu memasang ranjau TCP socket listen. |
| `main` | Pemantik inisial dari command-line Python; membuat *instance* dari Server, menangani mitigasi sinyal interupsi keyboard (`SIGINT/Ctrl+C`), serta melakukan boot eksekusi utamanya. |
| `__init__` | Alokasi pencatatan referensi IP:Port konfigurasi, penetapan dictionary penyimpan `SessionTokens` para pelintas batas klien *concurrent*, serta pemegang hak *Mutex-Lock* thread. |
| `start` | Eksekusi krusial *socket.bind* dan *socket.listen*. Loop tak terbatas ini akan mencaplok blok `socket.accept()` per mili-detik guna mencabut satu koneksi klien dari ruang hampa untuk ditugaskan pada benang perulangan independen `ClientHandler`. |
| `start_match` | Reseptor pemicu dari Lobby. Meminta RoomManager merajut ID bilik maya yang baru, mendelegasikan daftar pelancong *(players)* terkait, memutakhirkan sesi ID mereka, dan memberondong mereka dengan paket bahagia JSON `S_MATCH_FOUND`. |
| `shutdown` | Skrip bunuh diri (*kill switch*) gracefully. Jika server dicabut paksa, ia melacak socket peladen global, dan per-koneksi klien untuk ditelpon pemutusannya sehingga tak ada bandwith mengganjal (Zombies socket). |
| `signal_handler` | Interseptor `Ctrl+C` perantara dari OS tingkat rendah; mencegah tumpukan kesalahan (Stack Trace error python murni) demi merutekan pengakhiran langsung ke metode `shutdown`. |

**File: `server/client_handler.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `ClientHandler` | Mesin pekerja asinkron (*Thread object*). Satu objek eksklusif ini di-spawn hanya untuk mengadopsi sinyal detak tunggal satu jaringan *socket connection* pengguna hingga ajal (koneksi terputus). |
| `__init__` | Injektor dependensi referensial ke berbagai dewa peladen (*LobbyManager*, *RoomManager*, *RateLimiter*, *GameLogger*) beserta socket I/O mandiri `Player_ID`. |
| `send_packet` | Penerjemah logikal python *dict* peladen menjadi UTF-8 String, mengimbuhi terminasi koma batas (\n) lalu menyemprotkannya keras melalui socket. `sendall()`. |
| `send_to_opponent` | *Broadcast filter*. Menyemprotkan muatan paket secara selektif ke seantero profil pemain di kamar *Room* kecuali ke pengirim asli (Berguna untuk "Enemy Played a Card" alert). |
| `broadcast_room` | *Absolute Broadcaster*. Mengirim muatan kemasan JSON yang absolut merata menyapu semua penghuni kamar yang bersemayam dalam Match yang sama tanpa diskriminasi. |
| `send_game_state` | Pemantik engine otoritatif keamanan mutlak. Mengeksekusi permintaan rekonstruksi topologi paket status ke GameEngine dengan modifikasi (menyensor kartu teman dan kartu musuh) guna dilemparkan eksklusif via socket pribadi klien ini. |
| `send_your_turn` | Sinyal otoritas. Memberitahu antarmuka UI khusus dari Player_ID terkait bila rantai `current_turn_id` di ruang pertandingan selaras dengan namanya agar menonaktifkan gembok input layar mereka. |
| `run` | Perputaran tak terbatas. Motor penerima Byte-Stream pasif jaringan yang merampas potongan aliran 1024-byte demi potongan, mentransfusinya masuk ke tangki buffer penampung String string. |
| `_process_buffer` | Pisau bedah buffer. Mencari bilah sekat karakter batas *newline* untuk mengoperasi, mencincang serpih *payload* terenkripsi JSON menjadi *Dictionary* rasional yang legal di dalam tata bahasa program lalu mendistribusikannya ke kompartemen perutean (`_route_packet`). |
| `_route_packet` | Pos Pemeriksaan Logika (*The Switchboard*). Analisis if-else memanjang; memungut `type` spesifikasi komando paket klien dan mendelegasikan eksekusinya ke handler fungsional di bawah. Terproteksi limiter laju koneksi DoS. |
| `_handle_connect` | Penyerah gerbang awal sesi jaringan. Mengarsip profil username Klien pendatang, membangkitkan `session_token` rahasia buat mereka, dan menghidangkan respons hangat JSON selamat datang. |
| `_handle_reconnect` | Rutinitas ajaib restorasi sinyal. Merujuk token sakti `session_token` dari permintaan masuk kembali ke sesi *Room* lama guna mengambil alih instansi tubuh karakter di *GameEngine* agar tidak dianulir mati sia-sia. |
| `_handle_join_lobby` | Konfirmasi penyerahan nasib. Merutekan identitas profil pribadi menuju laci daftar pencarian *matchmaking* `LobbyManager` agar mulai mencari jodoh duel. |
| `_handle_leave_lobby` | Pemberhentian taktik pelarian. Mencabut kembali identitas yang sempat bersarang pada antrean `LobbyManager` gara-gara Klien memencet navigasi Cancel dari UI. |
| `_handle_play_cards` | Memproses permintaan interaksi "Menurunkan Kartu". Mengkonfirmasi apakah urutan turn sah di dalam aturan GameEngine, mengeksekusi kartu itu masuk tumpukan, lalu mengabari semesta seisi room. |
| `_handle_call_liar` | Mendengar seruan interupsi "Saya Curiga!". Menunda segala interaksi lain, mendorong GameEngine buat langsung melakukan rutinitas uji balistik integritas (pemeriksaan kartu aslinya) pada turn terdahulu. |
| `_handle_roulette_pull` | Menjalankan instruksi fatal permohonan eksekusi drum putaran Roulette, melempar seruan penalti ke GameEngine untuk melakukan hitungan probabilistik matematis, kemudian mem-broadcast vonis peluru tembus atau nihil. |
| `_handle_ping` | Agen pelayan Echo ringan. Jika klien menangis menanyakan C_PING waktu, handler dengan seketika memuntahkan S_PONG waktu tanpa mendaki tangga GameEngine untuk diagnosa secepat mungkin. |
| `_handle_chat` | Megafon komunikasi komunal. Eksekutor pesan omongan string pendek yang didistribusikan langsung menembus fungsi `broadcast_room` agar dinikmati semua musuh lokal. |
| `_handle_ready` | Pemverifikasi komitmen klien yang mengabarkan "Papan layar kami telah memuat tekstur sepenuhnya". Jika periksa daftar pemain berstatus lengkap tersambung siap, engine langsung meracik distribusi kocokan kartu (*Deal Cards*). |
| `_start_match` | Rutin peralihan nasib. Menganeksasi identitas Room ke handler diri sendiri setelah konfirmasi Matchmaking di `LobbyManager` menyodoknya agar tidak keliru menembak pesan keluar orbit Room nya. |
| `_handle_disconnect` | Modul pesimistis dari hilangnya denyut koneksi `socket.error`. Menonaktifkan profil, melonggarkan utas belenggu buffer, melempar jejak tragedi ke log konsol, lantas melahirkan jam pasir mematikan `reconnect_timeout`. |
| `cleanup` | Pengubur sampah memori absolut. Mengikis hapus kordinat token Player dari pustaka komunal sesi server dan *limiter* keamanaan manakala durasi mati permanen divalidasi habis. |
| `reconnect_timeout` | Pengawas ulir hitung mundur fana. Bersembunyi dalam senyap dan menunda diri 30 detik pasca-diskoneksi sebelum beranjak menjagal pemain (`forfeit()` engine call) karena dipastikan tak akan kembali pulang. |

### Sistem Logika & Aturan
**File: `server/game_engine.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `PlayerState` | Kamus struktur data fundamental *in-memory*. Mengekang identitas profil, kapasitas tumpukan kartu rahasia di tangan, dan deposit ketahanan nyawa 1 pengguna murni. |
| `GameEngine` | Otak yudisial, Sang *Dictator* peraturan permainan mutlak. Tumbuh berpisah sepenuhnya dari segala kaitan urusan transmisi jaringan (Soket-Agnostik), ia hanya menimbang angka, probabilitas kocokan deck, perputaran arah langkah dan penegakan tata tertib klaim kejujuran. |
| `__init__` | Mendaftarkan arketipe kamus wadah untuk pemain teritorialnya, merakit tumpukan kartu sentral (*center_pile*), melempar target rank konstan di atas meja awal, dan membaptis indikator giliran. |
| `create_deck` | Tukang cetak kartu matematis. Me-looping matriks konstanta dari Suit/Rank menjadi array panjang 52 digit utuh, lantas me-*Randomize* indeks urutannya tanpa belas kasihan. |
| `deal_cards` | Sang Pembagi. Mencomot rentetan list deck buatan atas tumpukan teratas dengan limit konstan, mengukirnya ke masing-masing kantong *PlayerState* privat mereka dan membuang jejak ampas sisanya. |
| `choose_table_card` | Agen Pengundi Konteks. Mencaplok jenis rank acak (seperti Angka 8, Jack, atau King) lalu menginstruksikan kepada arena Room bahwa kartu inilah yang harus dibualkan seluruh orang sepanjang ronde ini. |
| `current_turn_id` | Telunjuk penentu kekuasaan. Mengekstraksi pointer identitas eksklusif dari list urutan tempat duduk meja bundar untuk memilah orang yang tak boleh kena pembatasan. |
| `_advance_turn` | Jam weker perpindahan. Mengukur iterasi pointer (+1) ke pemain sirkuler berikutnya, otomatis meloncat gembira melewati subjek-subjek profil manapun yang diklasifikasi sebagai orang mati (HP=0). |
| `play_cards` | Pencatat seruan aksi melempar kartu; Melucuti list kartu itu dari kompartemen tangan si pelaku, meleburnya lenyap tenggelam ke `center_pile` memori, lantas mensahkan status turn dan rekaman bualan ("Saya main 3 Queens") untuk dipersiapkan bila memantik *Liar*. |
| `call_liar` | Perintah sengketa interupsi pengadilan. Mencegat jalannya siklus alur giliran jam, menyambar catatan lemparan korban subjek sebelumnya, lalu memaksa Engine menjadwalkan kalkulus interogasi kebenaran `check_cards_truth()`. |
| `check_cards_truth` | Penguji validitas *Bluff*. Pembedah logika murni. Jika deretan nominal array `cards` yang dibuang barusan sinkron 100% presisi identik nilainya dengan deklarasi kartu meja, Sang Pendakwa dinyatakan salah nuduh, namun bila terdapat walau 1 anomali penyusup, Si Pelempar diklaim *Liar!* |
| `pull_roulette` | Entropi mesin takdir mesin Roulette; menjerat jiwa yang dieliminasi fase sebelumnya, menarik nilai probabilitas silinder acak dan mengalkulasikan defisit satu nyawa pengurangan jika angka yang muncul sejalan laras kematian. |
| `check_round_over` | Detektif pamungkas kondisi meja. Memvalidasi jumlah pemain sisa pasca kekejaman roda Roulette. Jika populasi penyintas mengerucut menyentuh angka 1, layar tirai ditarik menuju status GameOver tamat. Kalau tidak, memacu pembersihan reset. |
| `reset_round` | Sesi petugas kebersihan panggung teater. Menghancurkan `center_pile` ke dasar lautan memori, merampas kartu-kartu di tangan sisa pemain untuk dihanguskan, memerintahkan `create_deck` ulang, mensetel `choose_table_card` baru, agar perputaran maut dilanjutkan dari babak nol. |
| `start_game` | Tuas penyalaan sistem sirkuler; mendestinasi ronde nol kepada angka satu, melarang segala pemain mengaktifkan *Roulette*, membungkam fase reinkarnasi, serta langsung menembakkan peluru `deal_cards()`. |
| `build_state_for_player` | Satpam otoritatif pencegah pembongkaran memori curang (*Anti Wall-hack/Map-hack*). Mesin ini me-rekursi keseluruhan tabel data Room, membikin kloning JSON palsu, mensabotase data tangan musuh dari angka mutlak menjadi wujud stempel string *'hidden'* sebelum dilempar paksa ke handler soket pendengar sang Klien si mata-mata. |
| `get_winner` | Penyisir iterasi logikal pencarian mahkota. Menganalisis kamus `PlayerState`; barangsiapa profil entitasnya memiliki saldo mutlak Integer Nyawa di atas angka mutlak terendah 0 dan menjauhi semua bangkai saudaranya, dialah yang me-return *ID Sang Juara*. |
| `get_loser` | Penyapu daftar mayat. Me-return konklusi daftar malang siapapun yang nyawanya habis tandas, merubah mereka dari sekadar penyintas menjadi subjek tontonan. |
| `set_player_connected` | Tuas boolean penyelamat integritas Turn-Logic. Menyetel variabel *is_connected* agar saat jam berputar melangkahi barisan pemain, dia mengabaikan orang mati internet alih-alih memberinya jatah *Turn* yang merusak dan menjebak pertandingan ke fase *Halted Freeze*. |
| `forfeit` | Pedang eksekusi pasca diskoneksi. Menyabotase mutlak nyawa pelaku yang sinyalnya mati melewati toleransi Timeout sehingga terbunuh resmi menjadi 0, melegalkan penutupan pertandingan bilamana musuhnya di ruang tanding dinyatakan bebas menang instan. |

### Manajer Sistem Lainnya
**File: `server/lobby_manager.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `LobbyManager` | Mesin penampung antrean yang pasif. Memelihara daftar list para pejuang tuna-Match dan bertugas menjerat mereka ke dalam kelompok bersekutu bila ambang persentase kelengkapan tim dirasa optimal memenuhi. |
| `__init__` | Meletakkan struktur penyimpanan Deque hampa dan otomatis melontarkan Background Thread asinkron yang bernapas mendenyut memindai parameter kelayakan match pada `_check_loop`. |
| `add_to_queue` | Mendaftarkan profil koneksi Klien serta me-stempel cap waktu OS di dalam list sebagai acuan *First In First Out (FIFO)* dan tolak ukur penentuan kejenuhan. |
| `_pop_match` | Eskavator data. Mencabut, mengerat, dan melucuti kumpulan n (kapasitas 2-4) profil dari depan belalai antrean panjang untuk secara mandiri diisolasi lalu didelegasikan ke *Room Creator*. |
| `_check_loop` | Siklus detak paru-paru Matchmaking. Perulangan thread abadi yang mencerna logika: "Apabila ada sekumpulan 4 ekor idle, ledakan menjadi Match. Jika ada 2 ekor tapi masa pengabdian antrean merengek melampaui 10 detik, cabut saja dan adukan duel walau tidak penuh". |
| `remove_from_queue` | Sabotase pendaftaran. Mengikis hapus kordinat indeks daftar pemain apabila mereka muak dan menekan cancel ke menu. |
| `queue_size` | Pemancar parameter antrean kasat mata yang menjumlahkan secara absolut *length* daftar deque untuk kebutuhan pengamatan (Debug/UI pasif). |
| `is_in_queue` | Validator duplikasi. Mencegah bug maut apabila pemain bandel men-spam request koneksi pencarian pertandingan sebelum memori terdahulu dihapuskan. |
| `get_wait_time_remaining` | Timer pengintip asinkron. Merujuk ke waktu pendaftaran kepingan entitas paling uzur di antrean lalu menakar seberapa laju lagi sisa sekon komputasi menanti hingga pemicuan Match dipaksa prematur karena kemurahan hati sistem. |
| `stop` | Racun pemutus iterasi. Mengubah bendera saklar Thread background menjadi *False* agar iterasi berhenti berdengung demi mengizinkan server shutdown aman. |

**File: `server/room_manager.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `Room` | Kamus kelas Data murni yang meminggit dan mengekang entitas *GameEngine* privat dan jejeran *Client ID* untuk dijerumuskan ke dalam parameter kotak terisolasi agar tak membocorkan event pertandingan ke bilik sebelah. |
| `RoomManager` | Arsiparis global (Hotel Clerk). Mengelola registrasi ribuan `Room` melalui buku kamus terdistribusi yang direferensi ID UUID. Memberi arahan direktori konektivitas mutlak ke mana handler socket harus membuang sampah JSON mereka. |
| `__init__` | Inisialisasi lumbung memori penyimpan daftar nama-nama Room. |
| `create_room` | Bidan pencipta pertempuran. Memanggil *Generator ID* UUID acak anti-tabrakan, menetaskan instansi segar `GameEngine`, mewariskan profil kelompok ke sana, membungkusnya sebagai objek `Room` dan menyegelnya di kamus *Manager*. |
| `remove_room` | Penggilas ruang. Pembersihan pariwara setelah Match dibubarkan dan semua pihak GameOver agar memori RAM peladen server kembali bening tak tersisa entitas Room berhantu. |
| `get_room` | Mesin pencari UUID mutlak. Mentranskripsikan string ruangan rujukan handler Klien guna mengambil kunci akses memori terhadap `GameEngine` di mana entitas klien itu tengah dihadapkan. |
| `get_room_by_player` | Radar penyisir brutal. Mencari paksa indeks *Player_ID* yang tak memiliki petunjuk kompas string Room nya di tengah lautan kamus, menggerus memori pencarian yang menyelamatkan nyawa ketika koneksi amnesia akibat Disconnect parah. |
| `room_count` | Meteran pengukur beban server dengan menghitung total keseluruhan kamus objek Room yang aktif bernafas. |
| `all_rooms` | Iterator pembuka pintu peninjauan ekstensif untuk sistem laporan (*Server Metrics Logging*). |

**File: `server/packet_validator.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `validate_packet` | Pos Pemeriksaan Biometrik payload. Mengendus kedalaman JSON Klien yang melintas dan menolak keras (return False) mutlak bila tiada format pengenal `type` string atau propertinya amorfik demi melestarikan kedamaian peladen. |
| `RateLimiter` | *Middleware Security* tameng penangkis Flooding. Menyusun matriks pelacak cap-waktu milidetik per Player ID spesifik untuk memastikan mereka bertingkah masuk akal layaknya manusia bernalar, alih-alih bot peretas gila. |
| `__init__` | Mendefinisikan toleransi peladen yakni `requests_per_second` untuk diizinkan masuk dari suatu celah IP tunggal dan membongkar kamus ingatan. |
| `check` | Meteran denyut. Menyandingkan cap waktu `time.time()` terkini yang mengalir masuk terhadap pergerakan milidetik lampau dari Player. Bila terdeteksi durasi kurang dari batas kuota pecahan detiknya, *False* diserukan, payload JSON dikutuk ke neraka kehampaan. |
| `remove_player` | Operasi pembersihan sanitasi RAM. Meniadakan ID Klien dari kamus pengintaian pembatasan laju setelah si empunya memutuskan angkat kaki diskoneksi. |

**File: `server/logger.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `GameLogger` | Arsitek pembukuan sentral asinkron (Print to File and Console). Memberlakukan stempel OS Time untuk menderetkan rentetan kisah pilu/kejayaan server pada subdirektori file ekstensi `.log`. |
| `__init__` | Pekerja mandor yang merintis subdirektori `/logs` mandiri bila tiada di jagat direktori, memahat file batu teks berinkripsi nama waktu kalender murni. |
| `log_connect` | Notaris pengikat ikrar soket yang mewartakan lahirnya entitas koneksi alamat IP dan penganugerahan status *Player_ID* barunya. |
| `log_disconnect` | Mencatat detik hembusan putusnya seutas nyawa TCP secara jujur (Normal Quit, Force Close, error soket). |
| `log_play_cards` | Menganalisis log tindakan dan meringkas bualan nominal kartu yang digelontorkan di atas pusat meja pertarungan agar terpantau. |
| `log_liar_call` | Mewartakan drama pengadilan saat jari telunjuk pemain memutuskan membantai musuhnya dalam intrik seruan tantangan kemunafikan. |
| `log_reveal` | Mempublikasi aib. Mengumumkan secara rinci konklusi hasil visum mesin GameEngine mengenai apakah klaim pelempar kartu terjustifikasi suci atau laknat. |
| `log_roulette` | Menjabarkan klimaks *Russian Roulette* dan memproklamirkan apakah pemicu berbunyi cetik selamat, atau derik peluru kematian. |
| `log_game_over` | Mengakhiri tinta lembaran pertempuran spesifik bilik Match dengan meresmikan mahkota kemenangan ID Sang Juara. |
| `log_invalid_packet` | Pembunyian sirine merah (Peringatan terminal) mengutuk kehadiran JSON selundupan cacat struktur dari klien yang menderita halusinasi programatik atau injeksi *Hacker*. |
| `log_info` | Pemutar fungsi cetak dasar *stdout* rona putih penanda kelancaran arus informasi netral di atas monitor server-side. |
| `log_error` | Penyemprot *stderr* string kemerahan bila peladen tersandung komplikasi fatal (soket hancur dsb). |
| `log_debug` | Senter pengintip arus bawah program pasif di lingkungan pengembangan (*Dev Environment*). |

---

## 5. Arsitektur Kode: Shared Utilities (`shared/`)

Kode konstan perantara (Simbiosis). Beroperasi membakukan hukum bahasa yang mesti disumpah setia baik dari klien maupun host.

**File: `shared/constants.py`**
*(Hanya Variabel Konstanta)*
| Referensi / Enumerasi | Penjelasan Eksklusif |
|----------------------|----------------------|
| Variabel Konstanta (e.g. `PHASE_GAME_OVER`) | Buku kitab *Hardcode*. Menyeragamkan label-label status (Fase permainan, tipe Remi Hearts, Batas Silinder peluru 6). Melumpuhkan potensi maut akibat anomali `Typo-Mismatch` pada rutinitas penulisan komparasi statemen if-else String Klien dan Server. |

**File: `shared/packet_types.py`**
*(Hanya Variabel Konstanta)*
| Referensi / Enumerasi | Penjelasan Eksklusif |
|----------------------|----------------------|
| `C_CONNECT`, `S_MATCH_FOUND`, dll. | Glosarium rute mutlak dari sistem Routing JSON. Sinyal bertajuk `C_` dirumuskan sebagai instruksi absolut rute (Client To Server), sedangkan tajuk `S_` menahbiskan maklumat wajib kepatuhan pengumuman (Server To Client). |

**File: `shared/utils.py`**
| Class / Fungsi | Penjelasan Eksklusif |
|----------------|----------------------|
| `serialize` | Penempa baja *Pure Function*. Memaksa array kamus objek dict Python murni untuk mengompres dirinya menjadi wujud string kompak tak berspasi, dikonversi utuh jadi balok Bytes UTF-8 demi peluncuran selamat ke meriam gorong TCP Socket. |
| `deserialize` | Kunci dekripsi transmisi mutlak. Merampas bongkahan byte basah dari lautan kabel aliran Socket, memurnikan string Unicode-nya, lantas mencoba mengurai balik ke entitas Dict murni. Membanting pengecualian `None` bila ampas yang masuk hancur. |
| `generate_id` | Pesulap algoritma acak UUIDv4 (Universally Unique Identifier) yang menggali *os.urandom* demi membangun sidik jari string identitas pengenal tunggal ruang, membasmi momok tabrakan parameter nama ruangan server. |
| `generate_session_token` | Pandai besi Hexadesimal. Mewariskan rantai String *Cryptography-Secure* 32 karakter untuk dipeluk permanen per Klien. Satu-satunya mantra pelindung jaminan kemurnian hak klaim akses rekoneksi kepemilikan bila socket tertebas angin. |
