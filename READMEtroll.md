Berikut adalah file `README.md` berisi penjelasan lengkap dan detail dari kode FUSE C yang kamu unggah. Ini termasuk cuplikan kode serta penjelasan tiap bagiannya agar mudah dipahami.

---

# FUSE Troll Filesystem

Sistem berkas ini adalah implementasi berbasis FUSE (`Filesystem in Userspace`) yang melakukan aksi “troll” terhadap user tertentu (`DainTontas`) saat menulis ke file `upload.txt`. Ketika troll terpicu, sistem menampilkan ASCII art dan mengubah perilaku sistem file secara permanen dengan mencatat status ke file trigger `/tmp/troll_trigger`.

## 📁 File: troll\_fs.c

### 🔧 Dependensi dan Konfigurasi Awal

```c
#define FUSE_USE_VERSION 26

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
#define _XOPEN_SOURCE 700
#endif
```

* Menentukan versi API FUSE yang digunakan.
* Mengaktifkan header opsional jika tersedia (`config.h`).
* Mendefinisikan makro untuk kompatibilitas POSIX (`_XOPEN_SOURCE 700`).

### 📚 Header Penting

```c
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
```

* Header ini digunakan untuk operasi file, direktori, akses user, dan logging.

### 🐛 Debug Mode

```c
#define DEBUG 1
#if DEBUG
#define debug_print(fmt, ...) fprintf(stderr, "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
#define debug_print(fmt, ...)
#endif
```

* Makro `debug_print` akan aktif jika `DEBUG` diset ke 1, mencetak log ke `stderr`.

---

## 💥 Trigger Troll dan Pemeriksaan User

```c
static bool trap_triggered = false;
const char *trigger_file = "/tmp/troll_trigger";
```

* `trap_triggered` menandakan apakah troll telah aktif.
* Status disimpan di file trigger agar permanen di antara mount/unmount.

### 👤 Pemeriksaan User

```c
static bool is_daintontas() {
    struct fuse_context *ctx = fuse_get_context();
    struct passwd *pw = getpwuid(ctx->uid);
    return pw && strcmp(pw->pw_name, "DainTontas") == 0;
}
```

* Mengecek apakah user aktif adalah `DainTontas`.

### 💾 Load dan Simpan Status Troll

```c
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
```

* `loadtrap()` memuat status troll dari file.
* `savetrap()` menyimpan status troll setelah dipicu.

---

## 🎨 ASCII Art Troll

```c
static const char *troll_art = "     ______     ____   ____ ...";
```

* Disimpan sebagai string besar, ditampilkan saat troll terpicu.

---

## 🔍 Operasi Filesystem FUSE

### 📂 `getattr` — Atribut File

```c
static int xmp_getattr(const char *path, struct stat *stbuf) {
    ...
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
    ...
}
```

* Menentukan atribut untuk root (`/`), `upload.txt`, dan `very_spicy_info.txt`.
* File lainnya dianggap tidak ada.

### 🔒 `access` — Izin Akses

```c
static int xmp_access(const char *path, int mask) {
    if (strcmp(path, "/upload.txt") == 0) return 0;
    int res = access(path, mask);
    return res == -1 ? -errno : 0;
}
```

* Mengizinkan akses penuh ke `upload.txt`.

### 📁 `readdir` — Isi Direktori

```c
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi) {
    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, "upload.txt", NULL, 0);
    filler(buf, "very_spicy_info.txt", NULL, 0);

    return 0;
}
```

* Menentukan isi direktori root: dua file virtual.

---

## ✍️ `write` — Memicu Troll

Dalam bagian kode lanjutan (tidak tampil semua), biasanya akan ada:

```c
if (strcmp(path, "/upload.txt") == 0 && is_daintontas()) {
    if (strstr(buf, "upload")) {
        trap_triggered = true;
        savetrap();
        write(STDOUT_FILENO, troll_art, strlen(troll_art));
    }
}
```

* Jika `DainTontas` menulis kata "upload" ke `upload.txt`, maka:

  * Troll aktif.
  * ASCII art muncul.
  * Status troll disimpan.

---

## 🚀 Kompilasi dan Jalankan

### Kompilasi

```bash
gcc troll_fs.c -o troll_fs `pkg-config fuse --cflags --libs`
```

### Jalankan

```bash
mkdir /mnt/troll
./troll_fs -f -o allow_other /mnt/troll
```

---

## 🧪 Testing

```bash
cd /mnt/troll
echo "upload" > upload.txt  # jika user adalah DainTontas → troll aktif
cat very_spicy_info.txt     # hasilnya bisa berubah jika troll aktif
```

---

## 🔐 Catatan

* Troll ini hanya aktif untuk user `DainTontas`.
* ASCII art tidak akan muncul untuk user lain.
* Status efek troll bersifat **persisten** via file `/tmp/troll_trigger`.

---

Jika kamu ingin saya tambahkan bagian `write`, `open`, `read`, dan lainnya yang belum muncul di atas, silakan beri tahu — saya akan lanjutkan dan lengkapi isi `README.md` ini.
