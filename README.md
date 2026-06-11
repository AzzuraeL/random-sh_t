# Kumpulan Praktikum Teori Bahasa dan Automata

Repositori ini berisi empat program antarmuka grafis (GUI) berbasis Python yang mengimplementasikan konsep-konsep dasar dalam Teori Bahasa dan Automata, mulai dari manipulasi string dasar, analisis leksikal, hingga simulasi Finite State Machine (FSM) dan Pushdown Automaton (PDA).

Semua program dibangun menggunakan library bawaan Python, yaitu `tkinter` untuk antarmuka pengguna grafis (GUI) dan `re` untuk pemrosesan *Regular Expression*.

---

## Daftar Isi
1. [Praktikum Dasar: Pengenal String Palindrome (`palindrom.py`)](#1-praktikum-dasar-pengenal-string-palindrome-palindrompy)
2. [Praktikum 1: Lexical Analyzer Bahasa C (`praktikum1.py`)](#2-praktikum-1-lexical-analyzer-bahasa-c-praktikum1py)
3. [Praktikum 2: FSM Automata Validator (`praktikum2.py`)](#3-praktikum-2-fsm-automata-validator-praktikum2py)
4. [Praktikum 3: Simulator Mesin PDA (`praktikum3.py`)](#4-praktikum-3-simulator-mesin-pda-praktikum3py)

---

## 1. Praktikum Dasar: Pengenal String Palindrome (`palindrom.py`)

Program ini adalah aplikasi sederhana untuk mendeteksi apakah sebuah string yang dimasukkan oleh pengguna merupakan *palindrome* (kata atau frasa yang dibaca sama baik dari depan maupun dari belakang), dengan mengabaikan spasi dan karakter non-alfanumerik.

### Bedah Kode
* **Import Library:**
    Menggunakan `tkinter` untuk membangun GUI dan `messagebox` untuk menampilkan jendela peringatan jika input tidak valid.
* **Fungsi `check_palindrome()`:**
    * **Pembersihan Data:** Mengambil teks dari input pengguna, mengubahnya menjadi huruf kecil (`lower()`), dan menghapus semua karakter selain huruf dan angka (`isalnum()`).
    * **Validasi:** Jika input kosong setelah dibersihkan, program memunculkan peringatan (warning).
    * **Pengecekan Logika:** Membandingkan string yang sudah dibersihkan dengan versi terbaliknya (`cleaned_text[::-1]`). Hasilnya (Palindrome / Bukan) ditampilkan dengan perubahan warna teks pada label.
* **Blok GUI (Antarmuka):**
    Menginisialisasi *window* utama (`root`), mengatur ukuran (`geometry`), dan merangkai widget seperti `Label` (judul & instruksi), `Entry` (kolom input teks), dan `Button` (tombol eksekusi yang memanggil `check_palindrome`).

---

## 2. Praktikum 1: Lexical Analyzer Bahasa C (`praktikum1.py`)

Program ini merupakan **Lexical Analyzer** (Scanner) sederhana untuk kode sumber berbahasa C. Program akan membaca input berupa blok kode C dan memecahnya menjadi token-token, lalu mengelompokkannya ke dalam empat kategori: *Reserve words*, Simbol/tanda baca, Variabel, dan Kalimat matematika (fungsi/operator/angka).

### Bedah Kode
* **Import Library & Inisialisasi Class:**
    Menggunakan `re` untuk *Regular Expression*. Program dibungkus dalam *class* `LexicalAnalyzerApp` untuk modularitas dan manajemen status GUI.
* **Metode `tokenize_c(self, code)` (Core Logic):**
    * **Definisi Keywords & Regex:** Menyimpan daftar kata kunci bahasa C dalam struktur data `set`. Mendefinisikan `token_specification` yang berisi pola Regex untuk Komentar, Fungsi, Angka, Kata (Word), Operator, Simbol, dan String.
    * **Proses *Scanning*:** Menggabungkan semua pola Regex dan menggunakan `re.finditer` untuk menelusuri teks.
    * **Pengelompokan (Categorization):** Melalui perulangan, token yang cocok diseleksi:
        * `FUNGSI` atau `WORD` yang ada di dalam *keywords* masuk ke grup "Reserve words". Jika tidak, masuk ke grup "Variabel" atau "Kalimat matematika" (jika berupa fungsi).
        * `SYMBOL` dikelompokkan ke "Simbol dan tanda baca".
        * `NUMBER` dan `OPERATOR` dikelompokkan ke "Kalimat matematika".
* **Metode `display_results(self, tokens)`:**
    Memformat dan mencetak isi struktur data hasil (dictionary) ke dalam widget `Text` output pada GUI.
* **Metode GUI (Clear & Analyze):** Menghubungkan tombol di antarmuka dengan fungsi logika utama.

---

## 3. Praktikum 2: FSM Automata Validator (`praktikum2.py`)

Program ini mensimulasikan **Finite State Machine (FSM)** yang dirancang untuk mengevaluasi apakah sebuah string yang terdiri dari karakter '0' dan '1' diterima atau ditolak berdasarkan aturan spesifik:
* Hanya boleh berisi `0` dan `1`.
* Harus diakhiri dengan `1`.
* Tidak boleh mengandung *substring* `00`.

### Bedah Kode
* **Class `FSM` (Logika Automata):**
    * `__init__`: Mendefinisikan konfigurasi FSM yang terdiri dari State Awal (`S`), Final State (`B`), dan tabel transisi antar state (`S`, `A`, `B`, `C`). State `C` bertindak sebagai *dead state* atau *trap state* (ketika mendeteksi '00').
    * `evaluate`: Membaca karakter satu per satu. Jika input bukan '0' atau '1', langsung ditolak. Mengubah *current state* berdasarkan *transitions rule*. Di akhir iterasi, jika state berada di `B`, maka string diterima (*Accepted*). Jika tidak, ditolak beserta alasan penolakannya.
* **Class `FSMApp` (Antarmuka):**
    Membungkus GUI menggunakan `tkinter`. Menggunakan `StringVar` agar input dapat dibaca secara dinamis, serta melakukan *binding* tombol `<Return>` (Enter) pada keyboard agar pengguna dapat mengevaluasi tanpa harus mengklik tombol. Hasilnya (VALID/INVALID) ditampilkan secara instan.

---

## 4. Praktikum 3: Simulator Mesin PDA (`praktikum3.py`)

Aplikasi ini adalah simulator **Pushdown Automaton (PDA)**. Program ini memvalidasi bahasa formal matematika sederhana, yaitu $L = \{a^n b^n \mid n \ge 0\}$. Artinya, program memastikan jumlah karakter `a` sama persis dengan jumlah karakter `b`, dan semua karakter `a` harus muncul mendahului karakter `b` (contoh: `ab`, `aabb`, `aaabbb`).

### Bedah Kode
* **Class `PDA` (Logika Automata Berbasis Stack):**
    * Mendefinisikan memori *Stack* menggunakan struktur data `list` Python (`self.stack = []`).
    * `process_string`: Iterasi setiap karakter dari input dengan dua buah State (`q0` dan `q1`).
        * **State `q0`:** Digunakan untuk membaca awalan. Setiap mendapat input `a`, program melakukan *Push* (menambahkan karakter 'A' ke dalam tumpukan). Jika mendapat input `b`, program langsung melakukan *Pop* (mengeluarkan isi tumpukan) dan pindah ke state `q1`.
        * **State `q1`:** Digunakan untuk mencocokkan sisa string. Hanya menerima input `b` yang memicu aksi *Pop*. Jika tumpukan sudah kosong sebelum string habis, atau ada karakter selain `a` dan `b`, langsung kembalikan `False`.
    * Di akhir iterasi, string diterima (*Accepted*) jika seluruh karakter sudah terbaca dan tumpukan (*Stack*) benar-benar kosong (`len(self.stack) == 0`).
* **Class `PDAApp` (Antarmuka):**
    Menyediakan GUI berbasis `tkinter` yang menginstansiasi objek `PDA`. Mengambil input pengguna lewat widget `Entry`, memasukkannya ke fungsi evaluasi, dan mengubah label di bawahnya menjadi warna hijau jika statusnya **ACCEPTED**, atau merah jika **REJECTED**.

---

## Persyaratan Sistem (Requirements)
* **Python 3.x** atau lebih baru.
* Modul `tkinter` (biasanya sudah terinstal secara default pada distribusi Python standar).

## Cara Menjalankan Program
Buka terminal (Command Prompt / PowerShell / Bash) pada direktori tempat file Python disimpan, lalu jalankan perintah berikut untuk masing-masing program:

```bash
python palindrom.py
python praktikum1.py
python praktikum2.py
python praktikum3.py
