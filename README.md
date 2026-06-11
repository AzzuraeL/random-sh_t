# Kumpulan Praktikum Teori Bahasa dan Automata

![Python](https://img.shields.io/badge/Python-3.x-blue?logo=python&logoColor=white)
![GUI](https://img.shields.io/badge/GUI-Tkinter-orange)
![Status](https://img.shields.io/badge/Status-Completed-brightgreen)

Repositori ini berisi empat program antarmuka grafis (GUI) berbasis Python yang mengimplementasikan konsep-konsep dasar dalam **Teori Bahasa dan Automata**, mulai dari manipulasi string dasar, analisis leksikal, hingga simulasi Finite State Machine (FSM) dan Pushdown Automaton (PDA).

> Semua program dibangun menggunakan library bawaan Python, yaitu `tkinter` untuk antarmuka pengguna grafis (GUI) dan `re` untuk pemrosesan *Regular Expression*. **Tidak diperlukan instalasi library tambahan.**

---

## Struktur Proyek

```
PRAKTIKUMOTOMATA/
├── palindrom.py      # Pengenal String Palindrome
├── praktikum1.py     # Lexical Analyzer Bahasa C
├── praktikum2.py     # FSM Automata Validator
├── praktikum3.py     # Simulator Mesin PDA
└── README.md         # Dokumentasi proyek
```

---

## Daftar Isi

1. [Praktikum Dasar: Pengenal String Palindrome](#-1-praktikum-dasar-pengenal-string-palindrome)
2. [Praktikum 1: Lexical Analyzer Bahasa C](#-2-praktikum-1-lexical-analyzer-bahasa-c)
3. [Praktikum 2: FSM Automata Validator](#-3-praktikum-2-fsm-automata-validator)
4. [Praktikum 3: Simulator Mesin PDA](#-4-praktikum-3-simulator-mesin-pda)
5. [Persyaratan Sistem](#-persyaratan-sistem)
6. [Cara Menjalankan Program](#-cara-menjalankan-program)
7. [Lisensi](#-lisensi)

---

## 1. Praktikum Dasar: Pengenal String Palindrome

**File:** [`palindrom.py`](palindrom.py)

Program ini adalah aplikasi sederhana untuk mendeteksi apakah sebuah string yang dimasukkan oleh pengguna merupakan *palindrome* (kata atau frasa yang dibaca sama baik dari depan maupun dari belakang), dengan mengabaikan spasi dan karakter non-alfanumerik.

### Kode

| Komponen | Deskripsi |
|----------|-----------|
| **Import Library** | Menggunakan `tkinter` untuk GUI dan `messagebox` untuk peringatan input tidak valid. |
| **`check_palindrome()`** | Membersihkan input (`lower()` + `isalnum()`), memvalidasi apakah kosong, lalu membandingkan string dengan versi terbaliknya (`[::-1]`). |
| **Blok GUI** | Menginisialisasi window utama, mengatur ukuran (`450x250`), dan menyusun widget `Label`, `Entry`, dan `Button`. |

### Contoh Penggunaan

| Input | Output | Keterangan |
|-------|--------|------------|
| `katak` | Palindrome | Dibaca sama dari depan & belakang |
| `A man a plan a canal Panama` | Palindrome | Mengabaikan spasi & huruf besar/kecil |
| `hello` | Bukan Palindrome | Tidak simetris |

---

## 2. Praktikum 1: Lexical Analyzer Bahasa C

**File:** [`praktikum1.py`](praktikum1.py)

Program ini merupakan **Lexical Analyzer** (Scanner) sederhana untuk kode sumber berbahasa C. Program akan membaca input berupa blok kode C dan memecahnya menjadi token-token, lalu mengelompokkannya ke dalam empat kategori:

- **Reserve words** — kata kunci bawaan bahasa C
- **Simbol dan tanda baca** — `{`, `}`, `(`, `)`, `;`, dll.
- **Variabel** — identifier yang bukan keyword
- **Kalimat matematika** — fungsi, operator, dan angka

### Kode

| Komponen | Deskripsi |
|----------|-----------|
| **Import & Class** | Menggunakan `re` untuk Regex. Dibungkus dalam class `LexicalAnalyzerApp`. |
| **`tokenize_c()`** | Mendefinisikan 32+ C keywords dan pola Regex (Komentar, Fungsi, Angka, Word, Operator, Simbol, String). Menggunakan `re.finditer` untuk scanning. |
| **Categorization** | Token dikelompokkan: keyword → Reserve words, identifier → Variabel, fungsi → Kalimat matematika, dll. |
| **`display_results()`** | Memformat hasil ke widget `Text` output. |

### Contoh Penggunaan

**Input:**
```c
int main() {
    int x = 10;
    printf("Hello");
    return 0;
}
```

**Output:**
```
a. Reserve words
 -> int, printf, return

b. Simbol dan tanda baca
 -> (, ), ,, ;, {, }

c. Variabel
 -> x

d. Kalimat matematika (persamaan, fungsi, dsb)
 -> 0, 10, =, main()
```

---

## 3. Praktikum 2: FSM Automata Validator

**File:** [`praktikum2.py`](praktikum2.py)

Program ini mensimulasikan **Finite State Machine (FSM)** yang dirancang untuk mengevaluasi apakah sebuah string biner diterima atau ditolak berdasarkan aturan berikut:

| Aturan | Deskripsi |
|--------|-----------|
| Alfabet | Hanya boleh berisi `0` dan `1` |
| Akhiran | Harus diakhiri dengan `1` |
| Substring | Tidak boleh mengandung substring `00` |

L = { x ∈ (0 + 1)+ | dengan karakter terakhir pada string x adalah 1 dan x tidak memiliki substring 00 }

### Diagram State FSM

<img width="406" height="187" alt="image" src="https://github.com/user-attachments/assets/0757e0c2-5f4c-4274-9838-0b6a435caf83" />


### Kode

| Komponen | Deskripsi |
|----------|-----------|
| **Class `FSM`** | Mendefinisikan State Awal (`S`), Final State (`B`), dan tabel transisi 4 state. State `C` adalah *dead/trap state* (mendeteksi `00`). |
| **`evaluate()`** | Membaca karakter satu per satu, mengubah state berdasarkan transisi. Di akhir: state `B` = Accepted, selainnya = Rejected + alasan. |
| **Class `FSMApp`** | GUI dengan `StringVar` untuk input dinamis dan binding `<Return>` untuk evaluasi via keyboard. |

### Contoh Penggunaan

| Input | Status | Alasan |
|-------|--------|--------|
| `1` | VALID | Berakhir di state B |
| `101` | VALID | Tidak ada `00`, berakhir di `1` |
| `100` | INVALID | Mengandung substring `00` |
| `10` | INVALID | Tidak berakhir dengan `1` |
| `abc` | INVALID | Karakter tidak valid |

---

## 4. Praktikum 3: Simulator Mesin PDA

**File:** [`praktikum3.py`](praktikum3.py)

Aplikasi ini adalah simulator **Pushdown Automaton (PDA)** yang memvalidasi bahasa formal:

$$L = \{a^n b^n \mid n \ge 0\}$$

Artinya, program memastikan jumlah karakter `a` sama persis dengan jumlah karakter `b`, dan semua `a` harus muncul sebelum semua `b`.

### Kode

| Komponen | Deskripsi |
|----------|-----------|
| **Class `PDA`** | Menggunakan `list` Python sebagai Stack (`self.stack = []`). |
| **State `q0`** | Membaca awalan: `a` → Push `'A'` ke stack, `b` → Pop dari stack & pindah ke `q1`. |
| **State `q1`** | Mencocokkan sisa: hanya menerima `b` yang memicu Pop. Stack kosong sebelum string habis → `False`. |
| **Acceptance** | String diterima jika seluruh karakter terbaca **dan** stack kosong (`len(self.stack) == 0`). |

### Contoh Penggunaan

| Input | Status | Keterangan |
|-------|--------|------------|
| *(kosong)* | ACCEPTED | n = 0 valid |
| `ab` | ACCEPTED | a¹b¹ |
| `aabb` | ACCEPTED | a²b² |
| `aaabbb` | ACCEPTED | a³b³ |
| `aab` | REJECTED | Jumlah `a` ≠ jumlah `b` |
| `ba` | REJECTED | Urutan salah |
| `abab` | REJECTED | `a` tidak mendahului semua `b` |

---

## Persyaratan Sistem

| Komponen | Versi |
|----------|-------|
| **Python** | 3.x atau lebih baru |
| **Tkinter** | Sudah terinstal default pada distribusi Python standar |
| **OS** | Windows / macOS / Linux |

> [!NOTE]
> Tidak memerlukan instalasi package tambahan (`pip install` tidak diperlukan).

---

## Cara Menjalankan Program

1. Pastikan Python 3.x sudah terinstal di sistem Anda.
2. Buka terminal (Command Prompt / PowerShell / Bash) pada direktori proyek.
3. Jalankan salah satu perintah berikut:

```bash
# Praktikum Dasar - Palindrome Checker
python palindrom.py

# Praktikum 1 - Lexical Analyzer
python praktikum1.py

# Praktikum 2 - FSM Validator
python praktikum2.py

# Praktikum 3 - PDA Simulator
python praktikum3.py
```
