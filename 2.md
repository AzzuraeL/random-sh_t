# LawakFS++ - A Cursed Filesystem with Censorship and Strict Access Policies

Teja adalah seorang penggemar sepak bola yang sangat bersemangat. Namun, akhir-akhir ini, tim kesayangannya selalu tampil kurang memuaskan di setiap pertandingan. Kekalahan demi kekalahan membuat Teja muak dan kesal. "Tim lawak!" begitu umpatnya setiap kali timnya gagal meraih kemenangan. Kekecewaan Teja yang mendalam ini menginspirasi sebuah ide: bagaimana jika ada sebuah filesystem yang bisa menyensor hal-hal "lawak" di dunia ini?

Untuk mengatasi hal tersebut, kami membuat filesystem terkutuk bernama **LawakFS++** yang mengimplementasikan kebijakan akses yang ketat, filtering konten dinamis, dan kontrol akses berbasis waktu untuk file tertentu. Filesystem ini dirancang sebagai read-only dan akan menerapkan perilaku khusus untuk akses file, termasuk logging dan manajemen konfigurasi.

- Kamu boleh memilih direktori sumber dan mount point apa pun untuk filesystem kamu.

- Kamu **wajib** mengimplementasikan setidaknya fungsi-fungsi berikut dalam struct `fuse_operations` kamu:

  - `getattr`
  - `readdir`
  - `read`
  - `open`
  - `access`

- Kamu diperbolehkan menyertakan fungsi tambahan seperti `init`, `destroy`, atau `readlink` jika diperlukan untuk implementasi kamu.

- **LawakFS++ harus benar-benar read-only.** Setiap percobaan untuk melakukan operasi tulis dalam FUSE mountpoint harus **gagal** dan mengembalikan error `EROFS` (Read-Only File System).

- System call berikut, dan perintah yang bergantung padanya, harus diblokir secara eksplisit:

  - `write()`
  - `truncate()`
  - `create()`
  - `unlink()`
  - `mkdir()`
  - `rmdir()`
  - `rename()`

> **Catatan:** Ketika pengguna mencoba menggunakan perintah seperti `touch`, `rm`, `mv`, atau perintah lain yang melakukan operasi tulis, mereka harus menerima error "Permission denied" atau "Read-only file system" yang jelas.

### a. Ekstensi File Tersembunyi

Setelah beberapa hari menggunakan filesystem biasa, Teja menyadari bahwa ekstensi file selalu membuat orang-orang bisa mengetahui jenis file dengan mudah. "Ini terlalu mudah ditebak!" pikirnya. Dia ingin membuat sistem yang lebih misterius, di mana orang harus benar-benar membuka file untuk mengetahui isinya.

Semua file yang ditampilkan dalam FUSE mountpoint harus **ekstensinya disembunyikan**.

- **Contoh:** Jika file asli adalah `document.pdf`, perintah `ls` di dalam direktori FUSE hanya menampilkan `document`.
- **Perilaku:** Meskipun ekstensi disembunyikan, mengakses file (misalnya, `cat /mnt/your_mountpoint/document`) harus dipetakan dengan benar ke path dan nama aslinya (misalnya, `source_dir/document.pdf`).

### b. Akses Berbasis Waktu untuk File Secret

Suatu hari, Teja menemukan koleksi foto-foto memalukan dari masa SMA-nya yang tersimpan dalam folder bernama "secret". Dia tidak ingin orang lain bisa mengakses file-file tersebut kapan saja, terutama saat dia sedang tidur atau tidak ada di rumah. "File rahasia hanya boleh dibuka saat jam kerja!" putusnya dengan tegas.

File yang nama dasarnya adalah **`secret`** (misalnya, `secret.txt`, `secret.zip`) hanya dapat diakses **antara pukul 08:00 (8 pagi) dan 18:00 (6 sore) waktu sistem**.

- **Pembatasan:** Di luar rentang waktu yang ditentukan, setiap percobaan untuk membuka, membaca, atau bahkan melakukan list file `secret` harus menghasilkan error `ENOENT` (No such file or directory).
- **Petunjuk:** Kamu perlu mengimplementasikan kontrol akses berbasis waktu ini dalam operasi FUSE `access()` dan/atau `getattr()` kamu.

### c. Filtering Konten Dinamis

Kekesalan Teja terhadap hal-hal "lawak" semakin memuncak ketika dia membaca artikel online yang penuh dengan kata-kata yang membuatnya kesal. Tidak hanya itu, gambar-gambar yang dia lihat juga sering kali tidak sesuai dengan ekspektasinya. "Semua konten yang masuk ke sistem saya harus difilter dulu!" serunya sambil mengepalkan tangan.

Ketika sebuah file dibuka dan dibaca, isinya harus **secara dinamis difilter atau diubah** berdasarkan tipe file yang terdeteksi:

| Tipe File      | Perlakuan                                                                                 |
| :------------- | :---------------------------------------------------------------------------------------- |
| **File Teks**  | Semua kata yang dianggap lawak (case-insensitive) harus diganti dengan kata `"lawak"`.    |
| **File Biner** | Konten biner mentah harus ditampilkan dalam **encoding Base64** alih-alih bentuk aslinya. |

> **Catatan:** Daftar "kata-kata lawak" untuk filtering file teks akan didefinisikan secara eksternal, seperti yang ditentukan dalam persyaratan **e. Konfigurasi**.

### d. Logging Akses

Sebagai seorang yang paranoid, Teja merasa perlu untuk mencatat setiap aktivitas yang terjadi di filesystemnya. "Siapa tahu ada yang mencoba mengakses file-file penting saya tanpa izin," gumamnya sambil menyiapkan sistem logging. Dia ingin setiap gerakan tercatat dengan detail, lengkap dengan waktu dan identitas pelakunya.

Semua operasi akses file yang dilakukan dalam LawakFS++ harus **dicatat** ke file yang terletak di **`/var/log/lawakfs.log`**.

Setiap entri log harus mematuhi format berikut:

```
[YYYY-MM-DD HH:MM:SS] [UID] [ACTION] [PATH]
```

Di mana:

- **`YYYY-MM-DD HH:MM:SS`**: Timestamp operasi.
- **`UID`**: User ID pengguna yang melakukan aksi.
- **`ACTION`**: Jenis operasi FUSE (misalnya, `READ`, `ACCESS`, `GETATTR`, `OPEN`, `READDIR`).
- **`PATH`**: Path ke file atau direktori dalam FUSE mountpoint (misalnya, `/secret`, `/images/photo.jpg`).

> **Persyaratan:** Kamu **hanya diwajibkan** untuk mencatat operasi `read` dan `access` yang berhasil. Logging operasi lain (misalnya, write yang gagal) bersifat opsional.

### e. Konfigurasi

Setelah menggunakan filesystemnya beberapa minggu, Teja menyadari bahwa kebutuhannya berubah-ubah. Kadang dia ingin menambah kata-kata baru ke daftar filter, kadang dia ingin mengubah jam akses file secret, atau bahkan mengubah nama file secret itu sendiri. "Saya tidak mau repot-repot kompilasi ulang setiap kali ingin mengubah pengaturan!" keluhnya. Akhirnya dia memutuskan untuk membuat sistem konfigurasi eksternal yang fleksibel.

Untuk memastikan fleksibilitas, parameter-parameter berikut **tidak boleh di-hardcode** dalam source code `lawak.c` kamu. Sebaliknya, mereka harus dapat dikonfigurasi melalui file konfigurasi eksternal (misalnya, `lawak.conf`):

- **Nama file dasar** dari file 'secret' (misalnya, `secret`).
- **Waktu mulai** untuk mengakses file 'secret'.
- **Waktu berakhir** untuk mengakses file 'secret'.
- **Daftar kata-kata yang dipisahkan koma** yang akan difilter dari file teks.

**Contoh konten `lawak.conf`:**

```
FILTER_WORDS=ducati,ferrari,mu,chelsea,prx,onic,sisop
SECRET_FILE_BASENAME=secret
ACCESS_START=08:00
ACCESS_END=18:00
```

FUSE kamu harus membaca dan mem-parse file konfigurasi ini saat inisialisasi.

### Ringkasan Perilaku yang Diharapkan

Untuk memastikan kejelasan, berikut adalah tabel konsolidasi perilaku yang diharapkan untuk skenario tertentu:

| Skenario                                                              | Perilaku yang Diharapkan                                                                         |
| :-------------------------------------------------------------------- | :----------------------------------------------------------------------------------------------- |
| Mengakses file di luar waktu yang diizinkan (misalnya, file `secret`) | Mengembalikan `ENOENT` (No such file or directory)                                               |
| Membaca file biner                                                    | Konten harus dioutput dalam **encoding Base64**                                                  |
| Membaca file teks                                                     | Kata-kata yang difilter harus diganti dengan `"lawak"`                                           |
| Melakukan list file di direktori mana pun                                  | Semua ekstensi file harus disembunyikan                                                          |
| Mencoba menulis, membuat, atau mengganti nama file/direktori          | Mengembalikan `EROFS` (Read-Only File System)                                                    |
| Logging operasi file                                                  | Entri baru harus ditambahkan ke `/var/log/lawakfs.log` untuk setiap operasi `read` dan `access`. |

### Contoh Perilaku

```bash
$ ls /mnt/lawak/
secret   image   readme

$ cat /mnt/lawak/secret
cat: /mnt/lawak/secret: No such file or directory
# (Output ini diharapkan jika diakses di luar 08:00-18:00)

$ cat /mnt/lawak/image
<string base64 dari konten gambar>

$ cat /mnt/lawak/readme
"Ini adalah filesystem lawak."
# (Kata "sisop" asli diganti dengan "lawak")

$ sudo tail /var/log/lawakfs.log
[2025-06-10 14:01:22] [1000] [READ] /readme
[2025-06-10 14:01:24] [1000] [ACCESS] /secret
```

---

# LawakFS++ - A Cursed Filesystem with Censorship and Strict Access Policies

Teja is a very passionate football fan. However, lately, his favorite team has been consistently underperforming in every match. Loss after loss has left Teja fed up and annoyed. "What a joke team (tim lawak)!" he'd curse every time his team failed to secure a win. Teja's deep disappointment sparked an idea: what if there was a filesystem that could censor all the "lawak" things in the world?

To address this, we are creating a cursed filesystem called **LawakFS++** that implements strict access policies, dynamic content filtering, and time-based access control for certain files. This filesystem is designed to be read-only and will enforce specific behaviors for file access, including logging and configuration management.

- You may choose any source directory and mount point for your filesystem.

- You are **required** to implement at least the following functions within your `fuse_operations` struct:

  - `getattr`
  - `readdir`
  - `read`
  - `open`
  - `access`

- You are permitted to include additional functions like `init`, `destroy`, or `readlink` if they are necessary for your implementation.

- **LawakFS++ must be strictly read-only.** Any attempt to perform write-related operations within the FUSE mountpoint must **fail** and return the `EROFS` (Read-Only File System) error.

- The following system calls, and thus commands that rely on them, must be explicitly blocked:

  - `write()`
  - `truncate()`
  - `create()`
  - `unlink()`
  - `mkdir()`
  - `rmdir()`
  - `rename()`

> **Note:** When users attempt to use commands such as `touch`, `rm`, `mv`, or any other command that performs a write operation, they should receive a clear "Permission denied" or "Read-only file system" error.

### a. Hidden File Extensions

After using regular filesystems for days, Teja realized that file extensions always made it too easy for people to identify file types. "This is too predictable!" he thought. He wanted to create a more mysterious system where people would have to actually open files to discover their contents.

All files displayed within the FUSE mountpoint must have their **extensions hidden**.

- **Example:** If the original file is `document.pdf`, an `ls` command inside the FUSE directory should only show `document`.
- **Behavior:** Despite the hidden extension, accessing a file (e.g., `cat /mnt/your_mountpoint/document`) must correctly map to its original path and name (e.g., `source_dir/document.pdf`).

### b. Time-Based Access for Secret Files

One day, Teja discovered his collection of embarrassing high school photos stored in a folder named "secret". He didn't want others to access these files at any time, especially when he was sleeping or away from home. "Secret files should only be accessible during working hours!" he decided firmly.

Files whose base name is **`secret`** (e.g., `secret.txt`, `secret.zip`) can only be accessed **between 08:00 (8 AM) and 18:00 (6 PM) system time**.

- **Restriction:** Outside this specified time range, any attempt to open, read, or even list the `secret` file must result in an `ENOENT` (No such file or directory) error.
- **Hint:** You will need to implement this time-based access control within your `access()` and/or `getattr()` FUSE operations.

### c. Dynamic Content Filtering

Teja's frustration with "lawak" things reached its peak when he read online articles full of words that annoyed him. Not only that, but the images he viewed often didn't meet his expectations either. "All content entering my system must be filtered first!" he exclaimed while clenching his fist.

When a file is opened and read, its content must be **dynamically filtered or transformed** based on its perceived type:

| File Type        | Behavior                                                                                      |
| :--------------- | :-------------------------------------------------------------------------------------------- |
| **Text Files**   | All words considered lawak (case-insensitive) must be replaced with the word `"lawak"`.       |
| **Binary Files** | The raw binary content must be presented in **Base64 encoding** instead of its original form. |

> **Note:** The list of "lawak words" for text file filtering will be defined externally, as specified in requirement **e. Configuration**.

### d. Access Logging

As a paranoid individual, Teja felt the need to record every activity happening in his filesystem. "Who knows if someone tries to access my important files without permission," he muttered while setting up the logging system. He wanted every movement recorded in detail, complete with timestamps and the perpetrator's identity.

All file access operations performed within LawakFS++ must be **logged** to a file located at **`/var/log/lawakfs.log`**.

Each log entry must strictly adhere to the following format:

```
[YYYY-MM-DD HH:MM:SS] [UID] [ACTION] [PATH]
```

Where:

- **`YYYY-MM-DD HH:MM:SS`**: The timestamp of the operation.
- **`UID`**: The User ID of the user performing the action.
- **`ACTION`**: The type of FUSE operation (e.g., `READ`, `ACCESS`, `GETATTR`, `OPEN`, `READDIR`).
- **`PATH`**: The logical path to the file or directory within the FUSE mountpoint (e.g., `/secret`, `/images/photo.jpg`).

> **Requirement:** You are **only required** to log successful `read` and `access` operations. Logging of other operations (e.g., failed writes) is optional.

### e. Configuration

After using his filesystem for several weeks, Teja realized that his needs kept changing. Sometimes he wanted to add new words to the filter list, sometimes he wanted to change the secret file access hours, or even change the name of the secret file itself. "I don't want the hassle of recompiling every time I want to change settings!" he complained. Finally, he decided to create a flexible external configuration system.

To ensure flexibility, the following parameters **must not be hardcoded** within your `lawak.c` source code. Instead, they must be configurable via an external configuration file (e.g., `lawak.conf`):

- The **base filename** of the 'secret' file (e.g., `secret`).
- The **start time** for accessing the 'secret' file.
- The **end time** for accessing the 'secret' file.
- The comma-separated **list of words to be filtered** from text files.

**Example `lawak.conf` content:**

```
FILTER_WORDS=ducati,ferrari,mu,chelsea,prx,onic,sisop
SECRET_FILE_BASENAME=secret
ACCESS_START=08:00
ACCESS_END=18:00
```

Your FUSE should read and parse this configuration file upon initialization.

### Summary of Expected Behaviors

To ensure clarity, here's a consolidated table of the expected behavior for specific scenarios:

| Scenario                                                    | Expected Behavior                                                                           |
| :---------------------------------------------------------- | :------------------------------------------------------------------------------------------ |
| Accessing a file outside allowed time (e.g., `secret` file) | Return `ENOENT` (No such file or directory)                                                 |
| Reading a binary file                                       | Content must be outputted in **Base64 encoding**                                            |
| Reading a text file                                         | Filtered words must be replaced with `"lawak"`                                              |
| Listing files in any directory                              | All file extensions must be hidden                                                          |
| Attempting to write, create, or rename any file/directory   | Return `EROFS` (Read-Only File System)                                                      |
| Logging of file operations                                  | A new entry must be added to `/var/log/lawakfs.log` for each `read` and `access` operation. |

### Example Behavior

```bash
$ ls /mnt/lawak/
secret   image   readme

$ cat /mnt/lawak/secret
cat: /mnt/lawak/secret: No such file or directory
# (This output is expected if accessed outside 08:00-18:00)

$ cat /mnt/lawak/image
<base64 string of image content>

$ cat /mnt/lawak/readme
"This is a lawak filesystem."
# (Original "sisop" word was replaced with "lawak")

$ sudo tail /var/log/lawakfs.log
[2025-06-10 14:01:22] [1000] [READ] /readme
[2025-06-10 14:01:24] [1000] [ACCESS] /secret
```
