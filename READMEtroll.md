# Drama Troll Filesystem (FUSE) — Penjelasan Implementasi

Repositori ini berisi implementasi filesystem FUSE jebakan yang dirancang untuk kasus "Drama Troll". Berikut penjelasan bagaimana setiap point soal dikerjakan, fungsi mana yang berperan, dan bagaimana mekanismenya:

---

## **a. Pembuatan User**

> **Soal:**  
> Buat 3 user: DainTontas, SunnyBolt, Ryeku menggunakan `useradd` dan `passwd`.

**Cara Mengerjakan:**
- Jalankan di terminal (sebagai root/sudo):
  ```bash
  sudo useradd DainTontas
  sudo passwd DainTontas
  sudo useradd SunnyBolt
  sudo passwd SunnyBolt
  sudo useradd Ryeku
  sudo passwd Ryeku
  ```
- User ini akan digunakan untuk login dan menguji filesystem FUSE yang dibuat.

---

## **b. Jebakan Troll (Filesystem FUSE dengan Dua File)**

> **Soal:**  
> Buat filesystem FUSE di `/mnt/troll` berisi hanya dua file:  
> - `very_spicy_info.txt`  
> - `upload.txt`  

**Fungsi yang Berperan:**
- `xmp_readdir`
- `xmp_getattr`

### Penjelasan Fungsi:
- **`xmp_readdir`**  
  Fungsi ini akan dipanggil saat directory `/mnt/troll` dibuka (misal dengan `ls`).  
  ```c
  static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
  ```
  - Mengecek jika `path` adalah root (`/`), lalu mengisi directory dengan:
    - `.`, `..`, `very_spicy_info.txt`, dan `upload.txt`
  - Ini memastikan hanya dua file yang terlihat di dalam mount point.

- **`xmp_getattr`**  
  Fungsi ini menangani permintaan atribut file (seperti stat), sehingga file yang disebutkan di atas tampak nyata.
  ```c
  static int xmp_getattr(const char *path, struct stat *stbuf)
  ```
  - Jika `path` adalah `/`, mengembalikan atribut directory.
  - Jika `/very_spicy_info.txt` atau `/upload.txt`, mengembalikan atribut file reguler.

---

## **c. Jebakan Troll (Isi File Berbeda untuk User Tertentu)**

> **Soal:**  
> Jika file `very_spicy_info.txt` dibuka oleh DainTontas, isinya:  
> `Very spicy internal developer information: leaked roadmap.docx`  
> Jika dibuka oleh user lain:  
> `DainTontas' personal secret!!.txt`

**Fungsi yang Berperan:**
- `xmp_read`
- `is_daintontas`

### Penjelasan Fungsi:
- **`is_daintontas`**  
  Fungsi helper untuk mengecek apakah user yang melakukan operasi adalah `DainTontas`.
  ```c
  static bool is_daintontas() {
      struct fuse_context *ctx = fuse_get_context();
      struct passwd *pw = getpwuid(ctx->uid);
      return pw && strcmp(pw->pw_name, "DainTontas") == 0;
  }
  ```
  - Mengambil context FUSE dan uid, lalu membandingkan dengan nama user.

- **`xmp_read`**  
  Fungsi ini menangani operasi baca file.
  ```c
  static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
  ```
  - Jika membaca `/very_spicy_info.txt`:
    - Jika user adalah `DainTontas`, isi file:  
      `Very spicy internal developer information: leaked roadmap.docx`
    - Jika bukan, isi file:  
      `DainTontas' personal secret!!.txt`

---

## **d. Trap — Trigger Perubahan Behavior via upload.txt**

> **Soal:**  
> Jika DainTontas melakukan `echo "upload" > upload.txt`, maka semua file `.txt` akan menampilkan ASCII art "Fell for it again reward" untuk DainTontas. Trigger ini harus persist walau FUSE di-unmount/restart.

**Fungsi yang Berperan:**
- `xmp_write`
- `savetrap`, `loadtrap`
- `xmp_read`
- Variabel global: `trap_triggered` dan file `/tmp/troll_trigger`
- ASCII art: `troll_art`

### Penjelasan Fungsi:
- **`xmp_write`**  
  Digunakan untuk mendeteksi jika `upload.txt` ditulis oleh DainTontas dengan string `"upload"`.
  ```c
  static int xmp_write(const char *path, const char *buf, size_t size,
                       off_t offset, struct fuse_file_info *fi)
  ```
  - Jika file `/upload.txt` dan user adalah DainTontas, dan ada string "upload" yang ditulis:
    - Set `trap_triggered = true`
    - Panggil `savetrap()` untuk menyimpan status ke file `/tmp/troll_trigger` (persisten).

- **`savetrap`, `loadtrap`**  
  Menyimpan/membaca status jebakan ke/dari file `/tmp/troll_trigger`.
  - `savetrap()`: Menulis status ke file.
  - `loadtrap()`: Membaca status dari file.
  - File ini akan tetap ada walau FUSE di-unmount atau mesin direstart, memastikan trigger persist.

- **`xmp_read`**  
  Saat membaca file `.txt`, fungsi akan:
    - Memanggil `loadtrap()`
    - Jika `trap_triggered == true` DAN user adalah DainTontas DAN file berekstensi `.txt`:
      - Mengembalikan **ASCII art** `troll_art` sebagai isi file apapun yang berekstensi `.txt`.

- **Variabel dan ASCII Art:**
  - Variabel global `trap_triggered` menandai apakah jebakan sudah aktif.
  - `troll_art` berisi ASCII art “Fell for it again reward”.

---

## **Behavior Lain**

- **`xmp_unlink` & `xmp_truncate`**  
  Hanya mengizinkan DainTontas menghapus atau truncate file `upload.txt`.
- **`xmp_open`, `xmp_access`**  
  Menjamin hanya file yang diizinkan bisa diakses.

---

## **Ringkasan Alur**

1. **User Creation**:  
   Tiga user dibuat agar bisa menguji perbedaan perilaku filesystem.

2. **Mount FUSE**:  
   Compile kode, lalu mount:
   ```bash
   gcc -Wall trollfs.c `pkg-config fuse --cflags --libs` -o trollfs
   sudo ./trollfs /mnt/troll
   ```

3. **Isi Directory**:  
   Hanya ada dua file: `very_spicy_info.txt` dan `upload.txt`.

4. **Isi File Dinamis**:  
   - Jika file `very_spicy_info.txt` dibuka:
     - DainTontas: “leaked roadmap”
     - User lain: “DainTontas' personal secret!!.txt”

5. **Trigger Jebakan**:  
   - Jika DainTontas menulis `"upload"` ke `upload.txt`, semua file `.txt` di dalam FUSE akan menampilkan ASCII art pada setiap akses oleh DainTontas.
   - Status trigger persist menggunakan file `/tmp/troll_trigger`.

---

## **Kesimpulan**

Setiap behavior di soal di-handle oleh fungsi di kode C ini, dengan mekanisme deteksi user, trigger jebakan, dan isi file dinamis tergantung status trigger. FUSE memungkinkan filesystem custom seperti ini dengan logika yang fleksibel.

---

### **Referensi Fungsi Kunci**

| Fungsi         | Soal/Behavior yang Di-handle                           |
|----------------|--------------------------------------------------------|
| xmp_readdir    | Menampilkan dua file di root mount                     |
| xmp_getattr    | Menampilkan atribut file dan directory                 |
| xmp_read       | Menampilkan isi file sesuai user & status trap         |
| is_daintontas  | Mengecek user yang mengakses                           |
| xmp_write      | Memicu trap jika DainTontas menulis "upload"           |
| savetrap/loadtrap | Membuat trigger persistent di file `/tmp/troll_trigger` |
| troll_art      | ASCII art jebakan                                      |

---
