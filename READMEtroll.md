# Drama Troll

Dalam sebuah perusahaan penuh drama fandom, seorang karyawan bernama DainTontas telah menyinggung komunitas Gensh _ n, Z _ Z, dan Wut \* ering secara bersamaan. Akibatnya, dua rekan kerjanya, SunnyBolt dan Ryeku, merancang sebuah troll dengan gaya khas: membuat filesystem jebakan menggunakan FUSE.

Mereka membutuhkan bantuanmu, ahli Sistem Operasi, untuk menciptakan filesystem kustom yang bisa mengecoh DainTontas, langsung dari terminal yang dia cintai.

---
## Full Code
```c
#define FUSE_USE_VERSION 26

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
#define _XOPEN_SOURCE 700
#endif

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <pwd.h>
#include <stdbool.h>

#define DEBUG 1
#if DEBUG
#define debug_print(fmt, ...) fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
#define debug_print(fmt, ...)
#endif

static bool trap_triggered = false;
const char *trigger_file = "/tmp/troll_trigger";

static bool is_daintontas() {
    struct fuse_context *ctx = fuse_get_context();
    struct passwd *pw = getpwuid(ctx->uid);
    return pw && strcmp(pw->pw_name, "DainTontas") == 0;
}

static void loadtrap() {
    FILE *f = fopen(trigger_file, "r");
    trap_triggered = false;
    if (f) {
        int status = 0;
        if (fscanf(f, "%d", &status) == 1) {
            trap_triggered = (status != 0);
        }
        fclose(f);
    }
}

static void savetrap() {
    FILE *f = fopen(trigger_file, "w");
    if (f) {
        fprintf(f, "%d", trap_triggered);
        fclose(f);
    }
}

static const char *troll_art =
"     ______     ____   ____              _ __                       _                                           __\n"
"    / ____/__  / / /  / __/___  _____   (_) /_   ____ _____ _____ _(_)___     ________ _      ______ __________/ /\n"
"   / /_  / _ \\/ / /  / /_/ __ \\/ ___/  / / __/  / __ `/ __ `/ __ `/ / __ \\   / ___/ _ \\ | /| / / __ `/ ___/ __  / \n"
"  / __/ /  __/ / /  / __/ /_/ / /     / / /_   / /_/ / /_/ / /_/ / / / / /  / /  /  __/ |/ |/ / /_/ / /  / /_/ /  \n"
" /_/    \\___/_/_/  /_/  \\____/_/     /_/\\__/   \\__,_/\\__, /\\__,_/_/_/ /_/  /_/   \\___/|__/|__/\\__,_/_/   \\__,_/  \n"
"                                                    /____/                                                        \n";

static int xmp_getattr(const char *path, struct stat *stbuf) {
    debug_print("getattr path = %s", path);

    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0777;
        stbuf->st_nlink = 2;
    } else if (strcmp(path, "/very_spicy_info.txt") == 0 || 
               strcmp(path, "/upload.txt") == 0 ) {
        stbuf->st_mode = S_IFREG | 0666;
        stbuf->st_nlink = 1;
        stbuf->st_size = 1024;
    } else {
        return -ENOENT;
    }

    stbuf->st_uid = geteuid();
    stbuf->st_gid = getegid();
    return 0;
}

static int xmp_access(const char *path, int mask) {
    debug_print("access: %s, mask: %d", path, mask);
    if (strcmp(path, "/upload.txt") == 0) return 0;
    int res = access(path, mask);
    return res == -1 ? -errno : 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi) {
    debug_print("readdir: %s", path);
    (void) offset;
    (void) fi;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, "very_spicy_info.txt", NULL, 0);
    filler(buf, "upload.txt", NULL, 0);
    return 0;
}

static int xmp_unlink(const char *path) {
    debug_print("unlink :%s", path);
    if (strcmp(path, "/upload.txt") == 0 && is_daintontas()) {
        unlink("/tmp/troll_trigger");
        return 0; 
    }
    return -EPERM;
}

static int xmp_truncate(const char *path, off_t size) {
    debug_print("truncate: %s", path);
    if (strcmp(path, "/upload.txt") == 0 && is_daintontas()) {
        return 0;
    }
    return -EPERM;
}

static int xmp_open(const char *path, struct fuse_file_info *fi) {
    debug_print("open: %s", path);
    if (strcmp(path, "/very_spicy_info.txt") != 0 &&
        strcmp(path, "/upload.txt") != 0 &&
        strcmp(path, "/") != 0)
        return -ENOENT;
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi) {
    debug_print("read: %s, size: %zu, offset: %lld", path, size, (long long)offset);
    loadtrap();

    if (trap_triggered && strstr(path, ".txt") && is_daintontas()) {
        size_t len = strlen(troll_art);
        if (offset >= len) return 0;
        if (offset + size > len) size = len - offset;
        memcpy(buf, troll_art + offset, size);
        return size;
    }

    if (strcmp(path, "/very_spicy_info.txt") == 0) {
        const char *content = is_daintontas()
            ? "Very spicy internal developer information: leaked roadmap.docx\n"
            : "DainTontas' personal secret!!.txt\n";

        size_t len = strlen(content);
        if (offset >= len) return 0;
        if (offset + size > len) size = len - offset;
        memcpy(buf, content + offset, size);
        return size;
    }

    if (strcmp(path, "/upload.txt") == 0) {
        return 0;
    }

    return -ENOENT;
}

static int xmp_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi) {
    debug_print("write: %s, size=%zu", path, size);
    if (strcmp(path, "/upload.txt") == 0 && is_daintontas()) {
        if (strstr(buf, "upload")) {
            trap_triggered = true;
            savetrap();
        }
        return size;
    }
    return -EACCES;
}

static int xmp_release(const char *path, struct fuse_file_info *fi) {
    debug_print("release: %s", path);
    return 0;
}

static int xmp_statfs(const char *path, struct statvfs *stbuf) {
    debug_print("statfs: %s", path);
    return 0;
}

static int xmp_fsync(const char *path, int isdatasync,
                     struct fuse_file_info *fi) {
    debug_print("fsync: %s", path);
    return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr    = xmp_getattr,
    .access     = xmp_access,
    .readdir    = xmp_readdir,
    .unlink     = xmp_unlink,
    .truncate   = xmp_truncate,
    .open       = xmp_open,
    .read       = xmp_read,
    .write      = xmp_write,
    .release    = xmp_release,
    .statfs     = xmp_statfs,
    .fsync      = xmp_fsync,
};

int main(int argc, char *argv[]) {
    debug_print("Starting Drama Troll Filesystem");

    FILE *f = fopen(trigger_file, "a+");
    if (f) {
        chmod(trigger_file, 0666);
        fclose(f);
    }
    loadtrap();

    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
```
---

## **a. Pembuatan User**

> **Soal:**  
> Buat 3 user di sistem sebagai berikut yang merepresentasikan aktor-aktor yang terlibat dalam _trolling_ kali ini, yaitu: DainTontas, SunnyBolt, Ryeku

**Cara Mengerjakan:**
Gunakan `useradd` dan `passwd` untuk membuat akun lokal. User ini akan digunakan untuk mengakses filesystem FUSE yang kamu buat.
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

**Screenshoot**
![Screenshot from 2025-06-22 17-34-56](https://github.com/user-attachments/assets/7d0452ba-2b93-40fa-837d-0d62342b00ed)

---

## **b. Jebakan Troll**

> **Soal:**  
> Untuk menjebak DainTontas, kamu harus menciptakan sebuah filesystem FUSE yang dipasang di `/mnt/troll`. Di dalamnya, hanya akan ada dua file yang tampak sederhana:
> - `very_spicy_info.txt` - umpan utama.
> - `upload.txt` - tempat DainTontas akan "secara tidak sadar" memicu jebakan.


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

**Screenshoot**

![Screenshot from 2025-06-22 17-45-56](https://github.com/user-attachments/assets/e30ca363-be60-43ea-9d98-8a9b3450782a)

---

## **c. Jebakan Troll (Berlanjut)**

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
  - Jika melakukan `cat` pada `/very_spicy_info.txt`:
    - Jika user adalah `DainTontas`, isi file:  
      `Very spicy internal developer information: leaked roadmap.docx`
    - Jika bukan, isi file:  
      `DainTontas' personal secret!!.txt`

---

**Screenshot**
![Screenshot from 2025-06-22 17-49-07](https://github.com/user-attachments/assets/6a96e437-40ac-487c-ad46-8c24bede0fd4)

---

## **d. Trap**

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

**Screenshoot**

![Screenshot from 2025-06-22 17-52-09](https://github.com/user-attachments/assets/a4fb8b3e-ccc6-47fb-a801-0e4728c12265)

---

## **Behavior Lain**

- **`xmp_unlink` & `xmp_truncate`**  
  Hanya mengizinkan DainTontas menghapus atau truncate file `upload.txt`.
- **`xmp_open`, `xmp_access`**  
  Menjamin hanya file yang diizinkan bisa diakses.

---

## Tambahan Untuk Mengembalikan file `.txt`

> **Soal Tersirat:**  
> Setelah seharian penuh mencoba untuk membetulkan PC dia, dia baru sadar bahwa file `very_spicy_info.txt` yang dia sebarkan melalui script uploadnya (`upload.txt`) tersebut teryata berisikan aib-aib dia. Dia pun ditegur oleh SunnyBolt dan Ryeku, dan akhirnya dia pun tobat.

---

### **Fungsi yang Berperan**

- **`xmp_unlink`**

### **Penjelasan Implementasi**

```c
static int xmp_unlink(const char *path)
```

- Saat DainTontas menjalankan:  
  ```bash
  rm /mnt/troll/upload.txt
  ```
  maka FUSE akan memanggil `xmp_unlink`.

#### **Logika di Dalam Fungsi:**

1. **Cek Path dan User:**  
   `if (strcmp(path, "/upload.txt") == 0 && is_daintontas())`  
   Hanya jika file yang dihapus adalah `/upload.txt` **dan** user adalah `DainTontas`.
   
2. **Menghapus Trap Persistent:**  
   `unlink("/tmp/troll_trigger");`  
   File trigger (`/tmp/troll_trigger`)** yang menandai status aktifnya jebakan akan dihapus.  
   Dengan menghapus file ini, jebakan **reset** (kembali ke kondisi sebelum DainTontas menulis "upload" ke `upload.txt`).

---

### **Efek di FUSE**

- Jika file `/tmp/troll_trigger` dihapus, pada akses berikutnya ke file `.txt`:
  - Fungsi `loadtrap()` akan membaca status trap menjadi tidak aktif.
  - Semua file `.txt` akan kembali ke perilaku semula (bukan ASCII art untuk DainTontas).

---
**Screenshoot**

![Screenshot from 2025-06-22 17-54-01](https://github.com/user-attachments/assets/57b50a4f-9a7c-4b71-9a0f-6818a4b2c501)

---
