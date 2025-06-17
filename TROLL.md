# **Drama Troll**

Dalam sebuah perusahaan penuh drama fandom, seorang karyawan bernama DainTontas telah menyinggung komunitas Gensh _ n, Z _ Z, dan Wut \* ering secara bersamaan. Akibatnya, dua rekan kerjanya, SunnyBolt dan Ryeku, merancang sebuah troll dengan gaya khas: membuat filesystem jebakan menggunakan FUSE.

Mereka membutuhkan bantuanmu, ahli Sistem Operasi, untuk menciptakan filesystem kustom yang bisa mengecoh DainTontas, langsung dari terminal yang dia cintai.

## **a. Pembuatan User**

Buat 3 user di sistem sebagai berikut yang merepresentasikan aktor-aktor yang terlibat dalam _trolling_ kali ini, yaitu:

- DainTontas
- SunnyBolt
- Ryeku

Gunakan `useradd` dan `passwd` untuk membuat akun lokal. User ini akan digunakan untuk mengakses filesystem FUSE yang kamu buat.

## **b. Jebakan Troll**

Untuk menjebak DainTontas, kamu harus menciptakan sebuah filesystem FUSE yang dipasang di `/mnt/troll`. Di dalamnya, hanya akan ada dua file yang tampak sederhana:

- `very_spicy_info.txt` - umpan utama.
- `upload.txt` - tempat DainTontas akan "secara tidak sadar" memicu jebakan.

## **c. Jebakan Troll (Berlanjut)**

Setelah membuat dua file tersebut, SunnyBolt dan Ryeku memintamu untuk membuat jebakan yang telah dirancang oleh mereka. SunnyBolt dan Ryeku yang merupakan kolega lama DainTontas dan tahu akan hal-hal memalukan yang dia pernah lakukan, akan menaruh aib-aib tersebut pada file `very_spicy_info.txt`. Apabila dibuka dengan menggunakan command `cat` oleh user selain DainTontas, akan memberikan output seperti berikut:

```
DainTontas' personal secret!!.txt
```

Untuk mengecoh DainTontas, apabila DainTontas membuka `very_spicy_info.txt`, isinya akan berupa seperti berikut:

```
Very spicy internal developer information: leaked roadmap.docx
```

## **d. Trap**

Suatu hari, DainTontas mengecek PC dia dan menemukan bahwa ada sebuah file baru, `very_spicy_info.txt`. Dia kemudian membukanya dan menemukan sebuah leak roadmap. Karena dia ingin menarik perhatian, dia akan mengupload file tersebut ke sosial media `Xitter` dimana _followernya_ dapat melihat dokumen yang bocor tersebut. Karena dia pernah melakukan hal ini sebelumnya, dia punya script bernama `upload.txt` yang dimana ketika dia melakukan `echo` berisi `upload` pada file tersebut, dia akan otomatis mengupload sebuah file ke akun `Xitter` miliknya.

Namun ini semua adalah rencana SunnyBolt dan Ryeku. Atas bantuanmu, mereka telah mengubah isi dari `upload.txt`. Apabila DainTontas melakukan upload dengan metode `echo` miliknya, maka dia dinyatakan telah "membocorkan" info leak tersebut. Setelah DainTontas membocorkan file tersebut, semua file berekstensi `.txt` akan selalu memunculkan sebuah ASCII Art dari kalimat

```
Fell for it again reward
```

Setelah seharian penuh mencoba untuk membetulkan PC dia, dia baru sadar bahwa file `very_spicy_info.txt` yang dia sebarkan melalui script uploadnya tersebut teryata berisikan aib-aib dia. Dia pun ditegur oleh SunnyBolt dan Ryeku, dan akhirnya dia pun tobat.

## **NOTES**

- `upload.txt` hanyalah berupa file kosong yang dipakai untuk trigger _behavior change_ dari `cat` milik DainTontas. Deskripsi di soal hanyalah sebagai _storytelling_ belaka.
- Trigger dari Trap akan bersifat `persist` bahkan ketika FUSE telah di-unmount ataupun ketika mesin dimatikan/direstart

---

# **Drama Troll**

In a company rife with fandom drama, an employee named DainTontas has managed to offend the Gensh _ n, Z _ Z, and Wut \* ering communities simultaneously. As a result, two of his colleagues, SunnyBolt and Ryeku, have devised a troll with a unique flair: creating a booby-trapped filesystem using FUSE.

They need your help, an Operating Systems expert, to create a custom filesystem that can deceive DainTontas—right from the terminal he loves so much.

## **a. User Creation**

Create 3 users on the system to represent the actors involved in this _trolling_ escapade:

- DainTontas
- SunnyBolt
- Ryeku

Use `useradd` and `passwd` to create these local accounts. These users will be used to access the FUSE filesystem you are about to create.

## **b. The Troll Trap**

To ensnare DainTontas, you must create a FUSE filesystem mounted at `/mnt/troll`. Inside, there will only be two seemingly simple files:

- `very_spicy_info.txt` - the main bait
- `upload.txt` - the file DainTontas will use to unwittingly spring the trap.

## **c. The Troll Trap (Continued)**

After creating the two files, SunnyBolt and Ryeku ask you to implement the trap they've designed. As old colleagues, they know plenty of embarrassing things about DainTontas and plan to put this "dirt" into the `very_spicy_info.txt` file. When this file is opened with the `cat` command by any user other than DainTontas, it should display the following output:

```
DainTontas' personal secret!!.txt
```

To deceive DainTontas, if he opens `very_spicy_info.txt`, the content should appear as:

```
Very spicy internal developer information: leaked roadmap.docx
```

## **d. The Trap**

One day, DainTontas checks his PC and discovers a new file, `very_spicy_info.txt`. He opens it and finds what he believes to be a leaked product roadmap. Eager for attention, he decides to upload the file to the social media platform `Xitter`, where his followers can see the "leaked" document. He has a script for this, `upload.txt`, which, when he runs `echo "upload" > upload.txt`, will automatically post a file to his `Xitter` account.

However, this is all part of SunnyBolt and Ryeku's plan. With your help, they've altered the behavior of `upload.txt`. When DainTontas uses his echo method to "upload" the file, he is considered to have "leaked" the information. After he triggers the trap, all files with a `.txt` extension will perpetually display an ASCII art of the phrase:

```
Fell for it again reward
```

After a full day of trying to fix his PC, he finally realizes that the `very_spicy_info.txt` file he spread via his upload script actually contained his own embarrassing secrets. He is confronted by SunnyBolt and Ryeku and, at last, sees the error of his ways.

## **NOTES**

- `upload.txt` is just an empty file used to trigger the behavior change for DainTontas's cat command. The description in the problem is purely for storytelling.
- The trap's trigger must be persistent, even when the FUSE filesystem is unmounted or the machine is shut down/restarted.
