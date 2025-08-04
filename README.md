# Bab 2: Pengenalan bahasa Golang

# 2.1 Go modules

## 2.1.1 Pengertian

Go modules merupakan tools untuk manajemen dependensi resmi milik Go. Modules digunakan untuk menginisialisasi sebuah project, sekaligus melakukan manajemen terhadap 3rd party atau library atau dependency yang digunakan dalam project. Modules penggunaannya adalah via CLI. Jika pembaca sudah sukses meng-install Go, maka otomatis bisa menggunakan operasi CLI Go Modules. Apa yang dimaksud dengan **Modules ini sebenarnya adalah sebuah project.**

## 2.1.2 Inisialisasi Module/Project Dengan Go Module

Command **“go mod init”** digunakan untuk menginisialisasi project baru. Mari langsung praktekan saja. Buat folder baru, bisa via CLI (cmd, Powershell, Terminal, dll)

```
mkdir first_module
cd first_module
go mod init first_module
```
![Enter image alt description](Images/9WU_Image_1.png)

Eksekusi perintah **go mod init** menghasilkan satu buah file baru bernama **go.mod**. File ini digunakan oleh Go toolchain untuk menandai bahwa folder di mana file tersebut berada adalah folder project. Jadi pastikan untuk tidak menghapus file tersebut.

![Enter image alt description](Images/ILu_Image_2.png)

# 2.2 Go Command

Pengembangan aplikasi Go pastinya tak akan jauh dari hal-hal yang berbau CLI atau *Command Line Interface*. Di Go, proses inisialisasi project, kompilasi, testing, eksekusi program, semuanya dilakukan lewat command line.

## 2.2.1 go mod

**1. go mod init**

Command go mod init digunakan untuk inisialisasi project pada Go yang menggunakan Go Modules. Untuk nama project bisa menggunakan apapun, tapi umumnya disamakan dengan nama direktori/folder.Nama project ini penting karena nantinya berpengaruh pada import path sub packages yang ada dalam project tersebut.

```
mkdir <project-name>
cd <project-name>
go mod init <project-name>
```
**2. go mod download**

Command go mod download digunakan untuk men-download dependency.

**3. go mod tidy**

*Command* go mod tidy digunakan untuk memvalidasi dependency sekaligus men-download-nya jika memang belum ter-download.

**4. go mod vendor**

Command ini digunakan untuk vendoring

## 2.2.2 go run

*Command* go run digunakan untuk eksekusi file program, yaitu file yang ber-ekstensi .go. Cara penggunaannya dengan menuliskan *command* tersebut diikuti argumen nama file. Berikut adalah contoh penerapan go run untuk eksekusi file program main.go yang tersimpan di path first_module yang path tersebut sudah diinisialisasi menggunakan go mod init.

![Enter image alt description](Images/oSe_Image_3.png)

*Command* go run hanya bisa digunakan pada file yang nama package-nya adalah main contohnya seperti program berikut.

```go
package main
import "fmt"
func main() {
    fmt.Println("Hello, World!")
}
```
## 2.2.3 go get

*Command* go get digunakan untuk men-download package atau *dependency*. Sebagai contoh, penulis ingin men-download package Kafka driver untuk Go pada project first_module, maka command-nya kurang lebih seperti berikut:

```
cd first_module
go get github.com/segmentio/kafka-go
```

![Enter image alt description](Images/8Dy_Image_4.png)


Pada contoh di atas, bisa dilihat bahwa URL [github.com/segmentio/kafka-go](http://github.com/segmentio/kafka-go) merupakan URL package kafka-go. Package yang sudah terunduh tersimpan dalam temporary folder yang ter-link dengan project folder di mana *command* go get dieksekusi, menjadikan project tersebut bisa meng-*import* package yang telah di-download. Command go get **harus dijalankan dalam folder project**. Jika dijalankan di-luar path project maka dependency yang ter-unduh akan ter-link dengan GOPATH, bukan dengan project.

# 2.3 Go Routines

Goroutine secara konsep mirip seperti *thread*, meskipun sebenarnya berbeda. Sebuah *native thread* bisa berisikan sangat banyak goroutine. Mungkin lebih pas kalau goroutine disebut sebagai mini thread. Goroutine sangat ringan, hanya dibutuhkan sekitar 2kB memori saja untuk satu buah goroutine. Eksekusi goroutine bersifat *asynchronous*, menjadikannya tidak saling tunggu dengan goroutine lain.


>*Karena goroutine sangat ringan, maka eksekusi banyak goroutine bukan masalah. Akan tetapi jika jumlah goroutine sangat banyak sekali (contoh 1 juta goroutine dijalankan pada komputer dengan RAM terbatas), memang proses akan jauh lebih cepat selesai, tapi memory/RAM pasti bengkak. Selain itu, dalam pengaplikasiannya jangan hanya terpaku pada size goroutine yang kecil tersebut, tapi pertimbangkan juga kode/proses/logic yang dibuat di dalam goroutine itu sekompleks apa, karena hal tersebut sangat berpengaruh dengan konsumsi resource hardware.*
>
>Goroutine merupakan salah satu bagian paling penting dalam *concurrent programming* di Go. Salah satu yang membuat goroutine sangat istimewa adalah eksekusi-nya dijalankan di multi core processor. Kita bisa tentukan berapa banyak core yang aktif, makin banyak akan makin cepat. 

Untuk menerapkan goroutine, proses yang akan dieksekusi sebagai goroutine harus dibungkus ke dalam sebuah fungsi, ini hukumnya wajib. Kemudian nantinya saat pemanggilan fungsi, tambahkan keyword go di depannya, dengan ini maka goroutine baru dibuat dengan tugas adalah menjalankan proses yang ada dalam fungsi tersebut. 

Berikut merupakan contoh implementasi sederhana tentang goroutine. Program di bawah ini menampilkan 10 baris teks, 5 dieksekusi dengan cara biasa, dan 5 lainnya dieksekusi sebagai goroutine baru.

```go
package main
import "fmt"
import "runtime"

func print(till int, message string) {
    for i := 0; i < till; i++ {
        fmt.Println((i + 1), message)
    }
}
func main() {
    runtime.GOMAXPROCS(2)

    go print(5, "halo")
    print(5, "apa kabar")

    var input string
    fmt.Scanln(&input)
}
```
Pada kode di atas, Fungsi runtime.GOMAXPROCS(n) digunakan untuk menentukan jumlah core yang diaktifkan untuk eksekusi program.

Pembuatan goroutine baru ditandai dengan keyword go. Contohnya pada statement go print(5, "halo"), di situ fungsi print() dieksekusi sebagai goroutine baru.

Fungsi fmt.Scanln() mengakibatkan proses jalannya aplikasi berhenti di baris itu (**blocking**) hingga user menekan tombol enter. Hal ini perlu dilakukan karena ada kemungkinan waktu selesainya eksekusi goroutine print() lebih lama dibanding waktu selesainya goroutine utama main(), mengingat bahwa keduanya sama-sama asnychronous. Jika itu terjadi, goroutine yang belum selesai secara paksa dihentikan prosesnya karena goroutine utama sudah selesai dijalankan. Output : 

![Enter image alt description](Images/VvI_Image_5.png)

# 2.4 Struktur Umum Folder Proyek 

## 1. MVC

### 2.4.A.1 Controller

Air traffic controllers adalah orang-orang yang memberi tahu setiap pesawat di bandara tentang ke mana harus terbang, kapan harus mendarat, dan di landasan mana harus mendarat. Mereka tidak menerbangkan pesawat secara langsung, tetapi bertugas memberikan arahan agar bandara bisa berjalan dengan lancar.

Controllers dalam MVC sangat mirip dengan air traffic controllers. Mereka tidak bertanggung jawab langsung untuk menulis ke database atau membuat HTML response, tetapi mereka mengarahkan data yang masuk ke model, view, dan package lain yang sesuai untuk menyelesaikan permintaan.

### 	2.4.A.2 Models

Models adalah bagian dari MVC, dan tanggung jawab utamanya adalah berinteraksi dengan data milik aplikasi kamu. Ini biasanya berarti berkomunikasi dengan database, tapi bisa juga berarti berinteraksi dengan data yang berasal dari layanan atau API lain. Bisa juga termasuk proses validasi atau normalisasi data.

Contoh singkat — sebagian besar web application biasanya memiliki konsep *user* yang mendaftar dan berinteraksi dengan aplikasi tersebut. Dalam aplikasi berbasis MVC, kita mungkin membuat tipe User struct {...} di dalam package *models* yang merepresentasikan user yang disimpan di dalam database. Lalu kita bisa membuat kode untuk membantu memvalidasi apakah user tersebut valid, serta kode tambahan untuk membuat, memperbarui, atau mencari data user di database.

Satu hal penting yang ingin saya tekankan di sini adalah bahwa saya menganggap package *models* mencakup seluruh interaksi dengan database.

### 	2.4.A.3 Routes

Dalam arsitektur **MVC (Model-View-Controller)**, folder routes berfungsi untuk mendefinisikan **rute (routes)** atau **jalur-jalur URL** yang akan diproses oleh aplikasi web. Folder ini biasanya berisi kode yang menghubungkan **request HTTP (seperti GET, POST, PUT, DELETE)** ke **controller** yang sesuai.

### 	2.4.A.4 Database

Sebaiknya lapisan *database* di-*abstract* agar jika suatu saat kamu perlu melakukan perubahan, kamu tidak perlu mencari-cari query di dalam *business logic*. Semua query disimpan di dalam folder *models*.

Proyek ini mendukung BoltDB, MongoDB, dan MySQL. Semua query disimpan dalam file yang sama, sehingga kamu bisa dengan mudah mengganti *database* hanya dengan mengubah file konfigurasi, tanpa perlu memodifikasi bagian lain.

File user.go dan note.go berada di root direktori *model* dan merupakan kompilasi dari semua query untuk masing-masing jenis *database*. Ada beberapa *hack* di bagian *models* agar struct bisa bekerja dengan semua *database* yang didukung.

### 	2.4.A.5 DTO

**DTO (Data Transfer Object)** adalah struktur data yang digunakan untuk **mengirim dan menerima data** antar lapisan (misalnya dari controller ke model, atau dari server ke client), **tanpa menyertakan logic bisnis atau database logic**.

### 	2.4.A.6 Middleware

Middleware biasanya berisi **fungsi-fungsi perantara** yang dijalankan sebelum (atau sesudah) permintaan HTTP diproses oleh controller. Terdapat beberapa *middleware* yang sudah disertakan. Package bernama **csrfbanana** melindungi dari serangan *Cross-Site Request Forgery* dan mencegah pengiriman ganda (*double submit*). Package **httprouterwrapper** menyediakan fungsi bantu (*helper*) agar fungsi bisa kompatibel dengan **httprouter**. Package **logrequest** akan mencatat setiap permintaan (*request*) ke situs web ke dalam konsol.

Package **pprofhandler** mengaktifkan **pprof** agar dapat bekerja dengan **httprouter**.

Di dalam file route.go, semua *route* individu menggunakan **alice** agar *chaining middleware* menjadi sangat mudah

## 2. Common Folder Structures in GO

```
project-root/
    ├── cmd/
    │   ├── your-app-name/
    │   │   ├── main.go         # Application entry point
    │   │   └── ...             # Other application-specific files
    │   └── another-app/
    │       ├── main.go         # Another application entry point
    │       └── ...
    ├── internal/                # Private application and package code
    │   ├── config/
    │   │   ├── config.go       # Configuration logic
    │   │   └── ...
    │   ├── database/
    │   │   ├── database.go     # Database setup and access
    │   │   └── ...
    │   └── ...
    ├── pkg/                     # Public, reusable packages
    │   ├── mypackage/
    │   │   ├── mypackage.go    # Public package code
    │   │   └── ...
    │   └── ...
    ├── api/                     # API-related code (e.g., REST or gRPC)
    │   ├── handler/
    │   │   ├── handler.go      # HTTP request handlers
    │   │   └── ...
    │   ├── middleware/
    │   │   ├── middleware.go  # Middleware for HTTP requests
    │   │   └── ...
    │   └── ...
    ├── web/                     # Front-end web application assets
    │   ├── static/
    │   │   ├── css/
    │   │   ├── js/
    │   │   └── ...
    │   └── templates/
    │       ├── index.html
    │       └── ...
    ├── scripts/                 # Build, deployment, and maintenance scripts
    │   ├── build.sh
    │   ├── deploy.sh
    │   └── ...
    ├── configs/                 # Configuration files for different environments
    │   ├── development.yaml
    │   ├── production.yaml
    │   └── ...
    ├── tests/                   # Unit and integration tests
    │   ├── unit/
    │   │   ├── ...
    │   └── integration/
    │       ├── ...
    ├── docs/                    # Project documentation
    ├── .gitignore               # Gitignore file
    ├── go.mod                   # Go module file
    ├── go.sum                   # Go module dependencies file
    └── README.md                # Project README
```
Berikut penjelasan singkat mengenai direktori-direktori utama:

- **cmd/**: Direktori ini berisi *entry point* spesifik aplikasi (biasanya satu untuk setiap aplikasi atau service). Di sinilah aplikasi kamu dimulai.
- **internal/**: Direktori ini menyimpan kode aplikasi dan package yang bersifat privat. Kode di dalam direktori ini tidak dimaksudkan untuk digunakan oleh proyek lain. Ini merupakan cara untuk membatasi akses dalam proyek kamu.
- **pkg/**: Direktori ini berisi package publik dan dapat digunakan kembali oleh proyek lain. Kode di dalam direktori ini dimaksudkan untuk di-*import* oleh proyek eksternal.
- **api/**: Direktori ini biasanya berisi kode yang terkait dengan API HTTP atau RPC, termasuk *request handler* dan *middleware*.
- **web/**: Jika proyek kamu memiliki aplikasi web front-end, maka aset-aset seperti CSS, JavaScript, dan template ditempatkan di sini.
- **scripts/**: Berisi skrip untuk membangun (*build*), menyebarkan (*deploy*), atau melakukan pemeliharaan proyek.
- **configs/**: Berisi file konfigurasi untuk berbagai lingkungan (*environment*), misalnya untuk *development* atau *production*.
- **tests/**: Menyimpan *unit test* dan *integration test* untuk kode kamu.
- **docs/**: Dokumentasi proyek, seperti dokumen desain atau dokumentasi API.

### - Flat Structure

```
project-root/
    ├── main.go
    ├── handler.go
    ├── config.go
    ├── database.go
    ├── ...
    ├── static/
    ├── templates/
    ├── scripts/
    ├── configs/
    ├── tests/
    └── docs/
```
### - Layered Structure

```
project-root/
    ├── main.go
    ├── web/
    │   ├── handler.go
    │   ├── static/
    │   ├── templates/
    ├── api/
    │   ├── routes.go
    │   ├── middleware/
    ├── data/
    │   ├── database.go
    │   ├── repository.go
    ├── configs/
    ├── tests/
    ├── docs/
```
### - Domain-Driven Design (DDD)

```
project-root/
    ├── cmd/
    │   ├── app1/
    │   ├── app2/
    ├── internal/
    │   ├── auth/
    │   │   ├── handler.go
    │   │   ├── service.go
    │   ├── orders/
    │   │   ├── handler.go
    │   │   ├── service.go
    │   ├── ...
    ├── pkg/
    │   ├── utility/
    │   │   ├── ...
    │   ├── ...
    ├── api/
    │   ├── app1/
    │   │   ├── ...
    │   ├── app2/
    │   │   ├── ...
    ├── web/
    │   ├── app1/
    │   │   ├── ...
    │   ├── app2/
    │   │   ├── ...
    ├── scripts/
    ├── configs/
    ├── tests/
    └── docs/
```
### - Clean Architecture

```
project-root/
    ├── cmd/
    │   ├── your-app/
    │   │   ├── main.go
    ├── internal/
    │   ├── app/
    │   │   ├── handler.go
    │   │   ├── service.go
    │   ├── domain/
    │   │   ├── model.go
    │   │   ├── repository.go
    ├── pkg/
    │   ├── utility/
    │   │   ├── ...
    ├── api/
    │   ├── ...
    ├── web/
    │   ├── ...
    ├── scripts/
    ├── configs/
    ├── tests/
    └── docs/
```
### - Modular Structure

```
project-root/
    ├── module1/
    │   ├── cmd/
    │   ├── internal/
    │   ├── pkg/
    │   ├── api/
    │   ├── web/
    │   ├── scripts/
    │   ├── configs/
    │   ├── tests/
    │   └── docs/
    ├── module2/
    │   ├── ...
```
# 2.5 Variable

Go mengadopsi dua jenis penulisan variabel, yaitu yang dituliskan tipe data-nya dan yang tidak. Kedua cara tersebut valid dan tujuannya sama yaitu untuk deklarasi variabel, pembedanya hanya pada cara penulisannya saja.

## 2.5.1. Deklarasi Variabel Beserta Tipe Data

Go memiliki aturan cukup ketat dalam hal penulisan variabel. Ketika deklarasi, tipe data yg digunakan harus dituliskan juga. Istilah dari metode deklarasi variabel ini adalah **manifest typing**. Berikut adalah contoh cara pembuatan variabel yang tipe datanya harus ditulis. Silakan tulis pada project baru atau pada project yang sudah ada, bebas. Pastikan pada setiap pembuatan project baru untuk tidak lupa menginisialisasi project menggunakan command go mod init <nama-project>.

```go
package main

import "fmt"

func main() {
    var firstName string = "john"

    var lastName string
    lastName = "wick"

    fmt.Printf("halo %s %s!\n", firstName, lastName)
}
```
Keyword var di atas digunakan untuk deklarasi variabel, contohnya bisa dilihat pada firstName dan lastName. Nilai variabel firstName diisi langsung ketika deklarasi, berbeda dibanding lastName yang nilainya diisi setelah baris kode deklarasi, hal seperti ini diperbolehkan di Go.

## 2.5.2. Deklarasi Variabel Menggunakan Keyword var

Pada kode di atas bisa dilihat bagaimana sebuah variabel dideklarasikan dan diisi nilainya. Keyword var digunakan untuk membuat variabel baru. Skema penggunaan keyword var:
```
var <nama-variable> <tipe-data>

var <nama-variable> <tipe-data> = <nilai>
```

Contoh:
```go
var firstName string = "john"

var lastName string
```
Nilai variabel bisa di-isi langsung pada saat deklarasi variabel.

### - Penggunaan Fungsi fmt.Printf()

Fungsi ini digunakan untuk menampilkan output dalam bentuk tertentu. Kegunaannya sama seperti fungsi fmt.Println(), hanya saja struktur outputnya didefinisikan di awal. Perhatikan bagian** ****"halo %s %s!\n"****,** karakter %s di situ akan diganti dengan data string yang berada di parameter ke-2, ke-3, dan seterusnya.

Contoh lain, ketiga baris kode berikut ini akan menghasilkan output yang sama, meskipun cara penulisannya berbeda.

```go
fmt.Printf("halo john wick!\n")
fmt.Printf("halo %s %s!\n", firstName, lastName)
fmt.Println("halo", firstName, lastName + "!")
```
Tanda plus (+) jika digunakan untuk penghubung 2 data string fungsinya adalah untuk operasi penggabungan string atau *string concatenation*.

Fungsi fmt.Printf() tidak menghasilkan baris baru di akhir text, oleh karena itu digunakanlah **literal *****newline***** yaitu ****\n****,** untuk memunculkan baris baru di akhir. Hal ini sangat berbeda jika dibandingkan dengan fungsi** ****fmt.Println()**** yang secara otomatis menghasilkan new line** (baris baru) di akhir.

## 2.5.3. Deklarasi Variabel Tanpa Tipe Data

Selain *manifest typing*, Go juga mengadopsi konsep **type inference**, yaitu metode deklarasi variabel yang tipe data-nya diketahui secara otomatis dari data/nilai variabel. Cara ini kontradiktif jika dibandingkan dengan cara pertama. Dengan metode jenis ini, keyword var dan tipe data tidak perlu ditulis.

```go
var firstName string = "john"
lastName := "wick"
fmt.Printf("halo %s %s!\n", firstName, lastName)
```
Variabel lastName dideklarasikan dengan menggunakan metode type inference. Penandanya tipe data tidak dituliskan pada saat deklarasi. Pada penggunaan metode ini, operand = harus diganti dengan := dan keyword var dihilangkan.

Tipe data lastName secara otomatis akan ditentukan menyesuaikan value atau nilai-nya. Jika nilainya adalah berupa string maka tipe data variabel adalah string. Pada contoh di atas, nilainya adalah string "wick". Diperbolehkan untuk tetap menggunakan keyword var pada saat deklarasi meskipun tanpa menuliskan tipe data, dengan ketentuan tidak menggunakan tanda :=, melainkan tetap menggunakan =.

```go
// menggunakan var, tanpa tipe data, menggunakan perantara "="
var firstName = "john"

// tanpa var, tanpa tipe data, menggunakan perantara ":="
lastName := "wick"
```
Tanda := hanya digunakan sekali di awal pada saat deklarasi. Untuk assignment nilai selanjutnya harus menggunakan tanda =, contoh:
```go
lastName := "wick"

lastName = "ethan"

lastName = "bourne"
```
*Deklarasi menggunakan **:=** hanya bisa dilakukan di dalam blok fungsi, misalnya dalam blok fungsi **main()*

## 2.5.4. Deklarasi Multi Variabel

Go mendukung metode deklarasi banyak variabel secara bersamaan, caranya dengan menuliskan variabel-variabel-nya dengan pembatas tanda koma (,). Untuk pengisian nilainya-pun diperbolehkan secara bersamaan.
```go
var first, second, third string

first, second, third = "satu", "dua", "tiga"
```
Pengisian nilai juga bisa dilakukan bersamaan pada saat deklarasi. Caranya dengan menuliskan nilai masing-masing variabel berurutan sesuai variabelnya dengan pembatas koma (,).
```go
var fourth, fifth, sixth string = "empat", "lima", "enam"
```
Kalau ingin lebih ringkas:
```go
seventh, eight, ninth := "tujuh", "delapan", "sembilan"
```
Dengan menggunakan teknik type inference, deklarasi multi variabel bisa dilakukan untuk variabel-variabel yang tipe data satu sama lainnya berbeda.
```go
one, isFriday, twoPointTwo, say := 1, true, 2.2, "hello"
```
## 2.5.5. Variabel Underscore _

Go memiliki aturan unik yang jarang dimiliki bahasa lain, yaitu tidak boleh ada satupun variabel yang menganggur. Artinya, semua variabel yang dideklarasikan harus digunakan. Jika ada variabel yang tidak digunakan tapi dideklarasikan, error akan muncul pada saat kompilasi dan program tidak akan bisa di-run.

![Enter image alt description](Images/yhp_Image_6.png)

*Underscore* (_) adalah *reserved variable* yang bisa dimanfaatkan untuk menampung nilai yang tidak dipakai. Bisa dibilang variabel ini merupakan keranjang sampah.
```go
_ = "belajar Golang"

_ = "Golang itu mudah"

name, _ := "john", "wick"
```
Pada contoh di atas, variabel name akan berisikan text john, sedang nilai wick ditampung oleh variabel underscore, menandakan bahwa nilai tersebut tidak akan digunakan.

Variabel underscore adalah *predefined*, jadi tidak perlu menggunakan := untuk pengisian nilai, cukup dengan = saja. Namun khusus untuk pengisian nilai multi variabel yang dilakukan dengan metode type inference, boleh di dalamnya terdapat variabel underscore.

Biasanya variabel underscore sering dimanfaatkan untuk menampung nilai balik fungsi yang tidak digunakan.

Perlu diketahui, bahwa isi variabel underscore tidak dapat ditampilkan. Data yang sudah masuk variabel tersebut akan hilang. Ibarat variabel underscore ini seperti blackhole, objek apapun yang masuk ke dalamnya, akan terjebak selamanya di-dalam singularity dan tidak akan bisa keluar .

## 2.5.6. Deklarasi Variabel Menggunakan Keyword new

Fungsi new() digunakan untuk membuat variabel **pointer** dengan tipe data tertentu. Nilai data default-nya akan menyesuaikan tipe datanya.

```go
name := new(string)

fmt.Println(name)   // 0x20818a220
fmt.Println(*name)  // ""
```
Variabel name menampung data bertipe **pointer string**. Jika ditampilkan yang muncul bukanlah nilainya melainkan alamat memori nilai tersebut (dalam bentuk notasi heksadesimal). Untuk menampilkan nilai aslinya, variabel tersebut perlu di-**dereference** terlebih dahulu, caranya dengan menuliskan tanda asterisk (*) sebelum nama variabel.

## 2.5.7. Deklarasi Variabel Menggunakan Keyword make

Fungsi make() ini hanya bisa digunakan untuk pembuatan beberapa jenis variabel saja, yaitu:

- channel

- slice

- Map

# 2.6 Tipe Data

Go mengenal beberapa jenis tipe data, di antaranya adalah tipe data numerik (desimal & non-desimal), string, dan boolean.

Pada pembahasan-pembahasan sebelumnya secara tak sadar kita sudah mengaplikasikan beberapa tipe data, diantaranya ada string dan tipe numerik int.

## 2.6.1. Tipe Data Numerik Non-Desimal

Tipe data numerik non-desimal atau **non floating point** di Go ada beberapa jenis. Secara umum ada 2 tipe data kategori ini yang perlu diketahui.

- uint, tipe data untuk bilangan cacah (bilangan positif).

- int, tipe data untuk bilangan bulat (bilangan negatif dan positif).

Kedua tipe data di atas kemudian dibagi lagi menjadi beberapa jenis, dengan pembagian berdasarkan lebar cakupan nilainya, detailnya bisa dilihat di tabel berikut.

| Tipe data | Cakupan bilangan |
|---|---|
| uint8 | 0 ↔ 255 |
| uint16 | 0 ↔ 65535 |
| uint32 | 0 ↔ 4294967295 |
| uint64 | 0 ↔ 18446744073709551615 |
| uint | sama dengan uint32 atau uint64 (tergantung nilai) |
| byte | sama dengan uint8 |
| int8 | -128 ↔ 127 |
| int16 | -32768 ↔ 32767 |
| int32 | -2147483648 ↔ 2147483647 |
| int64 | -9223372036854775808 ↔ 9223372036854775807 |
| int | sama dengan int32 atau int64 (tergantung nilai) |
| rune | sama dengan int32 |

Dianjurkan untuk tidak sembarangan dalam menentukan tipe data variabel, sebisa mungkin tipe yang dipilih harus disesuaikan dengan nilainya, karena efeknya adalah ke alokasi memori variabel. Pemilihan tipe data yang tepat akan membuat pemakaian memori lebih optimal, tidak berlebihan.

```go
var positiveNumber uint8 = 89
var negativeNumber = -1243423644

fmt.Printf("bilangan positif: %d\n", positiveNumber)
fmt.Printf("bilangan negatif: %d\n", negativeNumber)
```
Variabel positiveNumber bertipe uint8 dengan nilai awal 89. Sedangkan variabel negativeNumber dideklarasikan dengan nilai awal -1243423644. Compiler secara cerdas akan menentukan tipe data variabel tersebut sebagai int32 (karena angka tersebut masuk ke cakupan tipe data int32).

String format %d pada fmt.Printf() digunakan untuk memformat data numerik non-desimal.

## 2.6.2. Tipe Data Numerik Desimal

Tipe data numerik desimal yang perlu diketahui ada 2, float32 dan float64. Perbedaan kedua tipe data tersebut berada di lebar cakupan nilai desimal yang bisa ditampung. Untuk lebih jelasnya bisa merujuk ke spesifikasi [IEEE-754 32-bit floating-point numbers](http://www.h-schmidt.net/FloatConverter/IEEE754.html).

```go
var decimalNumber = 2.62

fmt.Printf("bilangan desimal: %f\n", decimalNumber)
fmt.Printf("bilangan desimal: %.3f\n", decimalNumber)
```
Pada kode di atas, variabel decimalNumber akan memiliki tipe data float32, karena nilainya berada di cakupan tipe data tersebut.

![Enter image alt description](Images/WLC_Image_7.png)

String format %f digunakan untuk memformat data numerik desimal menjadi string. Digit desimal yang akan dihasilkan adalah **6 digit**. Pada contoh di atas, hasil format variabel decimalNumber adalah 2.620000. Jumlah digit yang muncul bisa dikontrol menggunakan %.nf, tinggal ganti n dengan angka yang diinginkan. Contoh: %.3f maka akan menghasilkan 3 digit desimal, %.10f maka akan menghasilkan 10 digit desimal.

## 2.6.3. Tipe Data bool (Boolean)

Tipe data bool berisikan hanya 2 variansi nilai, true dan false. Tipe data ini biasa dimanfaatkan dalam percabangan dan Loop 
```go
var exist bool = true
fmt.Printf("exist? %t \n", exist)
```
Gunakan %t untuk memformat data bool menggunakan fungsi fmt.Printf().

## 2.6.4. Tipe Data string

Ciri khas dari tipe data string adalah nilainya diapit oleh tanda *quote* atau petik dua ("). Contoh penerapannya:
```go
var message string = "Halo"
fmt.Printf("message: %s \n", message)
```
Selain menggunakan tanda quote, deklarasi string juga bisa dengan tanda *grave accent/backticks* (`), tanda ini terletak di sebelah kiri tombol 1. Keistimewaan string yang dideklarasikan menggunakan backtics adalah membuat semua karakter di dalamnya **tidak di escape**, termasuk \n, tanda petik dua dan tanda petik satu, baris baru, dan lainnya. Semua akan terdeteksi sebagai string.

```go
var message = `Nama saya "John Wick".
Salam kenal.
Mari belajar "Golang".`

fmt.Println(message)
```
Ketika dijalankan, output akan muncul sama persis sesuai nilai variabel message di atas. Tanda petik dua akan muncul, baris baru juga muncul, sama persis.

![Enter image alt description](Images/AIH_Image_8.png)

## 2.6.5. Nilai nil & Zero Value

nil bukan merupakan tipe data, melainkan sebuah nilai. Variabel yang isi nilainya nil berarti memiliki nilai kosong.
Semua tipe data yang sudah dibahas di atas memiliki zero value (nilai default tipe data). Artinya meskipun variabel dideklarasikan dengan tanpa nilai awal, tetap akan ada nilai default-nya.

- Zero value dari string adalah "" (string kosong).
- Zero value dari bool adalah false.
- Zero value dari tipe numerik non-desimal adalah 0.
- Zero value dari tipe numerik desimal adalah 0.0.

Selain tipe data yang disebutkan di atas, ada juga tipe data lain yang zero value-nya adalah nil. Nil merepresentasikan nilai kosong, benar-benar kosong. nil tidak bisa digunakan pada tipe data yang sudah dibahas di atas.

Beberapa tipe data yang bisa di-set nilainya dengan nil, di antaranya:

- pointer
- tipe data fungsi
- slice
- map
- channel
- interface kosong atau any (yang merupakan alias dari interface{})

# 2.7 Operator

## 2.7.1. Operator Aritmatika

Operator aritmatika adalah operator yang digunakan untuk operasi yang sifatnya perhitungan. Go mendukung beberapa operator aritmatika standar, list-nya bisa dilihat di tabel berikut.

| Tanda | Penjelasan |
|---|---|
| + | penjumlahan |
| - | pengurangan |
| * | perkalian |
| / | pembagian |
| % | modulus / sisa hasil pembagian |

Contoh penggunaan:
```go
var value = (((2 + 6) % 3) * 4 - 2) / 3
```
## 2.7.2. Operator Perbandingan

Operator perbandingan digunakan untuk menentukan kebenaran suatu kondisi. Hasilnya berupa nilai boolean, true atau false.

Tabel di bawah ini berisikan operator perbandingan yang bisa digunakan di Go.

| Tanda | Penjelasan |
|---|---|
| == | apakah nilai kiri sama dengan nilai kanan |
| != | apakah nilai kiri tidak sama dengan nilai kanan |
| < | apakah nilai kiri lebih kecil daripada nilai kanan |
| <= | apakah nilai kiri lebih kecil atau sama dengan nilai kanan |
| > | apakah nilai kiri lebih besar dari nilai kanan |
| >= | apakah nilai kiri lebih besar atau sama dengan nilai kanan |

Contoh penggunaan:

```go
var value = (((2 + 6) % 3) * 4 - 2) / 3
var isEqual = (value == 2)
fmt.Printf("nilai %d (%t) \n", value, isEqual)
```
Pada kode di atas, terdapat statement operasi aritmatika yang hasilnya ditampung oleh variabel value. Selanjutnya, variabel tersebut dibandingkan dengan angka **2** untuk dicek apakah nilainya sama. Jika iya, maka hasilnya adalah true, jika tidak maka false. Nilai hasil operasi perbandingan tersebut kemudian disimpan dalam variabel isEqual.

Untuk memunculkan nilai bool menggunakan fmt.Printf(), bisa gunakan layout format %t.

## 2.7.3. Operator Logika

Operator ini digunakan untuk mencari benar tidaknya kombinasi data bertipe bool (bisa berupa variabel bertipe bool, atau hasil dari operator perbandingan).

Beberapa operator logika standar yang bisa digunakan:

| Tanda | Penjelasan |
|---|---|
| && | kiri dan kanan |
| \|\| | kiri atau kanan |
| ! | negasi / nilai kebalikan |

Contoh penggunaan:

```go
var left = false
var right = true

var leftAndRight = left && right
fmt.Printf("left && right \t(%t) \n", leftAndRight)

var leftOrRight = left || right
fmt.Printf("left || right \t(%t) \n", leftOrRight)

var leftReverse = !left
fmt.Printf("!left \t\t(%t) \n", leftReverse)
```
Hasil dari operator logika sama dengan hasil dari operator perbandingan, yaitu berupa boolean.

Berikut penjelasan statemen operator logika pada kode di atas.

- leftAndRight bernilai false, karena hasil dari false **dan** true adalah false.
- leftOrRight bernilai true, karena hasil dari false **atau** true adalah true.
- leftReverse bernilai true, karena **negasi** (atau lawan dari) false adalah true.

Template \t digunakan untuk menambahkan indent tabulasi. Biasa dimanfaatkan untuk merapikan tampilan output pada console.

# 2.8 Percabangan

Percabangan digunakan untuk mengontrol alur eksekusi flow program. Analoginya mirip seperti fungsi rambu lalu lintas di jalan raya. Kapan kendaraan diperbolehkan melaju dan kapan harus berhenti diatur oleh rambu tersebut. percabangan pada program juga kurang lebih sama, kapan sebuah blok kode dieksekusi dikontrol.

Yang dijadikan acuan oleh percabangan adalah nilai bertipe bool, bisa berasal dari variabel, ataupun hasil operasi perbandingan. Nilai tersebut menentukan blok kode mana yang akan dieksekusi.

Go memiliki 2 macam keyword untuk percabangan, yaitu **if else** dan **switch**. Pada chapter ini kita akan mempelajari keduanya.

*Go tidak mendukung percabangan menggunakan ****ternary****. \
Statement seperti **var data = (isExist ? "ada" : "tidak ada")** adalah invalid dan menghasilkan error.*

## 2.8.1. Percabangan Keyword if, else if, & else

Cara penerapan if-else di Go sama seperti pada bahasa pemrograman lain. Yang membedakan hanya tanda kurungnya *(parentheses)*, di Go tidak perlu ditulis. Kode berikut merupakan contoh penerapan percabangan if else, dengan jumlah kondisi 4 buah.

```go
var point = 8

if point == 10 {
    fmt.Println("lulus dengan nilai sempurna")
} else if point > 5 {
    fmt.Println("lulus")
} else if point == 4 {
    fmt.Println("hampir lulus")
} else {
    fmt.Printf("tidak lulus. nilai anda %d\n", point)
}
```
Dari ke-empat kondisi di atas, yang terpenuhi adalah if point > 5, karena nilai variabel point memang lebih besar dari 5. Maka blok kode tepat di bawah kondisi tersebut akan dieksekusi (blok kode ditandai kurung kurawal buka dan tutup), hasilnya text "lulus" muncul sebagai output.

![Enter image alt description](Images/Ktf_Image_9.png)

Penulisan if else Go diawali dengan keyword if kemudian diikuti nilai percabangan dan blok kode ketika kondisi terpenuhi. Ketika kondisinya tidak terpenuhi akan blok kode else dipanggil (jika blok kode else tersebut ada). Ketika ada banyak kondisi, gunakan else if.

## 2.8.2. Variabel Temporary Pada if - else

Variabel temporary adalah variabel yang hanya bisa digunakan pada deretan blok percabangan di mana ia ditempatkan. Penggunaan variabel ini membawa beberapa manfaat, antara lain:

- Scope atau cakupan variabel jelas, hanya bisa digunakan pada blok percabangan itu saja
- Kode menjadi lebih rapi
- Ketika nilai variabel tersebut didapat dari sebuah komputasi, perhitungan tidak perlu dilakukan di dalam blok masing-masing kondisi.

```go
var point = 8840.0

if percent := point / 100; percent >= 100 {
    fmt.Printf("%.1f%s perfect!\n", percent, "%")
} else if percent >= 70 {
    fmt.Printf("%.1f%s good\n", percent, "%")
} else {
    fmt.Printf("%.1f%s not bad\n", percent, "%")
}
```
Variabel percent nilainya didapat dari hasil perhitungan, dan hanya bisa digunakan di deretan blok percabangan itu saja yang mencakup blok if, else if, dan else.

*Deklarasi variabel temporary hanya bisa dilakukan lewat metode type inference yang menggunakan tanda **:=**. Penggunaan keyword **var** di situ tidak diperbolehkan karena menyebabkan error.*

## 2.8.3. Percabangan Keyword switch - case

Switch merupakan percabangan yang sifatnya fokus pada satu variabel, lalu kemudian di-cek nilainya. Contoh sederhananya seperti penentuan apakah nilai variabel x adalah: 1, 2, 3, atau lainnya.

```go
var point = 6

switch point {
case 8:
    fmt.Println("perfect")
case 7:
    fmt.Println("awesome")
default:
    fmt.Println("not bad")
}
```
Pada kode di atas, tidak ada kondisi atau case yang terpenuhi karena nilai variabel point tetap 6. Ketika hal seperti ini terjadi, blok kondisi default dipanggil. Bisa dibilang bahwa default merupakan else dalam sebuah switch.

Perlu diketahui, switch pada pemrograman Go memiliki perbedaan dibanding bahasa lain. Di Go, ketika sebuah case terpenuhi, tidak akan dilanjutkan ke pengecekan case selanjutnya, meskipun tidak ada keyword break di situ. Konsep ini berkebalikan dengan switch pada umumnya pemrograman lain (yang ketika sebuah case terpenuhi, maka akan tetap dilanjut mengecek case selanjutnya kecuali ada keyword break).

## 2.8.4. Pemanfaatan case Untuk Banyak Kondisi

Sebuah case dapat menampung banyak kondisi. Cara penerapannya yaitu dengan menuliskan nilai pembanding-pembanding variabel yang di-switch setelah keyword case dipisah tanda koma (,).

```go
var point = 6

switch point {
case 8:
    fmt.Println("perfect")
case 7, 6, 5, 4:
    fmt.Println("awesome")
default:
    fmt.Println("not bad")
}
```
Kondisi case 7, 6, 5, 4: akan terpenuhi ketika nilai variabel point adalah 7 atau 6 atau 5 atau 4.

## 2.8.5. Kurung Kurawal Pada Keyword case & default

Tanda kurung kurawal ({ }) bisa diterapkan pada keyword case dan default. Tanda ini opsional, boleh dipakai boleh tidak. Bagus jika dipakai pada blok kondisi yang di dalamnya ada banyak statement, dengannya kode akan terlihat lebih rapi.

Perhatikan kode berikut, bisa dilihat pada keyword default terdapat kurung kurawal yang mengapit 2 statement di dalamnya.

```go
var point = 6

switch point {
case 8:
    fmt.Println("perfect")
case 7, 6, 5, 4:
    fmt.Println("awesome")
default:
    {
        fmt.Println("not bad")
        fmt.Println("you can be better!")
    }
}
```
## 2.8.6. Switch Dengan Gaya if - else

Uniknya di Go, switch bisa digunakan dengan gaya ala if-else. Nilai yang akan dibandingkan tidak dituliskan setelah keyword switch, melainkan akan ditulis langsung dalam bentuk perbandingan dalam keyword case.

Pada kode di bawah ini, kode program switch di atas diubah ke dalam gaya if-else. Variabel point dihilangkan dari keyword switch, lalu kondisi-kondisinya dituliskan di tiap case.

```go
var point = 6

switch {
case point == 8:
    fmt.Println("perfect")
case (point < 8) && (point > 3):
    fmt.Println("awesome")
default:
    {
        fmt.Println("not bad")
        fmt.Println("you need to learn more")
    }
}
```
## 2.8.7. Penggunaan Keyword fallthrough Dalam switch

Seperti yang sudah dijelaskan sebelumnya, bahwa switch pada Go memiliki perbedaan dengan bahasa lain. Ketika sebuah case terpenuhi, pengecekan kondisi tidak akan diteruskan ke case-case setelahnya.

Keyword fallthrough digunakan untuk memaksa proses pengecekan tetap diteruskan ke case selanjutnya dengan **tanpa menghiraukan nilai kondisinya**, efeknya adalah case di pengecekan selanjutnya selalu dianggap true (meskipun aslinya bisa saja kondisi tersebut tidak terpenuhi, akan tetap dianggap true).

```go
var point = 6

switch {
case point == 8:
    fmt.Println("perfect")
case (point < 8) && (point > 3):
    fmt.Println("awesome")
    fallthrough
case point < 5:
    fmt.Println("you need to learn more")
default:
    {
        fmt.Println("not bad")
        fmt.Println("you need to learn more")
    }
}
```
Di contoh, setelah pengecekan case (point < 8) && (point > 3) selesai, dilanjut ke pengecekan case point < 5, karena ada fallthrough di situ. Dan kondisi case < 5 tersebut dianggap true meskipun secara logika harusnya tidak terpenuhi.

![Enter image alt description](Images/4X0_Image_10.png)

## 2.8.8. Percabangan Bersarang

Percabangan bersarang adalah percabangan, yang berada dalam percabangan, yang mungkin juga berada dalam percabangan, dan seterusnya. percabangan bersarang bisa dilakukan pada if - else, switch, ataupun kombinasi keduanya.

```go
var point = 10

if point > 7 {
    switch point {
    case 10:
        fmt.Println("perfect!")
    default:
        fmt.Println("nice!")
    }
} else {
    if point == 5 {
        fmt.Println("not bad")
    } else if point == 3 {
        fmt.Println("keep trying")
    } else {
        fmt.Println("you can do it")
        if point == 0 {
            fmt.Println("try harder!")
        }
    }
}
```
# 2.9 Loop

Loop adalah proses mengulang-ulang eksekusi blok kode tanpa henti, selama kondisi yang dijadikan acuan terpenuhi. Biasanya disiapkan variabel untuk iterasi atau variabel penanda kapan Loop akan diberhentikan.
Di Go keyword Loop hanya **for** saja, tetapi meski demikian, kemampuannya merupakan gabungan for, foreach, dan while ibarat bahasa pemrograman lain.

## 2.9.1. Loop Menggunakan Keyword for

Ada beberapa cara standar menggunakan for. Cara pertama dengan memasukkan variabel counter Loop beserta kondisinya setelah keyword. Perhatikan dan praktekan kode berikut.

```go
for i := 0; i < 5; i++ {
    fmt.Println("Angka", i)
}
```
Loop di atas hanya akan berjalan ketika variabel i bernilai di bawah 5, dengan ketentuan setiap kali Loop, nilai variabel i akan di-iterasi atau ditambahkan 1 (i++ artinya ditambah satu, sama seperti i = i + 1). Karena i pada awalnya bernilai 0, maka Loop akan berlangsung 5 kali, yaitu ketika i bernilai 0, 1, 2, 3, dan 4.

![Enter image alt description](Images/FOd_Image_11.png)

## 2.9.2. Keyword for Dengan Argumen Hanya Kondisi

Cara ke-2 adalah dengan menuliskan kondisi setelah keyword for (hanya kondisi). Deklarasi dan iterasi variabel counter tidak dituliskan setelah keyword, hanya kondisi Loop saja. Konsepnya mirip seperti while milik bahasa pemrograman lain.

Kode berikut adalah contoh for dengan argumen hanya kondisi (seperti if), output yang dihasilkan sama seperti penerapan for cara pertama.

```go
var i = 0

for i < 5 {
    fmt.Println("Angka", i)
    i++
}
```
## 2.9.3. Penggunaan Keyword for Tanpa Argumen

Cara ke-3 adalah for ditulis tanpa kondisi. Dengan ini akan dihasilkan Loop tanpa henti (sama dengan for true). Pemberhentian Loop dilakukan dengan menggunakan keyword break.

```go
var i = 0

for {
    fmt.Println("Angka", i)

    i++
    if i == 5 {
        break
    }
}
```
Dalam Loop tanpa henti di atas, variabel i yang nilai awalnya 0 di-inkrementasi. Ketika nilai i sudah mencapai 5, keyword break digunakan, dan Loop akan berhenti.

## 2.9.4. Penggunaan Keyword for - range

Cara ke-4 adalah Loop dengan menggunakan kombinasi keyword for dan range. Cara ini biasa digunakan untuk me-looping data gabungan (misalnya string, array, slice, map). 

```go
var xs = "123" // string
for i, v := range xs {
    fmt.Println("Index=", i, "Value=", v)
}

var ys = [5]int{10, 20, 30, 40, 50} // array
for _, v := range ys {
    fmt.Println("Value=", v)
}

var zs = ys[0:2] // slice
for _, v := range zs {
    fmt.Println("Value=", v)
}

var kvs = map[byte]int{'a': 0, 'b': 1, 'c': 2} // map
for k, v := range kvs {
    fmt.Println("Key=", k, "Value=", v)
}

for range kvs { // boleh juga baik k dan atau v nya diabaikan
    fmt.Println("Done")
}

// selain itu, bisa juga dengan cukup menentukan nilai numerik Loop
for i := range 5 {
    fmt.Print(i) // 01234
}
```
## 2.9.5. Penggunaan Keyword break & continue

Keyword break digunakan untuk menghentikan secara paksa sebuah Loop, sedangkan continue dipakai untuk memaksa maju ke Loop berikutnya.

Berikut contoh penerapan continue dan break. Kedua keyword tersebut dimanfaatkan untuk menampilkan angka genap berurutan yang lebih besar dari 0 dan kurang dari atau sama dengan 8.

```go
for i := 1; i <= 10; i++ {
    if i % 2 == 1 {
        continue
    }

    if i > 8 {
        break
    }

    fmt.Println("Angka", i)
}
```
Kode di atas akan lebih mudah dicerna jika dijelaskan secara berurutan. Berikut adalah penjelasannya.

1. Dilakukan Loop mulai angka 1 hingga 10 dengan i sebagai variabel iterasi.
2. Ketika i adalah ganjil (dapat diketahui dari i % 2, jika hasilnya 1, berarti ganjil), maka akan dipaksa lanjut ke Loop berikutnya.
3. Ketika i lebih besar dari 8, maka Loop akan berhenti.
4. Nilai i ditampilkan.

![Enter image alt description](Images/H0o_Image_12.png)

## 2.9.6. Loop Bersarang

Tak hanya percabangan yang bisa bersarang, Loop juga bisa. Cara pengaplikasiannya kurang lebih sama, tinggal tulis blok statement Loop di dalam Loop.

```go
for i := 0; i < 5; i++ {
    for j := i; j < 5; j++ {
        fmt.Print(j, " ")
    }

    fmt.Println()
}
```
Pada kode di atas, untuk pertama kalinya fungsi fmt.Println() dipanggil tanpa disisipkan parameter. Cara seperti ini bisa digunakan untuk menampilkan baris baru. Kegunaannya sama seperti output dari statement fmt.Print("\n").

![Enter image alt description](Images/Ztq_Image_13.png)

## 2.9.7. Pemanfaatan Label Dalam Loop

Di Loop bersarang, break dan continue akan berlaku pada blok Loop di mana ia digunakan saja. Ada cara agar kedua keyword ini bisa tertuju pada Loop terluar atau Loop tertentu, yaitu dengan memanfaatkan teknik pemberian **label**.

Program untuk memunculkan matriks berikut merupakan contoh penerapan label Loop.

```go
outerLoop:
for i := 0; i < 5; i++ {
    for j := 0; j < 5; j++ {
        if i == 3 {
            break outerLoop
        }
        fmt.Print("matriks [", i, "][", j, "]", "\n")
    }
}
```
Tepat sebelum keyword for terluar, terdapat baris kode outerLoop:. Maksud dari kode tersebut adalah disiapkan sebuah label bernama outerLoop untuk for di bawahnya. Nama label bisa diganti dengan nama lain (dan harus diakhiri dengan tanda titik dua atau *colon* (:) ).
Pada for bagian dalam, terdapat percabangan untuk pengecekan nilai i. Ketika nilai tersebut sama dengan 3, maka break dipanggil dengan target adalah Loop yang dilabeli outerLoop, Loop tersebut akan dihentikan.

![Enter image alt description](Images/KQy_Image_14.png)

# 2.10 Fungsi 

Dalam konteks pemrograman, fungsi adalah sekumpulan blok kode yang dibungkus dengan nama tertentu. Penerapan fungsi yang tepat akan menjadikan kode lebih modular dan juga *dry* (singkatan dari *don't repeat yourself*) yang artinya kita tidak perlu menuliskan banyak kode untuk kegunaan yang sama berulang kali. Cukup deklarasikan sekali saja blok kode sebagai suatu fungsi, lalu panggil sesuai kebutuhan.

## 2.10.1. Penerapan Fungsi

Sebenarnya kita sudah mengimplementasikan fungsi pada banyak praktek sebelumnya, yaitu fungsi main(). Fungsi main() sendiri merupakan fungsi utama pada program Go, yang akan dieksekusi ketika program dijalankan.
Selain fungsi main(), kita juga bisa membuat fungsi lainnya. Dan caranya cukup mudah, yaitu dengan menuliskan keyword func kemudian diikuti nama fungsi, lalu kurung () (yang bisa diisi parameter), dan diakhiri dengan kurung kurawal untuk membungkus blok kode.
Parameter merupakan variabel yang menempel di fungsi yang nilainya ditentukan saat pemanggilan fungsi tersebut. Parameter sifatnya opsional, suatu fungsi bisa tidak memiliki parameter, atau bisa saja memeliki satu atau banyak parameter (tergantung kebutuhan).
*Data yang digunakan sebagai value parameter saat pemanggilan fungsi biasa disebut dengan argument parameter (atau argument).*

Agar lebih jelas, silakan lihat dan praktekan kode contoh implementasi fungsi berikut ini:

```go
package main

import "fmt"
import "strings"

func main() {
    var names = []string{"Kevin", "Khaslana"}
    printMessage("halo", names)
}

func printMessage(message string, arr []string) {
    var nameString = strings.Join(arr, " ")
    fmt.Println(message, nameString)
}
```
Pada kode di atas, sebuah fungsi baru dibuat dengan nama printMessage() memiliki 2 buah parameter yaitu string message dan slice string arr.

Fungsi tersebut dipanggil dalam main(), dalam pemanggilannya disisipkan dua buah argument parameter.

1. Argument parameter pertama adalah string "halo" yang ditampung parameter message
2. Argument parameter ke-2 adalah slice string names yang nilainya ditampung oleh parameter arr

Di dalam printMessage(), nilai arr yang merupakan slice string digabungkan menjadi sebuah string dengan pembatas adalah karakter **spasi**. Penggabungan slice dapat dilakukan dengan memanfaatkan fungsi strings.Join() (berada di dalam package strings).

![Enter image alt description](Images/EJ3_Image_15.png)

## 2.10.2. Fungsi Dengan Return Value / Nilai Balik

Selain parameter, fungsi bisa memiliki attribute **return value** atau nilai balik. Fungsi yang memiliki return value, saat deklarasinya harus ditentukan terlebih dahulu tipe data dari nilai baliknya. *Fungsi yang tidak mengembalikan nilai apapun (contohnya seperti fungsi **main()** dan **printMessage()**) biasa disebut dengan ****void function***

Program berikut merupakan contoh penerapan fungsi yang memiliki return value.

```go
package main

import (
    "fmt"
    "math/rand"
    "time"
)

var randomizer = rand.New(rand.NewSource(time.Now().Unix()))

func main() {
    var randomValue int

    randomValue = randomWithRange(2, 10)
    fmt.Println("random number:", randomValue)

    randomValue = randomWithRange(2, 10)
    fmt.Println("random number:", randomValue)

    randomValue = randomWithRange(2, 10)
    fmt.Println("random number:", randomValue)
}

func randomWithRange(min, max int) int {
    var value = randomizer.Int()%(max-min+1) + min
    return value
}
```
Fungsi randomWithRange() didesain untuk *generate* angka acak sesuai dengan range yang ditentukan lewat parameter, yang kemudian angka tersebut dijadikan nilai balik fungsi.

![Enter image alt description](Images/1i1_Image_16.png)

Cara menentukan tipe data nilai balik fungsi adalah dengan menuliskan tipe data yang diinginkan setelah kurung parameter. Bisa dilihat pada kode di atas, bahwa int merupakan tipe data nilai balik fungsi randomWithRange().
```go
func randomWithRange(min, max int) int
```
Sedangkan cara untuk mengembalikan nilai itu sendiri adalah dengan menggunakan keyword return diikuti data yang dikembalikan. Pada contoh di atas, return value artinya nilai variabel value dijadikan nilai kembalian fungsi.
Eksekusi keyword return akan menjadikan proses dalam blok fungsi berhenti pada saat itu juga. Semua statement setelah keyword tersebut tidak akan dieksekusi.
Dari kode di atas mungkin ada beberapa hal yang belum pernah kita lakukan pada pembahasan-pembahasan sebelumnya, kita akan bahas satu-persatu.

## 2.10.3. Penggunaan Fungsi rand.New()

Fungsi rand.New() digunakan untuk membuat object randomizer, yang dari object tersebut kita bisa mendapatkan nilai random/acak hasil generator. Dalam penerapannya, fungsi rand.New() membutuhkan argument yaitu random source seed, yang bisa kita buat lewat statement rand.NewSource(time.Now().Unix()).
```go
var randomizer = rand.New(rand.NewSource(time.Now().Unix()))
```
*Dalam penggunaan fungsi **rand.NewSource()**, argument bisa diisi dengan nilai apapun, salah satunya adalah **time.Now().Unix()**.*

Fungsi rand.New() berada dalam package math/rand. Package tersebut harus di-import terlebih dahulu sebelum bisa menggunakan fungsi-fungsi yang ada didalamnya. Package time juga perlu di-import karena di contoh ini fungsi (time.Now().Unix()) digunakan.

## 2.10.4. Import Banyak Package

Penulisan keyword import untuk banyak package bisa dilakukan dengan dua cara, dengan menuliskannya di tiap package, atau cukup sekali saja, bebas silakan pilih sesuai selera.

```go
import "fmt"
import "math/rand"
import "time"

// atau

import (
    "fmt"
    "math/rand"
    "time"
)
```
## 2.10.5. Deklarasi Parameter Bertipe Data Sama

Khusus untuk fungsi yang tipe data parameternya sama, bisa ditulis dengan gaya yang unik. Tipe datanya dituliskan cukup sekali saja di akhir. Contohnya bisa dilihat pada kode berikut.

```go
func nameOfFunc(paramA type, paramB type, paramC type) returnType
func nameOfFunc(paramA, paramB, paramC type) returnType

func randomWithRange(min int, max int) int
func randomWithRange(min, max int) int
```
## 2.10.6. Penggunaan Keyword return Untuk Menghentikan Proses Dalam Fungsi

Selain sebagai penanda nilai balik, keyword return juga bisa dimanfaatkan untuk menghentikan proses dalam blok fungsi di mana ia ditulis. Contohnya bisa dilihat pada kode berikut.

```go
package main

import "fmt"

func main() {
    divideNumber(10, 2)
    divideNumber(4, 0)
    divideNumber(8, -4)
}

func divideNumber(m, n int) {
    if n == 0 {
        fmt.Printf("invalid divider. %d cannot divided by %d\n", m, n)
        return
    }

    var res = m / n
    fmt.Printf("%d / %d = %d\n", m, n, res)
}
```
Fungsi divideNumber() dirancang tidak memiliki nilai balik. Fungsi ini dibuat untuk membungkus proses pembagian 2 bilangan, lalu menampilkan hasilnya.

Di dalamnya terdapat proses validasi nilai variabel pembagi, jika nilainya adalah 0, maka akan ditampilkan pesan bahwa pembagian tidak bisa dilakukan, lalu proses dihentikan pada saat itu juga (dengan memanfaatkan keyword return). Jika nilai pembagi valid, maka proses pembagian diteruskan.

![Enter image alt description](Images/mNV_Image_17.png)

# 2.11 Pointer

Pointer adalah *reference* atau alamat memori. Variabel pointer berarti variabel yang berisi alamat memori suatu nilai. Sebagai contoh sebuah variabel bertipe integer memiliki nilai **4**, maka yang dimaksud pointer adalah **alamat memori di mana nilai 4 disimpan**, bukan nilai 4 itu sendiri.

Variabel-variabel yang memiliki *reference* atau alamat pointer yang sama, saling berhubungan satu sama lain dan nilainya pasti sama. Ketika ada perubahan nilai, maka akan memberikan efek kepada variabel lain (yang referensi-nya sama) yaitu nilainya ikut berubah.

## 2.11.1. Penerapan Pointer

Variabel bertipe pointer ditandai dengan adanya tanda **asterisk** (*) tepat sebelum penulisan tipe data ketika deklarasi.
```go
var number *int

var name *string
```
Nilai default variabel pointer adalah nil (kosong). Variabel pointer tidak bisa menampung nilai yang bukan pointer, dan sebaliknya variabel biasa tidak bisa menampung nilai pointer.

Ada dua hal penting yang perlu diketahui mengenai pointer:

- Variabel biasa bisa diambil nilai pointernya, caranya dengan menambahkan tanda **ampersand** (&) tepat sebelum nama variabel. Metode ini disebut dengan **referencing**.
- Dan sebaliknya, nilai asli variabel pointer juga bisa diambil, dengan cara menambahkan tanda **asterisk** (*) tepat sebelum nama variabel. Metode ini disebut dengan **dereferencing**.

```go
var numberA int = 4
var numberB *int = &numberA

fmt.Println("numberA (value)   :", numberA)  // 4
fmt.Println("numberA (address) :", &numberA) // 0xc20800a220

fmt.Println("numberB (value)   :", *numberB) // 4
fmt.Println("numberB (address) :", numberB)  // 0xc20800a220
```
Variabel numberB dideklarasikan bertipe pointer int dengan nilai awal adalah referensi variabel numberA (bisa dilihat pada kode &numberA). Dengan ini, variabel numberA dan numberB menampung data dengan referensi alamat memori yang sama.

![Enter image alt description](Images/Qke_Image_18.png)

Variabel pointer jika di-print akan menghasilkan string alamat memori (dalam notasi heksadesimal), contohnya seperti numberB yang diprint menghasilkan 0xc20800a220.

Nilai asli sebuah variabel pointer bisa didapatkan dengan cara di-dereference terlebih dahulu (bisa dilihat pada kode *numberB).

## 2.11.2. Efek Perubahan Nilai Pointer

Ketika salah satu variabel pointer di ubah nilainya, sedang ada variabel lain yang memiliki referensi memori yang sama, maka nilai variabel lain tersebut juga akan berubah.

Variabel numberA dan numberB memiliki referensi memori yang sama. Perubahan pada salah satu nilai variabel tersebut akan memberikan efek pada variabel lainnya. Pada contoh di atas, numberA nilainya di ubah menjadi 5. membuat nilai asli variabel numberB ikut berubah menjadi 5.

![Enter image alt description](Images/fhC_Image_19.png)

## 2.11.3. Parameter Pointer

Parameter bisa juga dirancang sebagai pointer. Cara penerapannya kurang lebih sama, dengan cara mendeklarasikan parameter sebagai pointer.

```go
package main
import "fmt"

func main() {
    var number = 4
    fmt.Println("before :", number) // 4
    change(&number, 10)
    fmt.Println("after  :", number) // 10
}
func change(original *int, value int) {
    *original = value
}
```
Fungsi change() memiliki 2 parameter, yaitu original yang tipenya adalah pointer int, dan value yang bertipe int. Di dalam fungsi tersebut nilai asli parameter pointer original diubah.
Fungsi change() kemudian diimplementasikan di main. Variabel number yang nilai awalnya adalah 4 diambil referensi-nya lalu digunakan sebagai parameter pada pemanggilan fungsi change().
Nilai variabel number berubah menjadi 10 karena perubahan yang terjadi di dalam fungsi change adalah pada variabel pointer.

![Enter image alt description](Images/kHR_Image_20.png)

# 2.12 Struct & Interface

## A. Struct

Go tidak mengadopsi konsep class seperti pada beberapa bahasa pemrograman OOP lainnya. Namun Go memiliki tipe data struktur Struct.

Struct adalah kumpulan definisi variabel (atau property) dan atau fungsi (atau method), yang dibungkus sebagai tipe data baru dengan nama tertentu. Property dalam struct, tipe datanya bisa bervariasi. Mirip seperti map, hanya saja key-nya sudah didefinisikan di awal, dan tipe data tiap itemnya bisa berbeda.

Dari sebuah struct, kita bisa buat variabel baru, yang memiliki atribut sesuai skema struct tersebut. Kita sepakati dalam buku ini, variabel tersebut dipanggil dengan istilah **object** atau **variabel object**.

*Konsep struct di golang mirip dengan konsep ****class**** pada OOP, meski sebenarnya memiliki perbedaan. Di sini penulis menggunakan konsep OOP sebagai analogi, untuk mempermudah pembaca untuk memahami pembelajaran di chapter ini.*

Dengan memanfaatkan struct, penyimpanan data yang sifatnya kolektif menjadi lebih mudah, lebih rapi, dan mudah untuk dikelola.

### 2.12.A.1. Deklarasi Struct

Kombinasi keyword type dan struct digunakan untuk deklarasi struct. Di bawah ini merupakan contoh cara penerapannya.
```go
type student struct {
    name string
    grade int
}
```
Struct student dideklarasikan memiliki 2 property, yaitu name dan grade. Property adalah istilah untuk variabel yang menempel ke struct.

### 2.12.A.2. Penerapan Struct Untuk Membuat Object

Struct student yang sudah disiapkan di atas kita gunakan untuk membuat variabel objek. Variabel tersebut tipe datanya adalah student. Kemudian dari variabel object, kita bisa mengakses isi property variabel. Contoh:

```go
func main() {
    var s1 student
    s1.name = "Kevin Khaslana"
    s1.grade = 2

    fmt.Println("name  :", s1.name)
    fmt.Println("grade :", s1.grade)
}
```
Cara membuat variabel objek sama seperti pembuatan variabel biasa. Tinggal tulis saja nama variabel diikuti nama struct, contoh: var s1 student.

Semua property variabel objek pada awalnya memiliki zero value sesuai tipe datanya. Misalnya, 0 untuk tipe int, dan string kosong "" untuk string.

Property variabel objek bisa diakses nilainya menggunakan notasi titik, contohnya s1.name. Nilai property-nya juga bisa diubah, contohnya s1.grade = 2.

![Enter image alt description](Images/F70_Image_21.png)

### 2.12.A.3. Inisialisasi Object Struct

Cara inisialisasi variabel objek adalah dengan menuliskan nama struct yang telah dibuat diikuti dengan kurung kurawal. Nilai masing-masing property bisa diisi pada saat inisialisasi.

Pada contoh berikut, terdapat 3 buah variabel objek yang dideklarasikan dengan cara berbeda.

```go
var s1 = student{}
s1.name = "KEvin"
s1.grade = 2

var s2 = student{"ethan", 2}

var s3 = student{name: "jason"}

fmt.Println("student 1 :", s1.name)
fmt.Println("student 2 :", s2.name)
fmt.Println("student 3 :", s3.name)
```
Pada kode di atas, variabel s1 menampung objek cetakan student. Variabel tersebut kemudian di-set nilai property-nya.

Variabel objek s2 dideklarasikan dengan metode yang sama dengan s1, pembedanya di s2 nilai propertinya di isi langsung ketika deklarasi. Nilai pertama akan menjadi nilai property pertama (yaitu name), dan selanjutnya berurutan.

Pada deklarasi s3, dilakukan juga pengisian property ketika pencetakan objek. Hanya saja, yang diisi hanya name saja. Cara ini cukup efektif jika digunakan untuk membuat objek baru yang nilai property-nya tidak semua harus disiapkan di awal. Keistimewaan lain menggunakan cara ini adalah penentuan nilai property bisa dilakukan dengan tidak berurutan. Contohnya:
```go
var s4 = student{name: "wayne", grade: 2}
var s5 = student{grade: 2, name: "bruce"}
```
### 2.12.A.4. Variabel Objek Pointer

Objek yang dibuat dari tipe struct bisa diambil nilai pointer-nya, dan bisa disimpan pada variabel objek yang bertipe struct pointer. Contoh penerapannya:

```go
var s1 = student{name: "KEvin", grade: 2}

var s2 *student = &s1
fmt.Println("student 1, name :", s1.name)
fmt.Println("student 4, name :", s2.name)

s2.name = "Khaslana"
fmt.Println("student 1, name :", s1.name)
fmt.Println("student 4, name :", s2.name)
```
s2 adalah variabel pointer hasil cetakan struct student. s2 menampung nilai referensi s1, menjadikan setiap perubahan pada property variabel tersebut, akan juga berpengaruh pada variabel objek s1.

Meskipun s2 bukan variabel asli, property nya tetap bisa diakses seperti biasa. Inilah keistimewaan property dalam objek pointer, tanpa perlu di-dereferensi nilai asli property tetap bisa diakses. Pengisian nilai pada property tersebut juga bisa langsung menggunakan nilai asli, contohnya seperti s2.name = "Khaslana”

![Enter image alt description](Images/mec_Image_22.png)

### 2.12.A.5. Embedded Struct

**Embedded** struct adalah mekanisme untuk menempelkan sebuah struct sebagai properti struct lain. Agar lebih mudah dipahami, mari kita bahas kode berikut

```go
package main

import "fmt"

type person struct {
    name string
    age  int
}

type student struct {
    grade int
    person
}

func main() {
    var s1 = student{}
    s1.name = "KEVBIN"
    s1.age = 21
    s1.grade = 2

    fmt.Println("name  :", s1.name)
    fmt.Println("age   :", s1.age)
    fmt.Println("age   :", s1.person.age)
    fmt.Println("grade :", s1.grade)
}
```
Pada kode di atas, disiapkan struct person dengan properti yang tersedia adalah name dan age. Disiapkan juga struct student dengan property grade. Struct person di-embed ke dalam struct student. Caranya cukup mudah, yaitu dengan menuliskan nama struct yang ingin di-embed ke dalam body struct target.

Embedded struct adalah **mutable**, nilai property-nya nya bisa diubah.

Khusus untuk properti yang bukan merupakan properti asli (melainkan properti turunan dari struct lain), pengaksesannya dilakukan dengan cara mengakses struct *parent*-nya terlebih dahulu, contohnya s1.person.age. Nilai yang dikembalikan memiliki referensi yang sama dengan s1.age.

### 2.12.A.6. Embedded Struct Dengan Nama Property Yang Sama

Jika salah satu nama properti sebuah struct memiliki kesamaan dengan properti milik struct lain yang di-embed, maka pengaksesan property-nya harus dilakukan secara eksplisit atau jelas. Silakan lihat kode berikut agar lebih jelas.

```go
package main

import "fmt"

type person struct {
    name string
    age  int
}

type student struct {
    person
    age   int
    grade int
}

func main() {
    var s1 = student{}
    s1.name = "KEVIN"
    s1.age = 21        // age of student
    s1.person.age = 22 // age of person

    fmt.Println(s1.name)
    fmt.Println(s1.age)
    fmt.Println(s1.person.age)
}
```
Struct person di-embed ke dalam struct student, dan kedua struct tersebut kebetulan salah satu nama property-nya ada yang sama, yaitu age. Cara mengakses property age milik struct person lewat objek struct student, adalah dengan menuliskan nama struct yang di-embed kemudian nama property-nya, contohnya: s1.person.age = 22

### 2.12.A.7. Pengisian Nilai Sub-Struct

Pengisian nilai property sub-struct bisa dilakukan dengan langsung memasukkan variabel objek yang tercetak dari struct yang sama.

```go
var p1 = person{name: "Kevin", age: 21}
var s1 = student{person: p1, grade: 2}

fmt.Println("name  :", s1.name)
fmt.Println("age   :", s1.age)
fmt.Println("grade :", s1.grade)
```
### 2.12.A.8. Anonymous Struct

Anonymous struct adalah struct yang tidak dideklarasikan di awal sebagai tipe data baru, melainkan langsung ketika pembuatan objek. Teknik ini cukup efisien digunakan pada *use case* pembuatan variabel objek yang struct-nya hanya dipakai sekali.

```go
package main

import "fmt"

type person struct {
    name string
    age  int
}

func main() {
    var s1 = struct {
        person
        grade int
    }{}
    s1.person = person{"Kevin", 21}
    s1.grade = 2

    fmt.Println("name  :", s1.person.name)
    fmt.Println("age   :", s1.person.age)
    fmt.Println("grade :", s1.grade)
}
```
Pada kode di atas, variabel s1 langsung diisi objek anonymous struct yang memiliki property grade, dan property person yang merupakan embedded struct.

Salah satu aturan yang perlu diingat dalam pembuatan anonymous struct adalah, deklarasi harus diikuti dengan inisialisasi. Bisa dilihat pada s1 setelah deklarasi struktur struct, terdapat kurung kurawal untuk inisialisasi objek. Meskipun nilai tidak diisikan di awal, kurung kurawal tetap harus ditulis.

```go
var s1 = struct {
    person
    grade int
}{}

// anonymous struct dengan pengisian property
var s2 = struct {
    person
    grade int
}{
    person: person{"Kebin", 21},
    grade:  2,
}
```
### 2.12.A.9. Kombinasi Slice & Struct

Slice dan struct bisa dikombinasikan seperti pada slice dan map, caranya penggunaannya-pun mirip, cukup tambahkan tanda [] sebelum tipe data pada saat deklarasi.

```go
type person struct {
    name string
    age  int
}

var allStudents = []person{
    {name: "Kevin", age: 23},
    {name: "Khaslana", age: 23},
    {name: "Phainon", age: 22},
}

for _, student := range allStudents {
    fmt.Println(student.name, "age is", student.age)
}
```
### 2.12.A.10. Inisialisasi Slice Anonymous Struct

Anonymous struct bisa dijadikan sebagai tipe sebuah slice. Dan nilai awalnya juga bisa diinisialisasi langsung pada saat deklarasi. Berikut adalah contohnya:

```go
var allStudents = []struct {
    person
    grade int
}{
    {person: person{"wick", 21}, grade: 2},
    {person: person{"ethan", 22}, grade: 3},
    {person: person{"bond", 21}, grade: 3},
}

for _, student := range allStudents {
    fmt.Println(student)
}
```
### 2.12.A.11. Deklarasi Anonymous Struct Menggunakan Keyword var

Cara lain untuk deklarasi anonymous struct adalah dengan menggunakan keyword var.

```go
var student struct {
    person
    grade int
}

student.person = person{"Kevin", 21}
student.grade = 2
```
Statement type student struct adalah contoh cara deklarasi struct. Maknanya akan berbeda ketika keyword type diganti var, seperti pada contoh di atas var student struct, yang artinya dicetak sebuah objek dari anonymous struct kemudian disimpan pada variabel bernama student. Deklarasi anonymous struct menggunakan metode ini juga bisa dilakukan dengan disertai inisialisasi data.

```go
// hanya deklarasi
var student struct {
    grade int
}

// deklarasi sekaligus inisialisasi
var student = struct {
    grade int
} {
    12,
}
```
### 2.12.A.12. Nested struct

Nested struct adalah anonymous struct yang di-embed ke sebuah struct. Deklarasinya langsung di dalam struct peng-embed. Contoh:

```go
type student struct {
    person struct {
        name string
        age  int
    }
    grade   int
    hobbies []string
}
```
Teknik ini biasa digunakan ketika decoding data **JSON** yang struktur datanya cukup kompleks dengan proses decode hanya sekali.

### 2.12.A.13. Deklarasi Dan Inisialisasi Struct Secara Horizontal

Deklarasi struct bisa dituliskan secara horizontal, caranya bisa dilihat pada kode berikut:
```go
type person struct { name string; age int; hobbies []string }
```
Tanda semi-colon (;) digunakan sebagai pembatas deklarasi poperty yang dituliskan secara horizontal. Inisialisasi nilai juga bisa dituliskan dengan metode ini. Contohnya:
```go
var p1 = struct { name string; age int } { age: 22, name: "Kevin" }
var p2 = struct { name string; age int } { "ethan", 23 }
```
### 2.12.A.14. Tag property dalam struct

Tag merupakan informasi opsional yang bisa ditambahkan pada property struct.
```go
type person struct {
  name string <code>tag1</code>
  age  int    <code>tag2</code>
}
```
Tag biasa dimanfaatkan untuk keperluan encode/decode data. Informasi tag juga bisa diakses lewat reflect. Nantinya akan ada pembahasan yang lebih detail mengenai pemanfaatan tag dalam struct, terutama ketika sudah masuk chapter JSON

### 2.12.A.15. Type Alias

Sebuah tipe data, seperti struct, bisa dibuatkan alias baru, caranya dengan type NamaAlias = TargetStruct. Contoh:

```go
type Person struct {
    name string
    age  int
}
type People = Person

var p1 = Person{"KEvin", 21}
fmt.Println(p1)
var p2 = People{"KEvin", 21}
fmt.Println(p2)
```
Pada kode di atas, sebuah alias bernama People dibuat untuk struct Person. Casting dari objek (yang dicetak lewat struct tertentu) ke tipe yang merupakan alias dari struct pencetak, hasilnya selalu valid. Berlaku juga sebaliknya.
```go
people := People{"KEvin", 21}
fmt.Println(Person(people))
person := Person{"KEvin", 21}
fmt.Println(People(person))
```
Pembuatan struct baru juga bisa dilakukan lewat teknik type alias. Silakan perhatikan kode berikut.

```go
type People1 struct {
    name string
    age  int
}
type People2 = struct {
    name string
    age  int
}
```
Struct People1 dideklarasikan, kemudian struct alias People2 juga dideklarasikan. Struct People2 merupakan alias dari anonymous struct. Penggunaan teknik type alias untuk anonymous struct menghasilkan output yang ekuivalen dengan pendeklarasian struct. 

Teknik type alias ini tidak dirancang hanya untuk pembuatan alias pada tipe struct saja, semua jenis tipe data bisa dibuatkan alias. Contohnya seperti pada kode berikut, ada tipe data baru bernama Number yang merupakan alias dari tipe data int.
```go
type Number = int
var num Number = 12
```
## B. Interface

Interface adalah definisi suatu kumpulan method yang tidak memiliki isi, jadi hanya definisi header/schema-nya saja. Kumpulan method tersebut ditulis dalam satu block interface dengan nama tertentu.

Interface merupakan tipe data. Objek bertipe interface memiliki zero value yaitu nil. Variabel bertipe interface digunakan untuk menampung nilai objek konkret yang memiliki definisi method minimal sama dengan yang ada di interface.

### 2.12.B.1. Penerapan Interface

Untuk menerapkan interface, pertama siapkan deklarasi tipe baru menggunakan keyword type dan tipe data interface lalu siapkan juga isinya (definisi method-nya).

```go
package main

import "fmt"
import "math"

type hitung interface {
    luas() float64
    keliling() float64
}
```
Di atas, interface hitung dideklarasikan memiliki 2 buah method yaitu luas() dan keliling(). Interface ini nantinya digunakan sebagai tipe data pada variabel untuk menampung objek bangun datar hasil dari struct yang akan dibuat.

Dengan adanya interface hitung ini, maka perhitungan luas dan keliling bangun datar bisa dilakukan tanpa perlu tahu jenis bangun datarnya sendiri itu apa.

Selanjutnya, siapkan struct bangun datar lingkaran, struct ini memiliki definisi method yang sebagian adalah ada di interface hitung.

```go
type lingkaran struct {
    diameter float64
}

func (l lingkaran) jariJari() float64 {
    return l.diameter / 2
}

func (l lingkaran) luas() float64 {
    return math.Pi * math.Pow(l.jariJari(), 2)
}

func (l lingkaran) keliling() float64 {
    return math.Pi * l.diameter
}
```
Struct lingkaran memiliki tiga buah method yaitu jariJari(), luas(), dan keliling(). Berikutnya, siapkan struct bangun datar persegi berikut:

```go
type persegi struct {
    sisi float64
}

func (p persegi) luas() float64 {
    return math.Pow(p.sisi, 2)
}

func (p persegi) keliling() float64 {
    return p.sisi * 4
}
```
Perbedaan struct persegi dengan lingkaran terletak pada method jariJari(). Struct persegi tidak memiliki method tersebut. Tetapi meski demikian, variabel objek hasil cetakan 2 struct ini akan tetap bisa ditampung oleh variabel cetakan interface hitung, karena dua method yang ter-definisi di interface tersebut juga ada pada struct persegi dan lingkaran, yaitu method luas() dan keliling(). Sekarang buat implementasi perhitungan di fungsi main().

```go
func main() {
    var bangunDatar hitung

    bangunDatar = persegi{10.0}
    fmt.Println("===== persegi")
    fmt.Println("luas      :", bangunDatar.luas())
    fmt.Println("keliling  :", bangunDatar.keliling())

    bangunDatar = lingkaran{14.0}
    fmt.Println("===== lingkaran")
    fmt.Println("luas      :", bangunDatar.luas())
    fmt.Println("keliling  :", bangunDatar.keliling())
    fmt.Println("jari-jari :", bangunDatar.(lingkaran).jariJari())
}
```
Perhatikan kode di atas. Variabel objek bangunDatar bertipe interface hitung. Variabel tersebut digunakan untuk menampung objek konkrit buatan struct lingkaran dan persegi.

Dari variabel tersebut, method luas() dan keliling() diakses. Secara otomatis Golang akan mengarahkan pemanggilan method pada interface ke method asli milik struct yang bersangkutan.

![Enter image alt description](Images/5EF_Image_23.png)

Method jariJari() pada struct lingkaran tidak akan bisa diakses karena tidak terdefinisi dalam interface hitung. Pengaksesannya secara paksa menyebabkan error.

Untuk mengakses method yang tidak ter-definisi di interface, variabel-nya harus di-casting terlebih dahulu ke tipe asli variabel konkritnya (pada kasus ini tipenya lingkaran), setelahnya method akan bisa diakses.

Cara casting objek interface sedikit unik, yaitu dengan menuliskan nama tipe tujuan dalam kurung, ditempatkan setelah nama interface dengan menggunakan notasi titik (seperti cara mengakses property, hanya saja ada tanda kurung nya). Contohnya bisa dilihat di kode berikut. Statement bangunDatar.(lingkaran) adalah contoh casting pada objek interface.
```go
var bangunDatar hitung = lingkaran{14.0}
var bangunLingkaran lingkaran = bangunDatar.(lingkaran)
bangunLingkaran.jariJari()
```
*Metode casting pada tipe data interface biasa disebut dengan ****type assertion. ***Perlu diketahui juga, jika ada interface yang menampung objek konkrit yang mana struct-nya tidak memiliki salah satu method yang terdefinisi di interface, maka error akan muncul. Intinya kembali ke aturan awal, variabel interface hanya bisa menampung objek yang minimal memiliki semua method yang terdefinisi di interface tersebut.

### 2.12.B.2. Embedded Interface

Interface bisa di-embed ke interface lain, sama seperti struct. Cara penerapannya juga sama, cukup dengan menuliskan nama interface yang ingin di-embed ke dalam body interface tujuan.

Pada contoh berikut, disiapkan interface bernama hitung2d dan hitung3d. Kedua interface tersebut kemudian di-embed ke interface baru bernama hitung.

```go
package main

import "fmt"
import "math"

type hitung2d interface {
    luas() float64
    keliling() float64
}

type hitung3d interface {
    volume() float64
}

type hitung interface {
    hitung2d
    hitung3d
} 
```
Interface hitung2d berisikan method untuk kalkulasi luas dan keliling, sedang hitung3d berisikan method untuk mencari volume bidang. Kedua interface tersebut embed ke interface hitung, menjadikannya memiliki kemampuan untuk mengakses method luas(), keliling(), dan volume().

Next, siapkan struct baru bernama kubus yang memiliki method luas(), keliling(), dan volume().

```go
type kubus struct {
    sisi float64
}

func (k *kubus) volume() float64 {
    return math.Pow(k.sisi, 3)
}

func (k *kubus) luas() float64 {
    return math.Pow(k.sisi, 2) * 6
}

func (k *kubus) keliling() float64 {
    return k.sisi * 12
}
```
Objek hasil cetakan struct kubus di atas, nantinya akan ditampung oleh objek cetakan interface hitung yang isinya merupakan gabungan interface hitung2d dan hitung3d.

Terakhir, buat implementasi-nya di fungsi main().

```go
func main() {
    var bangunRuang hitung = &kubus{4}

    fmt.Println("===== kubus")
    fmt.Println("luas      :", bangunRuang.luas())
    fmt.Println("keliling  :", bangunRuang.keliling())
    fmt.Println("volume    :", bangunRuang.volume())
}
```
Bisa dilihat di kode di atas, lewat interface hitung, method luas(), keliling(), dan volume() bisa di akses.

Method pointer bisa diakses lewat variabel objek biasa dan variabel objek pointer. Variabel objek yang dicetak menggunakan struct yang memiliki method pointer, jika ditampung ke dalam variabel interface, harus diambil referensi-nya terlebih dahulu. Contohnya bisa dilihat pada kode di atas var bangunRuang hitung = &kubus{4}.

![Enter image alt description](Images/2wu_Image_24.png)

# Bab 5: Database & GORM

# 5.1 Jenis-jenis Database (Relasional dan Non Relasional)

Secara umum, database dibagi menjadi dua jenis utama, yaitu **database relasional** dan **database non relasional (NoSQL)**.

#### **1. Database Relasional**

Database relasional menyimpan data dalam bentuk tabel dan menggunakan **SQL (Structured Query Language)** untuk mengelola data. Setiap tabel memiliki baris dan kolom yang terstruktur dengan relasi antar tabel.

Contoh: MySQL, PostgreSQL, SQLite, SQL Server

#### **2. Database Non Relasional (NoSQL)**

Database non relasional menyimpan data dalam format yang lebih fleksibel, seperti dokumen, key-value, graf, atau kolom lebar. Biasanya tidak menggunakan skema tetap dan tidak menggunakan SQL.

Contoh: MongoDB (document-based), Redis (key-value), Cassandra (wide-column), Neo4j (graph-based).

# 5.2 Definisi ORM

ORM (Object-Relational Mapping) dalam konteks Go adalah sebuah teknik yang memungkinkan pengembang untuk berinteraksi dengan database menggunakan objek-objek dalam bahasa pemrograman, tanpa harus menulis query SQL secara langsung. Dalam Go, ORM seperti GORM (Go ORM) menyediakan berbagai fungsi untuk mempermudah operasi database seperti membuat, membaca, memperbarui, dan menghapus data (CRUD) serta mengelola relasi antar tabel.

# 5.3 Declaring model 

Model didefinisikan menggunakan struct biasa. Struct ini dapat berisi field dengan tipe dasar Go, pointer atau alias dari tipe itu, atau bahkan tipe custom, selama tetap mengimplementasikan Scanner dan Valuer interface dari database/sql package. Perhatikan contoh model User dibawah ini:

```go
type User struct {
  ID           uint           // Standard field for the primary key
  Name         string         // A regular string field
  Email        *string        // A pointer to a string, allowing for null values
  Age          uint8          // An unsigned 8-bit integer
  Birthday     *time.Time     // A pointer to time.Time, can be null
  MemberNumber sql.NullString // Uses sql.NullString to handle nullable strings
  ActivatedAt  sql.NullTime   // Uses sql.NullTime for nullable time fields
  CreatedAt    time.Time      // Automatically managed by GORM for creation time
  UpdatedAt    time.Time      // Automatically managed by GORM for update time
  ignored      string         // fields that aren't exported are ignored
}
```
Dalam model ini:

1. Tipe data dasar seperti uint, string, dan uint8 digunakan secara langsung.

2. Sebuah pointer ke tipe seperti *string dan *time.Time mengindikasikan bidang yang dapat bernilai null.

3. sql.NullString dan sql.NullTime dari package database/sql digunakan untuk bidang yang dapat bernilai null dengan kontrol yang lebih luas.

4. CreatedAt dan UpdatedAt adalah bidang khusus yang secara otomatis diisi oleh GORM dengan waktu saat ini ketika record dibuat atau diperbarui.

5. Ignore adalah field yang berisi hal yang ingin diabaikan (tidak dimasukkan)

## 5.3.1 Convention

1. **Primary Key**: GORM menggunakan field bernama ID sebagai primary key default untuk setiap model.

```go
type User struct {
  ID   string // field named `ID` used as a primary field by default
  Name string
}
```
Namun, bisa juga menggunakan nama selain ‘ID’ dengan cara menambahkan tag “primaryKey” seperti berikut

```go
// Set field `UUID` as primary field
type Animal struct {
  ID     int64
  UUID   string `gorm:"primaryKey"`
  Name   string
  Age    int64
}
```
2. **Table Names**: Secara default, GORM akan mengubah nama dari sebuah struct menjadi style  snake_case. Sebagai contoh nama struct User akan menjadi users di dalam database, dan nama seperti  GormUserName akan menjadi  gorm_user_names. Namun, hal ini juga bisa dilakukan secara manual jika kita ingin dengan menggunakan Tabler interface, seperti berikut

```go
type Tabler interface {
  TableName() string
}

// TableName overrides the table name used by User to `profiles`
func (User) TableName() string {
  return "profiles"
}
```
TableName ini tidak mengizinkan pengubahan nama secara dynamic, hasilnya hanya berupa cache yang akan digunakan di masa depan, jika ingin dynamic bisa menggunakan [Scope](https://gorm.io/docs/conventions.html). GORM mengizinkan user untuk membuat nama custom menggunakan NamingStrategy, yang digunakan seperti TableName, ColumnName, JoinTableName, RelationshipFKName, CheckerName, IndexName.

3. **Column Names**: GORM juga secara otomatis mengubah nama field menjadi style snake_case untuk nama kolom di database. Kita bisa melakukan kustomisasi nama menggunakan NamingStrategy maupun menggunakan tag seperti dibawah ini
```go
type Animal struct {
  AnimalID int64     <code>gorm:"column:beast_id"</code>        
  Birthday time.Time <code>gorm:"column:day_of_the_beast"</code> 
  Age      int64     <code>gorm:"column:age_of_the_beast"</code> 
}
```
**Timestamp Fields**: GORM menggunakan field bernama CreatedAt dan UpdatedAt untuk mendeteksi secara otomatis kapan terjadinya creation dan update di dalam records. Timestamps otomatis ini juga dapat dimatikan jika kita menginginkannya menggunakan tag seperti dibawah ini 
```go
type User struct{

  CreatedAt time.Time gorm:"autoCreateTime:false"
}
```
Syntax diatas juga berlaku untuk UpdateAt. Dengan mengikuti konvensi ini kita dapat mengurangi jumlah konfigurasi atau kode yang harus kita buat.

5.3.2 gorm.Model

GORM menyediakan sebuah struktur bawaan bernama gorm.Model yang sudah mencakup field (kolom) yang umum digunakan:

```go
// definisi gorm.Model
type Model struct {
  ID        uint           `gorm:"primaryKey"`
  CreatedAt time.Time
  UpdatedAt time.Time
  DeletedAt gorm.DeletedAt `gorm:"index"`
}
```
Kamu bisa langsung menyisipkan gorm.Model ke dalam struct milikmu agar field-field tersebut otomatis tersedia. Contoh cara penyisipan adalah

```go
type Author struct {
    Name  string
    Email string
}

type Blog struct {
  ID      int
  Author  Author `gorm:"embedded"`
  Upvotes int32
}
// equals
type Blog struct {
    ID    int64
    Name  string
    Email string
    Upvotes  int32
}
```

Cara diatas dapat diterapkan juga untuk gorm.Model cukup sisipkan “gorm.Model” dan itu akan langsung menambahkan field: ID, CreatedAt, UpdatedAt, DeletedAt dalam struct. Ini sangat berguna untuk menjaga konsistensi di antara berbagai model dan juga memanfaatkan aturan otomatis dari GORM. Penjelasan Field:

**ID**: Penanda unik untuk tiap data (sebagai *primary key*).

**CreatedAt**: Otomatis diisi dengan waktu saat data dibuat.

**UpdatedAt**: Otomatis diperbarui dengan waktu saat data diubah.

**DeletedAt**: Digunakan untuk *soft delete*, yaitu menandai data sebagai terhapus tanpa benar-benar menghapus dari database. \


# 5.4 Koneksi ke Database

GORM secara resmi mendukung beberapa basis data seperti MySQL, PostgreSQL, GaussDB, SQLite, SQL Server, TiDB. Dibawah ini contoh cara koneksi ke beberapa database

## 1. MySQL

```go
import (
    "gorm.io/driver/mysql"
    "gorm.io/gorm"
)


func main() {
//Lihat https://github.com/go-sql-driver/mysql#dsn-data-source-name untuk detail format koneksi
    dsn := "user:pass@tcp(127.0.0.1:3306)/dbname?charset=utf8mb4&parseTime=True&loc=Local"
    db, err := gorm.Open(mysql.Open(dsn), &gorm.Config{})
}
```
Untuk menangani time.Time dengan benar, kamu **perlu menambahkan parameter **parseTime. Jika ingin UTF-8 full encoding kite perlu ubah charset=utf8 menjadi charset=utf8mb4. Lihat artikel terkait untuk penjelasan lengkap. Driver MySQL juga menyediakan beberapa pengaturan lanjutan saat inisialisasi, contoh:

```go
db, err := gorm.Open(mysql.New(mysql.Config{
  DSN: "gorm:gorm@tcp(127.0.0.1:3306)/gorm?charset=utf8&parseTime=True&loc=Local", // data source name
  DefaultStringSize: 256, // default size for string fields
  DisableDatetimePrecision: true, // disable datetime precision, which not supported before MySQL 5.6
  DontSupportRenameIndex: true, // drop & create when rename index, rename index not supported before MySQL 5.7, MariaDB
  DontSupportRenameColumn: true, // `change` when rename column, rename column not supported before MySQL 8, MariaDB
  SkipInitializeWithVersion: false, // auto configure based on currently MySQL version
}), &gorm.Config{})
```

GORM juga memungkinkan kamu untuk menggunakan driver MySQL kustom dengan opsi DriverName, misalnya:

```go
import (
  _ "example.com/my_mysql_driver"
  "gorm.io/driver/mysql"
  "gorm.io/gorm"
)

db, err := gorm.Open(mysql.New(mysql.Config{
  DriverName: "my_mysql_driver",
  DSN: "gorm:gorm@tcp(localhost:9910)/gorm?charset=utf8&parseTime=True&loc=Local", //https://github.com/go-sql-driver/mysql#dsn-data-source-name
}), &gorm.Config{})
```
Jika kamu sudah punya koneksi *sql.DB sebelumnya, GORM bisa langsung menggunakan koneksi tersebut:

```go
import (
  "database/sql"
  "gorm.io/driver/mysql"
  "gorm.io/gorm"
)

sqlDB, err := sql.Open("mysql", "mydb_dsn")
gormDB, err := gorm.Open(mysql.New(mysql.Config{
  Conn: sqlDB,
}), &gorm.Config{})
```
## 2. PostgreSQL

```go
import (
  "gorm.io/driver/postgres"
  "gorm.io/gorm"
)

dsn := "host=localhost user=gorm password=gorm dbname=gorm port=9920 sslmode=disable TimeZone=Asia/Shanghai"
db, err := gorm.Open(postgres.Open(dsn), &gorm.Config{})
```
Secara default, GORM menggunakan pgx sebagai driver PostgreSQL, yang mendukung cache prepared statement. Jika ingin menonaktifkan fitur ini:

```go
db, err := gorm.Open(postgres.New(postgres.Config{
  DSN: "user=gorm password=gorm dbname=gorm port=9920 sslmode=disable TimeZone=Asia/Shanghai",
  PreferSimpleProtocol: true,
}), &gorm.Config{})
```
Kamu juga bisa menggunakan driver khusus untuk PostgreSQL, misalnya yang disediakan oleh Google Cloud:

```go
import (
  _ "github.com/GoogleCloudPlatform/cloudsql-proxy/proxy/dialers/postgres"
  "gorm.io/gorm"
)

db, err := gorm.Open(postgres.New(postgres.Config{
  DriverName: "cloudsqlpostgres",
  DSN: "host=project:region:instance user=postgres dbname=postgres password=password sslmode=disable",
}))
```
Koneksi Database yang Sudah Ada seperti berikut

```go
import (
  "database/sql"
  "gorm.io/driver/postgres"
  "gorm.io/gorm"
)

sqlDB, err := sql.Open("pgx", "mydb_dsn")
gormDB, err := gorm.Open(postgres.New(postgres.Config{
  Conn: sqlDB,
}), &gorm.Config{})
```
# 3. GaussDB

```go
import (
  "gorm.io/driver/gaussdb"
  "gorm.io/gorm"
)

dsn := "host=localhost user=gorm password=gorm dbname=gorm port=8000 sslmode=disable TimeZone=Asia/Shanghai"
db, err := gorm.Open(gaussdb.Open(dsn), &gorm.Config{})
```
GORM menggunakan gaussdb-go sebagai driver GaussDB dan secara default mengaktifkan cache prepared statement. Kamu bisa menonaktifkannya seperti ini:

```go
db, err := gorm.Open(gaussdb.New(gaussdb.Config{
  DSN: "user=gorm password=gorm dbname=gorm port=8000 sslmode=disable TimeZone=Asia/Shanghai",
  PreferSimpleProtocol: true,
}), &gorm.Config{})
```
Contoh driver yang dikustomisasi seperti dibawah ini

```go
import (
  _ "github.com/GoogleCloudPlatform/cloudsql-proxy/proxy/dialers/gaussdb"
  "gorm.io/gorm"
)

db, err := gorm.Open(gaussdb.New(gaussdb.Config{
  DriverName: "cloudsqlgaussdb",
  DSN: "host=project:region:instance user=gaussdb dbname=gaussdb password=password sslmode=disable",
}))
```
 \
Koneksi ke database yang sudah ada dapat dilakukan seperti berikut

```go
import (
  "database/sql"
  "gorm.io/driver/gaussdb"
  "gorm.io/gorm"
)

sqlDB, err := sql.Open("gaussdbgo", "mydb_dsn")
gormDB, err := gorm.Open(gaussdb.New(gaussdb.Config{
  Conn: sqlDB,
}), &gorm.Config{})
```
Selengkapnya dapat kamu pelajari di dokumentasi resmi [GORM](https://gorm.io/docs/connecting_to_the_database.html).

# 5.5 CRUD

## A.CREATE

### 1. Create Record

#### a. Generic API

```
user := User{Name: "Jinzhu", Age: 18, Birthday: time.Now()}
// Membuat satu record
ctx := context.Background()
err := gorm.G[User](db).Create(ctx, &user) // kirim pointer data ke Create

result := gorm.WithResult()
err := gorm.G[User](db, result).Create(ctx, &user)
user.ID             // mengembalikan primary key dari data yang dimasukkan
result.Error        // mengembalikan error
result.RowsAffected // jumlah record yang dimasukkan
```
#### b. Traditional API

```
user := User{Name: "Jinzhu", Age: 18, Birthday: time.Now()}

result := db.Create(&user) // kirim pointer data ke Create

user.ID             // mengembalikan primary key dari data yang dimasukkan
result.Error        // mengembalikan error
result.RowsAffected // jumlah record yang dimasukkan
```
### 2. Membuat Banyak Record (Batch Create)

```
users := []*User{
  {Name: "Jinzhu", Age: 18, Birthday: time.Now()},
  {Name: "Jackson", Age: 19, Birthday: time.Now()},
}

result := db.Create(users) // kirim slice untuk memasukkan beberapa baris

result.Error        // mengembalikan error
result.RowsAffected // jumlah record yang dimasukkan
```
Anda tidak bisa mengirim struct langsung ke Create, jadi pastikan untuk mengirim pointer ke data.

### 3. Membuat Record dengan Field Tertentu

#### 1. Hanya Memilih Field Tertentu
```go
db.Select("Name", "Age", "CreatedAt").Create(&user)
// INSERT INTO users (name,age,created_at) VALUES ("jinzhu", 18, "2020-07-04 11:05:21.775")
```
#### 2. Melewati Field Tertentu (Omit)
```go
db.Omit("Name", "Age", "CreatedAt").Create(&user)
// INSERT INTO users (birthday,updated_at) VALUES ("2020-01-01 00:00:00.000", "2020-07-04 11:05:21.775")
```
### 4. Batch Insert (Masukkan Banyak Data Sekaligus)

Untuk memasukkan banyak data secara efisien, kirimkan slice ke metode Create. GORM akan membuat satu pernyataan SQL untuk semua data dan mengisi kembali nilai primary key. Metode hook juga akan dipanggil. Transaksi akan digunakan jika data dipecah dalam beberapa batch.

```go
var users = []User{{Name: "jinzhu1"}, {Name: "jinzhu2"}, {Name: "jinzhu3"}}
db.Create(&users)

for _, user := range users {
  user.ID // hasil: 1, 2, 3
}
```
Ukuran Batch juga bisa ditentukan dengan

```go
var users = []User{{Name: "jinzhu_1"}, ..., {Name: "jinzhu_10000"}}

// ukuran batch: 100
db.CreateInBatches(users, 100)
```
### 5. Create Hooks (Fungsi yang Dipanggil Otomatis Sebelum/Sesudah Create)

GORM mendukung hook yang bisa Anda definisikan sendiri seperti: BeforeSave, BeforeCreate, AfterSave, AfterCreate. Fungsi-fungsi ini akan dipanggil saat membuat record.

```go
func (u *User) BeforeCreate(tx *gorm.DB) (err error) {
  u.UUID = uuid.New()

  if u.Role == "admin" {
    return errors.New("invalid role")
  }
  return
}
```
Jika kamu ingin **melewati hook**, gunakan SkipHooks:
```go
DB.Session(&gorm.Session{SkipHooks: true}).Create(&user)
DB.Session(&gorm.Session{SkipHooks: true}).Create(&users)
DB.Session(&gorm.Session{SkipHooks: true}).CreateInBatches(users, 100)
```

### 6. Membuat dari Map (Create from Map)

GORM mendukung pembuatan dari map[string]interface{} atau []map[string]interface{}{}.

```go
db.Model(&User{}).Create(map[string]interface{}{
  "Name": "jinzhu", "Age": 18,
})

// batch insert dari slice of map
db.Model(&User{}).Create([]map[string]interface{}{
  {"Name": "jinzhu_1", "Age": 18},
  {"Name": "jinzhu_2", "Age": 20},
})
```
### 7. Create From SQL Expression/Context Valuer

GORM juga memungkinkan membuat data dari ekspresi SQL, melalui map[string]interface{} atau tipe data yang dikustomisasi (customized data types)
```go
`db.Model(&User{}).Create(map[string]interface{}{
  "Name": gorm.Expr("UPPER(?)", "jinzhu"),
})
```
## B.QUERY

### 1. Mengambil Satu Data

GORM menyediakan metode seperti First, Take, dan Last untuk mengambil satu objek dari database. Ketiganya akan otomatis menambahkan kondisi LIMIT 1 pada query, dan akan menghasilkan error ErrRecordNotFound jika tidak ditemukan data.

#### 1. Generic API

```go
ctx := context.Background()

user, err := gorm.G[User](db).First(ctx) // ambil data pertama berdasarkan primary key (biasanya id ASC)
user, err := gorm.G[User](db).Take(ctx)  // ambil satu data tanpa urutan khusus
user, err := gorm.G[User](db).Last(ctx)  // ambil data terakhir berdasarkan primary key (biasanya id DESC)

errors.Is(err, gorm.ErrRecordNotFound) // cek apakah error-nya karena tidak ada data
```
#### 2. **Traditional API**

```go
db.First(&user)
db.Take(&user)
db.Last(&user)

result := db.First(&user)
result.RowsAffected // jumlah baris yang ditemukan
result.Error        // error (jika ada)
```
Jika ingin menghindari error ErrRecordNotFound, bisa gunakan Find dengan Limit(1), tapi perlu diingat Find akan tetap mengembalikan slice atau struct yang diisi meskipun tidak ditemukan (tanpa error).

### 2. Mengambil Berdasarkan Primary Key

Bisa menggunakan nilai langsung atau kondisi Where, baik angka maupun string:

#### 1. Generic API

```go
ctx := context.Background()

// Using numeric primary key
user, err := gorm.G[User](db).Where("id = ?", 10).First(ctx)
// SELECT * FROM users WHERE id = 10;

// Using string primary key
user, err := gorm.G[User](db).Where("id = ?", "10").First(ctx)
// SELECT * FROM users WHERE id = 10;

// Using multiple primary keys
users, err := gorm.G[User](db).Where("id IN ?", []int{1,2,3}).Find(ctx)
// SELECT * FROM users WHERE id IN (1,2,3);

// If the primary key is a string (for example, like a uuid)
user, err := gorm.G[User](db).Where("id = ?", "1b74413f-f3b8-409f-ac47-e8c062e3472a").First(ctx)
// SELECT * FROM users WHERE id = "1b74413f-f3b8-409f-ac47-e8c062e3472a";
```
#### 2. Traditional API


```go
db.First(&user, 10)                          // ambil user dengan id 10
db.First(&user, "10")                        // sama saja, karena string tetap di-cast ke angka
db.Find(&users, []int{1, 2, 3})              // ambil banyak user dengan id 1, 2, dan 3
db.First(&user, "id = ?", "uuid-value")      // jika primary key berupa string seperti UUID
```
Jika struct sudah punya nilai ID, maka saat First dipanggil, GORM akan otomatis pakai nilai tersebut.
```go
user := User{ID: 10}
db.First(&user) // SELECT * FROM users WHERE id = 10
```
Jika model punya field DeletedAt, GORM akan otomatis menambahkan filter deleted_at IS NULL.

### 3. Mengambil Semua Data
```go
db.Find(&users) // ambil semua data dari tabel users
```
### 4. String Conditions

Contoh penggunaan kondisi umum:

```go
db.Where("name = ?", "jinzhu").First(&user)
db.Where("name IN ?", []string{"a", "b"}).Find(&users)
db.Where("name LIKE ?", "%jin%").Find(&users)
db.Where("age BETWEEN ? AND ?", 20, 30).Find(&users)
```
Kalau kamu mengisi primary key (ID) di struct user, maka .Where() tidak akan override, tapi malah jadi tambahan kondisi. Contoh:

```go
user := User{ID: 10}
db.Where("id = ?", 20).First(&user)
// Query jadi: WHERE id = 10 AND id = 20
// Tidak akan ketemu (karena id tidak bisa 10 dan 20 sekaligus)
```
### 5. Struct & Map Conditions

#### 1. Struct
```go
db.Where(&User{Name: "jinzhu", Age: 20}).First(&user)
// SELECT * FROM users WHERE name = 'jinzhu' AND age = 20
```
#### 2. Map
```go
db.Where(map[string]interface{}{"name": "jinzhu", "age": 20}).Find(&users)
// SELECT * FROM users WHERE name = 'jinzhu' AND age = 20
```
#### 3. Slice of ID
```go
db.Where([]int64{20, 21, 22}).Find(&users)

// SELECT * FROM users WHERE id IN (20, 21, 22)
Kalau pakai struct, field kosong seperti 0, "", false tidak dianggap sebagai kondisi.

db.Where(&User{Name: "jinzhu", Age: 0}).Find(&users)

// SELECT * FROM users WHERE name = "jinzhu";  (Age = 0 tidak dipakai)
```
### 6. Specify Struct Fields
```go
db.Where(&User{Name: "jinzhu"}, "name", "Age").Find(&users)

// SELECT * FROM users WHERE name = "jinzhu" AND age = 0
db.Where(&User{Name: "jinzhu"}, "Age").Find(&users)

// SELECT * FROM users WHERE age = 0
```
### 7. Inline Condition (kondisi langsung dalam fungsi Find/First)

```go
db.First(&user, "id = ?", "abc-uuid")
// SELECT * FROM users WHERE id = 'abc-uuid'

db.Find(&users, "name <> ? AND age > ?", "jinzhu", 20)
// SELECT * FROM users WHERE name <> 'jinzhu' AND age > 20

db.Find(&users, User{Age: 20})
// SELECT * FROM users WHERE age = 20

db.Find(&users, map[string]interface{}{"age": 20})
// SELECT * FROM users WHERE age = 20
```
### 8. Not Condition (kondisi pengecualian)

```go
// NOT string
db.Not("name = ?", "jinzhu").First(&user)
// SELECT * FROM users WHERE NOT name = 'jinzhu' LIMIT 1

// NOT IN map
db.Not(map[string]interface{}{"name": []string{"a", "b"}}).Find(&users)
// SELECT * FROM users WHERE name NOT IN ('a','b')

// NOT struct
db.Not(User{Name: "jinzhu", Age: 18}).First(&user)
// SELECT * FROM users WHERE name <> 'jinzhu' AND age <> 18 LIMIT 1

// NOT ID
db.Not([]int64{1, 2, 3}).First(&user)
// SELECT * FROM users WHERE id NOT IN (1,2,3) LIMIT 1
```
### 9. Or Condition

```go
db.Where("role = ?", "admin").Or("role = ?", "super_admin").Find(&users)
// SELECT * FROM users WHERE role = 'admin' OR role = 'super_admin'

db.Where("name = 'jinzhu'").Or(User{Name: "jinzhu 2", Age: 18}).Find(&users)
// SELECT * FROM users WHERE name = 'jinzhu' OR (name = 'jinzhu 2' AND age = 18)

db.Where("name = 'jinzhu'").Or(map[string]interface{}{"name": "jinzhu 2", "age": 18}).Find(&users)
// SELECT * FROM users WHERE name = 'jinzhu' OR (name = 'jinzhu 2' AND age = 18)
```
### 10. Selecting Specific Fields

```go
db.Select("name", "age").Find(&users)
// SELECT name, age FROM users;

db.Select([]string{"name", "age"}).Find(&users)
// SELECT name, age FROM users;

db.Table("users").Select("COALESCE(age,?)", 42).Rows()
// SELECT COALESCE(age,'42') FROM users;
```
### 11. Order

```
db.Order("age desc, name").Find(&users)
// SELECT * FROM users ORDER BY age desc, name;


// Multiple orders
db.Order("age desc").Order("name").Find(&users)
// SELECT * FROM users ORDER BY age desc, name;

db.Clauses(clause.OrderBy{
  Expression: clause.Expr{SQL: "FIELD(id,?)", Vars: []interface{}{[]int{1, 2, 3}}, WithoutParentheses: true},
}).Find(&User{})
// SELECT * FROM users ORDER BY FIELD(id,1,2,3)
```
### 12. Limit & Offset

**Limit** menentukan jumlah maksimum data yang akan diambil.

**Offset** menentukan jumlah data yang akan dilewati sebelum mulai mengambil data.

```go
db.Limit(3).Find(&users)
// SELECT * FROM users LIMIT 3;

// Cancel limit condition with -1
db.Limit(10).Find(&users1).Limit(-1).Find(&users2)
// SELECT * FROM users LIMIT 10; (users1)
// SELECT * FROM users; (users2)

db.Offset(3).Find(&users)
// SELECT * FROM users OFFSET 3;

db.Limit(10).Offset(5).Find(&users)
// SELECT * FROM users OFFSET 5 LIMIT 10;

// Cancel offset condition with -1
db.Offset(10).Find(&users1).Offset(-1).Find(&users2)
// SELECT * FROM users OFFSET 10; (users1)
// SELECT * FROM users; (users2)
```
### 13. Group By & Having

```go
type result struct {
  Date  time.Time
  Total int
}
db.Model(&User{}).Select("name, sum(age) as total").Where("name LIKE ?", "group%").Group("name").First(&result)
// SELECT name, sum(age) as total FROM `users` WHERE name LIKE "group%" GROUP BY `name` LIMIT 1

db.Model(&User{}).Select("name, sum(age) as total").Group("name").Having("name = ?", "group").Find(&result)
// SELECT name, sum(age) as total FROM `users` GROUP BY `name` HAVING name = "group"

rows, err := db.Table("orders").Select("date(created_at) as date, sum(amount) as total").Group("date(created_at)").Rows()
defer rows.Close()
for rows.Next() {
  ...
}

rows, err := db.Table("orders").Select("date(created_at) as date, sum(amount) as total").Group("date(created_at)").Having("sum(amount) > ?", 100).Rows()
defer rows.Close()
for rows.Next() {
  ...
}

type Result struct {
  Date  time.Time
  Total int64
}
db.Table("orders").Select("date(created_at) as date, sum(amount) as total").Group("date(created_at)").Having("sum(amount) > ?", 100).Scan(&results)
```
### 14. Distinct
```go
db.Distinct("name", "age").Order("name, age desc").Find(&results)
```
### 15. Join

```go
type result struct {
  Name  string
  Email string
}

db.Model(&User{}).Select("users.name, emails.email").Joins("left join emails on emails.user_id = users.id").Scan(&result{})
// SELECT users.name, emails.email FROM `users` left join emails on emails.user_id = users.id

rows, err := db.Table("users").Select("users.name, emails.email").Joins("left join emails on emails.user_id = users.id").Rows()
for rows.Next() {
  ...
}

db.Table("users").Select("users.name, emails.email").Joins("left join emails on emails.user_id = users.id").Scan(&results)

// multiple joins with parameter
db.Joins("JOIN emails ON emails.user_id = users.id AND emails.email = ?", "jinzhu@example.org").Joins("JOIN credit_cards ON credit_cards.user_id = users.id").Where("credit_cards.number = ?", "411111111111").Find(&user)
```
### 16. Scan

```go
type Result struct {
  Name string
  Age  int
}

var result Result
db.Table("users").Select("name", "age").Where("name = ?", "Antonio").Scan(&result)

// Raw SQL
db.Raw("SELECT name, age FROM users WHERE name = ?", "Antonio").Scan(&result)
```
## C.UPDATE

### 1. Save All Data

#### 1. Traditional API

```go
db.First(&user)

user.Name = "jinzhu 2"
user.Age = 100
db.Save(&user)
// UPDATE users SET name='jinzhu 2', age=100, birthday='2016-01-01', updated_at = '2013-11-17 21:34:10' WHERE id=111;
```
Save is an upsert function:

If the value contains no primary key, it performs Create

If the value has a primary key, it first executes Update (all fields, by Select(*)).

If rows affected = 0 after Update, it automatically falls back to Create.

### 2. Update SIngle Column

#### 1. Generic API

```go
ctx := context.Background()
// Update with conditions
err := gorm.G[User](db).Where("active = ?", true).Update(ctx, "name", "hello")
// UPDATE users SET name='hello', updated_at='2013-11-17 21:34:10' WHERE active=true;

// Update with ID condition
err := gorm.G[User](db).Where("id = ?", 111).Update(ctx, "name", "hello")
// UPDATE users SET name='hello', updated_at='2013-11-17 21:34:10' WHERE id=111;

// Update with multiple conditions
err := gorm.G[User](db).Where("id = ? AND active = ?", 111, true).Update(ctx, "name", "hello")
// UPDATE users SET name='hello', updated_at='2013-11-17 21:34:10' WHERE id=111 AND active=true;
```
#### 2. Traditional API

```go
// Update with conditions
db.Model(&User{}).Where("active = ?", true).Update("name", "hello")
// UPDATE users SET name='hello', updated_at='2013-11-17 21:34:10' WHERE active=true;

// User's ID is `111`:
db.Model(&user).Update("name", "hello")
// UPDATE users SET name='hello', updated_at='2013-11-17 21:34:10' WHERE id=111;

// Update with conditions and model value
db.Model(&user).Where("active = ?", true).Update("name", "hello")
// UPDATE users SET name='hello', updated_at='2013-11-17 21:34:10' WHERE id=111 AND active=true;
```
### 3. Update Multiple Column

#### 1. Generic API

```go
ctx := context.Background()

// Update attributes with `struct`, will only update non-zero fields
err := gorm.G[User](db).Where("id = ?", 111).Updates(ctx, User{Name: "hello", Age: 18, Active: false})
// UPDATE users SET name='hello', age=18, updated_at = '2013-11-17 21:34:10' WHERE id = 111;

// Update attributes with `map`
err := gorm.G[User](db).Where("id = ?", 111).Updates(ctx, map[string]interface{}{"name": "hello", "age": 18, "active": false})
// UPDATE users SET name='hello', age=18, active=false, updated_at='2013-11-17 21:34:10' WHERE id=111;
```
#### 2. Traditional API

```go
// Update attributes with `struct`, will only update non-zero fields
db.Model(&user).Updates(User{Name: "hello", Age: 18, Active: false})
// UPDATE users SET name='hello', age=18, updated_at = '2013-11-17 21:34:10' WHERE id = 111;

// Update attributes with `map`
db.Model(&user).Updates(map[string]interface{}{"name": "hello", "age": 18, "active": false})
// UPDATE users SET name='hello', age=18, active=false, updated_at='2013-11-17 21:34:10' WHERE id=111;
```
### 4. Update Selected Field

#### 1. Generic API

```go
ctx := context.Background()

// Select with Map
err := gorm.G[User](db).Where("id = ?", 111).Select("name").Updates(ctx, map[string]interface{}{"name": "hello", "age": 18, "active": false})
// UPDATE users SET name='hello' WHERE id=111;

err := gorm.G[User](db).Where("id = ?", 111).Omit("name").Updates(ctx, map[string]interface{}{"name": "hello", "age": 18, "active": false})
// UPDATE users SET age=18, active=false, updated_at='2013-11-17 21:34:10' WHERE id=111;

// Select with Struct (select zero value fields)
err := gorm.G[User](db).Where("id = ?", 111).Select("Name", "Age").Updates(ctx, User{Name: "new_name", Age: 0})
// UPDATE users SET name='new_name', age=0 WHERE id=111;

// Select all fields (select all fields include zero value fields)
err := gorm.G[User](db).Where("id = ?", 111).Select("*").Updates(ctx, User{Name: "jinzhu", Role: "admin", Age: 0})

// Select all fields but omit Role (select all fields include zero value fields)
err := gorm.G[User](db).Where("id = ?", 111).Select("*").Omit("Role").Updates(ctx, User{Name: "jinzhu", Role: "admin", Age: 0})
```
#### 2. Traditional API

```go
// Select with Map
// User's ID is `111`:
db.Model(&user).Select("name").Updates(map[string]interface{}{"name": "hello", "age": 18, "active": false})
// UPDATE users SET name='hello' WHERE id=111;

db.Model(&user).Omit("name").Updates(map[string]interface{}{"name": "hello", "age": 18, "active": false})
// UPDATE users SET age=18, active=false, updated_at='2013-11-17 21:34:10' WHERE id=111;

// Select with Struct (select zero value fields)
db.Model(&user).Select("Name", "Age").Updates(User{Name: "new_name", Age: 0})
// UPDATE users SET name='new_name', age=0 WHERE id=111;

// Select all fields (select all fields include zero value fields)
db.Model(&user).Select("*").Updates(User{Name: "jinzhu", Role: "admin", Age: 0})

// Select all fields but omit Role (select all fields include zero value fields)
db.Model(&user).Select("*").Omit("Role").Updates(User{Name: "jinzhu", Role: "admin", Age: 0})
```
### 5. Update Hooks

```go
func (u *User) BeforeUpdate(tx *gorm.DB) (err error) {
 if u.Role == "admin" {
   return errors.New("admin user not allowed to update")
 }
 return
}
```
### 6. Batch Update

```go
// Update with struct
db.Model(User{}).Where("role = ?", "admin").Updates(User{Name: "hello", Age: 18})
// UPDATE users SET name='hello', age=18 WHERE role = 'admin';

// Update with map
db.Table("users").Where("id IN ?", []int{10, 11}).Updates(map[string]interface{}{"name": "hello", "age": 18})
// UPDATE users SET name='hello', age=18 WHERE id IN (10, 11);
```
### 7. Block Global Update

```go
db.Model(&User{}).Update("name", "jinzhu").Error // gorm.ErrMissingWhereClause

db.Model(&User{}).Where("1 = 1").Update("name", "jinzhu")
// UPDATE users SET `name` = "jinzhu" WHERE 1=1

db.Exec("UPDATE users SET name = ?", "jinzhu")
// UPDATE users SET name = "jinzhu"

db.Session(&gorm.Session{AllowGlobalUpdate: true}).Model(&User{}).Update("name", "jinzhu")
// UPDATE users SET `name` = "jinzhu"
```
### 8. Update Record Count

```go
// Get updated records count with `RowsAffected`
result := db.Model(User{}).Where("role = ?", "admin").Updates(User{Name: "hello", Age: 18})
// UPDATE users SET name='hello', age=18 WHERE role = 'admin';

result.RowsAffected // returns updated records count
result.Error        // returns updating error
```
```
ctx := context.Background()

// Delete by ID
err := gorm.G[Email](db).Where("id = ?", 10).Delete(ctx)
// DELETE from emails where id = 10;

// Delete with additional conditions
err := gorm.G[Email](db).Where("id = ? AND name = ?", 10, "jinzhu").Delete(ctx)
// DELETE from emails where id = 10 AND name = "jinzhu";
```
## D.DELETE

### 1. Delete Record

#### 1. Generic API

|
|

#### 2. Traditional API

```go
// Email's ID is `10`
db.Delete(&email)
// DELETE from emails where id = 10;

// Delete with additional conditions
db.Where("name = ?", "jinzhu").Delete(&email)
// DELETE from emails where id = 10 AND name = "jinzhu";
```
### 2. Delete With Primary Key

```go
db.Delete(&User{}, 10)
// DELETE FROM users WHERE id = 10;

db.Delete(&User{}, "10")
// DELETE FROM users WHERE id = 10;

db.Delete(&users, []int{1,2,3})
// DELETE FROM users WHERE id IN (1,2,3);
```
### 3. Delete Hooks

```go
func (u *User) BeforeDelete(tx *gorm.DB) (err error) {
 if u.Role == "admin" {
   return errors.New("admin user not allowed to delete")
 }
 return
}
```
### 4. Batch Delete

#### 1. Generic API


```go
ctx := context.Background()

// Batch delete with conditions
err := gorm.G[Email](db).Where("email LIKE ?", "%jinzhu%").Delete(ctx)
// DELETE from emails where email LIKE "%jinzhu%";
```
#### 2. Traditional API

```go
db.Where("email LIKE ?", "%jinzhu%").Delete(&Email{})
// DELETE from emails where email LIKE "%jinzhu%";

db.Delete(&Email{}, "email LIKE ?", "%jinzhu%")
// DELETE from emails where email LIKE "%jinzhu%";
```

**ATAU**


```go
var users = []User{{ID: 1}, {ID: 2}, {ID: 3}}
db.Delete(&users)
// DELETE FROM users WHERE id IN (1,2,3);

db.Delete(&users, "name LIKE ?", "%jinzhu%")
// DELETE FROM users WHERE name LIKE "%jinzhu%" AND id IN (1,2,3);
```
### 5. Block Global Delete

#### 1. Generic API

```go
ctx := context.Background()
// These will return error
err := gorm.G[User](db).Delete(ctx) // gorm.ErrMissingWhereClause
// These will work
err := gorm.G[User](db).Where("1 = 1").Delete(ctx)
// DELETE FROM `users` WHERE 1=1
```
#### 2. Traditional API

```go
db.Delete(&User{}).Error // gorm.ErrMissingWhereClause
db.Delete(&[]User{{Name: "jinzhu1"}, {Name: "jinzhu2"}}).Error // gorm.ErrMissingWhereClause
db.Where("1 = 1").Delete(&User{})
// DELETE FROM `users` WHERE 1=1
db.Exec("DELETE FROM users")
// DELETE FROM users
db.Session(&gorm.Session{AllowGlobalUpdate: true}).Delete(&User{})
// DELETE FROM users
```
### 6. Returning Data From Deleted Rows

```go
// return all columns
var users []User
DB.Clauses(clause.Returning{}).Where("role = ?", "admin").Delete(&users)
// DELETE FROM `users` WHERE role = "admin" RETURNING *
// users => []User{{ID: 1, Name: "jinzhu", Role: "admin", Salary: 100}, {ID: 2, Name: "jinzhu.2", Role: "admin", Salary: 1000}}

// return specified columns
DB.Clauses(clause.Returning{Columns: []clause.Column{{Name: "name"}, {Name: "salary"}}}).Where("role = ?", "admin").Delete(&users)
// DELETE FROM `users` WHERE role = "admin" RETURNING `name`, `salary`
// users => []User{{ID: 0, Name: "jinzhu", Role: "", Salary: 100}, {ID: 0, Name: "jinzhu.2", Role: "", Salary: 1000}}
```
### 7. Soft Delete

**	**Sempat disinggung pada pembahasa model yakni di gorm.Model

#### 1. Find Soft Deleted Records

```go
db.Unscoped().Where("age = 20").Find(&users)
// SELECT * FROM users WHERE age = 20;
```
#### 2. Delete Permanently

```go
db.Unscoped().Delete(&order)
// DELETE FROM orders WHERE id=10;
```

# 5.6 Assosiasi (has one, has many, many to many)

## 5.6.1 Has One

Asosiasi Has One menetapkan hubungan satu-ke-satu dengan model lain, tetapi dengan semantik (dan konsekuensi) yang sedikit berbeda. Asosiasi ini menunjukkan bahwa setiap instance dari suatu model memiliki atau menguasai satu instance dari model lain.

Contoh: jika aplikasi kamu memiliki entitas *User* dan *CreditCard*, dan setiap pengguna hanya bisa memiliki satu kartu kredit.

### 1. Declaration

```go
// User has one CreditCard, UserID is the foreign key
type User struct {
 gorm.Model
 CreditCard CreditCard
}

type CreditCard struct {
 gorm.Model
 Number string
 UserID uint
}
```
### 2. Retrieve

```go
// Retrieve user list with eager loading credit card
func GetAll(db *gorm.DB) ([]User, error) {
 var users []User
 err := db.Model(&User{}).Preload("CreditCard").Find(&users).Error
 return users, err
}
```
### 3. Menimpa Foreign Key

Untuk hubungan has one, kolom foreign key harus ada, dan entitas pemilik akan menyimpan primary key dari model yang dimilikinya ke dalam kolom ini.

Nama kolom tersebut biasanya dihasilkan dari **tipe model has one** ditambah dengan primary key-nya, misalnya pada contoh di atas menjadi UserID.

Ketika kamu memberikan kartu kredit ke pengguna, maka ID dari pengguna akan disimpan ke kolom UserID.

Jika kamu ingin menggunakan kolom lain sebagai foreign key, kamu bisa mengubahnya dengan tag foreignKey, contohnya:

```go
type User struct {
 gorm.Model
 CreditCard CreditCard `gorm:"foreignKey:UserName"`
 // use UserName as foreign key
}

type CreditCard struct {
 gorm.Model
 Number   string
 UserName string
}
```
### 4. Menimpa Referensi

Secara default, entitas yang dimiliki akan menyimpan primary key dari model yang memilikinya ke dalam foreign key. Kamu bisa mengubahnya agar menyimpan nilai dari kolom lain, seperti menggunakan Name pada contoh berikut:

```go
type User struct {
 gorm.Model
 Name       string     `gorm:"index"`
 CreditCard CreditCard `gorm:"foreignKey:UserName;references:Name"`
}

type CreditCard struct {
 gorm.Model
 Number   string
 UserName string
}
```
### 5. FOREIGN KEY Constrain

Kamu bisa mengatur batasan **OnUpdate** dan **OnDelete** menggunakan tag constraint. Ini akan diterapkan saat melakukan migrasi dengan GORM, contohnya:

```go
type User struct {
  gorm.Model
  CreditCard CreditCard `gorm:"constraint:OnUpdate:CASCADE,OnDelete:SET NULL;"`
}

type CreditCard struct {
  gorm.Model
  Number string
  UserID uint
}
```
## 5.6.2. Has Many

Has Many adalah asosiasi yang menetapkan hubungan satu-ke-banyak dengan model lain. Berbeda dengan Has One, pemiliknya bisa memiliki nol atau banyak instance dari model terkait. Misalnya, jika aplikasi Anda memiliki entitas *User* dan *CreditCard*, dan setiap *User* bisa memiliki banyak *CreditCard*.

### 1. Declare

```go
// User has many CreditCards, UserID is the foreign key
type User struct {
  gorm.Model
  CreditCards []CreditCard
}
type CreditCard struct {
  gorm.Model
  Number string
  UserID uint
}
```
### 2. Retrieve

```go
// Retrieve user list with eager loading credit cards
func GetAll(db *gorm.DB) ([]User, error) {
   var users []User
   err := db.Model(&User{}).Preload("CreditCards").Find(&users).Error
   return users, err
}
```
### 3. Override Foreign Key

Untuk mendefinisikan hubungan **has many**, sebuah *foreign key* harus ada. Secara default, nama *foreign key* adalah gabungan dari nama tipe pemilik dan nama primary key-nya. Sebagai contoh, untuk mendefinisikan model yang *belongs to* User, maka foreign key-nya adalah UserID.

Jika ingin menggunakan field lain sebagai *foreign key*, Anda bisa menyesuaikannya dengan tag foreignKey, contohnya:

```go
type User struct {
 gorm.Model
 CreditCards []CreditCard `gorm:"foreignKey:UserRefer"`
}

type CreditCard struct {
 gorm.Model
 Number    string
 UserRefer uint
}
```
### 4. Override References

Secara default, GORM menggunakan primary key dari pemilik sebagai nilai dari foreign key. Pada contoh di atas, GORM akan menyimpan User.ID ke field UserID milik CreditCard.

Namun, Anda bisa menggantinya dengan tag references, contohnya:

```go
type User struct {
  gorm.Model
  MemberNumber string
  CreditCards  []CreditCard `gorm:"foreignKey:UserNumber;references:MemberNumber"`
}

type CreditCard struct {
  gorm.Model
  Number 	string
  UserNumber string
}
```
### 5. FOREIGN KEY Constrain

```go
type User struct {
  gorm.Model
  CreditCards []CreditCard `gorm:"constraint:OnUpdate:CASCADE,OnDelete:SET NULL;"`
}

type CreditCard struct {
  gorm.Model
  Number string
  UserID uint
}
```
## 5.6.3 Many2Many

Relasi **many to many** (banyak ke banyak) menambahkan *join table* (tabel penghubung) antara dua model.

Contoh: jika aplikasi kamu memiliki users dan languages, di mana satu user bisa berbicara banyak bahasa, dan satu bahasa bisa digunakan oleh banyak user:

```go
// User memiliki banyak Language melalui tabel user_languages
type User struct {
  gorm.Model
  Languages []Language `gorm:"many2many:user_languages;"`
}

type Language struct {
  gorm.Model
  Name string
}
```
Saat menggunakan AutoMigrate, GORM akan secara otomatis membuat tabel user_languages sebagai *join table*.

### 1. Back_Reference Declare

```go
type User struct {
  gorm.Model
  Languages []*Language `gorm:"many2many:user_languages;"`
}

type Language struct {
  gorm.Model
  Name  string
  Users []*User `gorm:"many2many:user_languages;"`
}
```
### 2. Back_Reference Retrieve

```go
// Retrieve user list with eager loading languages
func GetAllUsers(db *gorm.DB) ([]User, error) {
 var users []User
 err := db.Model(&User{}).Preload("Languages").Find(&users).Error
 return users, err
}

// Retrieve language list with eager loading users
func GetAllLanguages(db *gorm.DB) ([]Language, error) {
 var languages []Language
 err := db.Model(&Language{}).Preload("Users").Find(&languages).Error
 return languages, err
}
```
### 3. Override Foreign Key

```go
type User struct {
  gorm.Model
  Profiles []Profile `gorm:"many2many:user_profiles;foreignKey:Refer;joinForeignKey:UserReferID;References:UserRefer;joinReferences:ProfileRefer"`
  Refer	uint  	`gorm:"index:,unique"`
}

type Profile struct {
  gorm.Model
  Name  	string
  UserRefer uint `gorm:"index:,unique"`
}
```
### 4. Self-Referential

```go
type User struct {
  gorm.Model
  Friends []*User `gorm:"many2many:user_friends"`
}
```
### 5. FOREIGN KEY Constrain

```go
type User struct {
  gorm.Model
  Languages []Language `gorm:"many2many:user_speaks;"`
}

type Language struct {
  Code string `gorm:"primaryKey"`
  Name string
}
```
### 6. Composite Foreign Key

Jika kamu menggunakan **primary key gabungan** (lebih dari satu kolom), GORM akan secara otomatis membuat foreign key gabungan. Contoh:

```go
type Tag struct {
  ID 	uint   `gorm:"primaryKey"`
  Locale string `gorm:"primaryKey"`
  Value  string
}

type Blog struct {
  ID     	uint   `gorm:"primaryKey"`
  Locale 	string `gorm:"primaryKey"`
  Subject	string
  Body   	string
  Tags   	[]Tag `gorm:"many2many:blog_tags;"`
  LocaleTags []Tag `gorm:"many2many:locale_blog_tags;ForeignKey:id,locale;References:id"`
  SharedTags []Tag `gorm:"many2many:shared_blog_tags;ForeignKey:id;References:id"`
}
```
# 5.7 Migrate

## 5.7.1 Auto Migration

AutoMigrate akan membuat tabel, foreign key yang hilang, constraints, kolom, dan index. Ini akan mengubah tipe kolom yang sudah ada jika ukuran atau presisinya berubah, atau jika berubah dari non-nullable menjadi nullable. TIDAK akan menghapus kolom yang tidak digunakan untuk melindungi data kamu.

```go
db.AutoMigrate(&User{})

db.AutoMigrate(&User{}, &Product{}, &Order{})

// Add table suffix when creating tables
db.Set("gorm:table_options", "ENGINE=InnoDB").AutoMigrate(&User{})
```
AutoMigrate akan membuat foreign key constraint secara otomatis. Kamu bisa menonaktifkan fitur ini saat inisialisasi:

```go
db, err := gorm.Open(sqlite.Open("gorm.db"), &gorm.Config{
 DisableForeignKeyConstraintWhenMigrating: true,
})
```
## 5.7.2 Migrator Interface

GORM menyediakan **interface migrator**, yang berisi API terpadu untuk setiap jenis database, yang bisa digunakan untuk membuat migrasi lintas-database, contohnya: SQLite tidak mendukung ALTER COLUMN, DROP COLUMN. GORM akan membuat tabel baru, menyalin data, menghapus tabel lama, lalu mengganti nama tabel baru. MySQL juga tidak mendukung rename kolom/index pada beberapa versi, GORM akan menyesuaikan perintah SQL sesuai versi MySQL.

```go
type Migrator interface {
 // AutoMigrate
 AutoMigrate(dst ...interface{}) error

 // Database
 CurrentDatabase() string
 FullDataTypeOf(*schema.Field) clause.Expr

 // Tables
 CreateTable(dst ...interface{}) error
 DropTable(dst ...interface{}) error
 HasTable(dst interface{}) bool
 RenameTable(oldName, newName interface{}) error
 GetTables() (tableList []string, err error)

 // Columns
 AddColumn(dst interface{}, field string) error
 DropColumn(dst interface{}, field string) error
 AlterColumn(dst interface{}, field string) error
 MigrateColumn(dst interface{}, field *schema.Field, columnType ColumnType) error
 HasColumn(dst interface{}, field string) bool
 RenameColumn(dst interface{}, oldName, field string) error
 ColumnTypes(dst interface{}) ([]ColumnType, error)

 // Views
 CreateView(name string, option ViewOption) error
 DropView(name string) error

 // Constraints
 CreateConstraint(dst interface{}, name string) error
 DropConstraint(dst interface{}, name string) error
 HasConstraint(dst interface{}, name string) bool

 // Indexes
 CreateIndex(dst interface{}, name string) error
 DropIndex(dst interface{}, name string) error
 HasIndex(dst interface{}, name string) bool
 RenameIndex(dst interface{}, oldName, newName string) error
}
```
## 5.7.3 CurrentDatabase

Mengembalikan nama database yang sedang digunakan:

```go
db.Migrator().CurrentDatabase()
```
## 5.7.4 Table

```go
// Membuat tabel untuk User
db.Migrator().CreateTable(&User{})

// Tambahkan opsi ENGINE
db.Set("gorm:table_options", "ENGINE=InnoDB").Migrator().CreateTable(&User{})

// Cek apakah tabel ada
db.Migrator().HasTable(&User{})
db.Migrator().HasTable("users")

// Hapus tabel jika ada
db.Migrator().DropTable(&User{})
db.Migrator().DropTable("users")

// Ganti nama tabel lama ke yang baru
db.Migrator().RenameTable(&User{}, &UserInfo{})
db.Migrator().RenameTable("users", "user_infos")
```
## 5.7.5 Columns

```go
type User struct {
 Name string
}

// Add name field
db.Migrator().AddColumn(&User{}, "Name")
// Drop name field
db.Migrator().DropColumn(&User{}, "Name")
// Alter name field
db.Migrator().AlterColumn(&User{}, "Name")
// Check column exists
db.Migrator().HasColumn(&User{}, "Name")

type User struct {
 Name    string
 NewName string
}

// Rename column to new name
db.Migrator().RenameColumn(&User{}, "Name", "NewName")
db.Migrator().RenameColumn(&User{}, "name", "new_name")

// ColumnTypes
db.Migrator().ColumnTypes(&User{}) ([]gorm.ColumnType, error)

type ColumnType interface {
 Name() string
 DatabaseTypeName() string                 // varchar
 ColumnType() (columnType string, ok bool) // varchar(64)
 PrimaryKey() (isPrimaryKey bool, ok bool)
 AutoIncrement() (isAutoIncrement bool, ok bool)
 Length() (length int64, ok bool)
 DecimalSize() (precision int64, scale int64, ok bool)
 Nullable() (nullable bool, ok bool)
 Unique() (unique bool, ok bool)
 ScanType() reflect.Type
 Comment() (value string, ok bool)
 DefaultValue() (value string, ok bool)
}
```
Ada juga ColumnType yang menyediakan informasi seperti Nama kolom (Name), Tipe data (DatabaseTypeName), Panjang (Length), Nullable, Unique, DefaultValue, dll.

```go
db.Migrator().ColumnTypes(&User{}) ([]gorm.ColumnType, error)
```
## 5.7.6 Views

SQLite belum mendukung opsi Replace pada ViewOption

```go
query := db.Model(&User{}).Where("age > ?", 20)

// Create View
db.Migrator().CreateView("users_pets", gorm.ViewOption{Query: query})
// CREATE VIEW `users_view` AS SELECT * FROM `users` WHERE age > 20

// Create or Replace View
db.Migrator().CreateView("users_pets", gorm.ViewOption{Query: query, Replace: true})
// CREATE OR REPLACE VIEW `users_pets` AS SELECT * FROM `users` WHERE age > 20

// Create View With Check Option
db.Migrator().CreateView("users_pets", gorm.ViewOption{Query: query, CheckOption: "WITH CHECK OPTION"})
// CREATE VIEW `users_pets` AS SELECT * FROM `users` WHERE age > 20 WITH CHECK OPTION

// Drop View
db.Migrator().DropView("users_pets")
// DROP VIEW IF EXISTS "users_pets"
```
## 5.7.7 Constrain

```go
type UserIndex struct {
 Name  string `gorm:"check:name_checker,name <> 'jinzhu'"`
}

// Create constraint
db.Migrator().CreateConstraint(&User{}, "name_checker")

// Drop constraint
db.Migrator().DropConstraint(&User{}, "name_checker")

// Check constraint exists
db.Migrator().HasConstraint(&User{}, "name_checker")
```
Buat FOREIGN KEY untuk relasi

```go
type User struct {
 gorm.Model
 CreditCards []CreditCard
}

type CreditCard struct {
 gorm.Model
 Number string
 UserID uint
}

// create database foreign key for user & credit_cards
db.Migrator().CreateConstraint(&User{}, "CreditCards")
db.Migrator().CreateConstraint(&User{}, "fk_users_credit_cards")
// ALTER TABLE `credit_cards` ADD CONSTRAINT `fk_users_credit_cards` FOREIGN KEY (`user_id`) REFERENCES `users`(`id`)

// check database foreign key for user & credit_cards exists or not
db.Migrator().HasConstraint(&User{}, "CreditCards")
db.Migrator().HasConstraint(&User{}, "fk_users_credit_cards")

// drop database foreign key for user & credit_cards
db.Migrator().DropConstraint(&User{}, "CreditCards")
db.Migrator().DropConstraint(&User{}, "fk_users_credit_cards")
```
## 5.7.8 Indexes

```go
type User struct {
 gorm.Model
 Name string `gorm:"size:255;index:idx_name,unique"`
}

// Create index for Name field
db.Migrator().CreateIndex(&User{}, "Name")
db.Migrator().CreateIndex(&User{}, "idx_name")

// Drop index for Name field
db.Migrator().DropIndex(&User{}, "Name")
db.Migrator().DropIndex(&User{}, "idx_name")

// Check Index exists
db.Migrator().HasIndex(&User{}, "Name")
db.Migrator().HasIndex(&User{}, "idx_name")

type User struct {
 gorm.Model
 Name  string `gorm:"size:255;index:idx_name,unique"`
 Name2 string `gorm:"size:255;index:idx_name_2,unique"`
}
// Rename index name
db.Migrator().RenameIndex(&User{}, "Name", "Name2")
db.Migrator().RenameIndex(&User{}, "idx_name", "idx_name_2")
```
