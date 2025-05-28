[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/Eu-CByJh)
|    NRP     |      Name      |
| :--------: | :------------: |
| 5025241024 | Muhammad Nabil Fauzan |
| 5025241026 | Very Ardiansyah |
| 5025241035 | Acquirell Kriswanto |
| 5025241052 | Muhammad Hilbran Akmal Abrar |

# Praktikum Modul 3 _(Module 3 Lab Work)_

### Laporan Resmi Praktikum Modul 3 _(Module 3 Lab Work Report)_

Di suatu pagi hari yang cerah, Budiman salah satu mahasiswa Informatika ditugaskan oleh dosennya untuk membuat suatu sistem operasi sederhana. Akan tetapi karena Budiman memiliki keterbatasan, Ia meminta tolong kepadamu untuk membantunya dalam mengerjakan tugasnya. Bantulah Budiman untuk membuat sistem operasi sederhana!

_One sunny morning, Budiman, an Informatics student, was assigned by his lecturer to create a simple operating system. However, due to Budiman's limitations, he asks for your help to assist him in completing his assignment. Help Budiman create a simple operating system!_

### Soal 1

> Sebelum membuat sistem operasi, Budiman diberitahu dosennya bahwa Ia harus melakukan beberapa tahap terlebih dahulu. Tahap-tahapan yang dimaksud adalah untuk **mempersiapkan seluruh prasyarat** dan **melakukan instalasi-instalasi** sebelum membuat sistem operasi. Lakukan seluruh tahapan prasyarat hingga [perintah ini](https://github.com/arsitektur-jaringan-komputer/Modul-Sisop/blob/master/Modul3/README-ID.md#:~:text=sudo%20apt%20install%20%2Dy%20busybox%2Dstatic) pada modul!

> _Before creating the OS, Budiman was informed by his lecturer that he must complete several steps first. The steps include **preparing all prerequisites** and **installing** before creating the OS. Complete all the prerequisite steps up to [this command](https://github.com/arsitektur-jaringan-komputer/Modul-Sisop/blob/master/Modul3/README-ID.md#:~:text=sudo%20apt%20install%20%2Dy%20busybox%2Dstatic) in the module!_

**Answer:**

- **Code:**

  ```
  wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.1.1.tar.xz
  tar -xvf linux-6.1.1.tar.xz
  cd linux-6.1.1
  make tinyconfig
  make menuconfig
  make -j$(nproc)
  cp arch/x86/boot/bzImage ..

- **Explanation:**

  `Pengerjaan sesuai dengan modul`

- **Screenshot:**
![Soal 1 Busybox](https://github.com/user-attachments/assets/a04544b5-e6da-462a-a147-6368d54e1626)
![Soal 1 bzImage](https://github.com/user-attachments/assets/0ecfdec6-c902-49db-a1dd-c7a300b5fbf1)


### Soal 2

> Setelah seluruh prasyarat siap, Budiman siap untuk membuat sistem operasinya. Dosen meminta untuk sistem operasi Budiman harus memiliki directory **bin, dev, proc, sys, tmp,** dan **sisop**. Lagi-lagi Budiman meminta bantuanmu. Bantulah Ia dalam membuat directory tersebut!

> _Once all prerequisites are ready, Budiman is ready to create his OS. The lecturer asks that the OS should contain the directories **bin, dev, proc, sys, tmp,** and **sisop**. Help Budiman create these directories!_

**Answer:**

- **Code:**

  ```
  sudo bash
   mkdir -p /myramdisk/{bin,dev,proc,sys,etc,root,home,tmp,sisop}
  cp -a /dev/null /myramdisk/dev
  cp -a /dev/tty* /myramdisk/dev
  cp -a /dev/zero /myramdisk/dev
  cp -a /dev/console /myramdisk/dev
  cp /usr/bin/busybox /myramdisk/bin
  cd /myramdisk/bin
  ./busybox --install .`

- **Explanation:**

  ```
  sudo bash
  Menjalankan shell sebagai superuser (root).
  
  mkdir -p /myramdisk/{bin,dev,proc,sys,etc,root,home,tmp,sisop}
  Membuat struktur folder dasar untuk sistem di dalam direktori /myramdisk.
  
  cp -a /dev/null /myramdisk/dev
  Menyalin perangkat null ke folder dev dalam ramdisk.
  
  cp -a /dev/tty* /myramdisk/dev
  Menyalin semua perangkat terminal (tty) ke ramdisk.
  
  cp -a /dev/zero /myramdisk/dev
  Menyalin perangkat zero ke ramdisk.
  
  cp -a /dev/console /myramdisk/dev
  Menyalin perangkat console ke ramdisk.
  
  cp /usr/bin/busybox /myramdisk/bin
  Menyalin file busybox ke folder bin di ramdisk.
  
  cd /myramdisk/bin
  Berpindah ke direktori bin dalam ramdisk.
  
  ./busybox --install .
  Menginstall semua perintah dasar dari busybox ke direktori saat ini.
  ```


- **Screenshot:**

![Soal 2](https://github.com/user-attachments/assets/1408e6ae-b13c-4aae-9f2a-a83a52c850f4)

### Soal 3

> Budiman lupa, Ia harus membuat sistem operasi ini dengan sistem **Multi User** sesuai permintaan Dosennya. Ia meminta kembali kepadamu untuk membantunya membuat beberapa user beserta directory tiap usernya dibawah directory `home`. Buat pula password tiap user-usernya dan aplikasikan dalam sistem operasi tersebut!

> _Budiman forgot that he needs to create a **Multi User** system as requested by the lecturer. He asks your help again to create several users and their corresponding home directories under the `home` directory. Also set each user's password and apply them in the OS!_

**Format:** `user:pass`

  ```
  root:Iniroot
  Budiman:PassBudi
  guest:guest
  praktikan1:praktikan1
  praktikan2:praktikan2
  ```

**Answer:**

- **Code:**
  
  ```
  sudo bash
  
  cd /myramdisk/home
  
  mkdir Budiman guest praktikan1 praktikan2
  
  openssl passwd -1 Iniroot
  openssl passwd -1 PassBudi
  openssl passwd -1 guest 
  openssl passwd -1 praktikan1
  openssl passwd -1 praktikan2
  
  nano /myramdisk/etc/passwd
  
  MASUKKAN INI
  
  root:$1$hxOKj5F1$VW.7ZGv4P6UNwmm3Pmb3P.:0:0:root:/root:/bin/sh
  Budiman:$1$sC0NMCP.$LAT3f.76keNUVhOXifqB01:1001:100:Budiman:/home/Budiman:/bin/sh
  guest:$1$.UDBAHdm$lQCu8VMvAs7u3aKlletPy0:1002:100:guest:/home/guest:/bin/sh
  praktikan1:$1$hJEEi68z$76/FijjM7L0618RMW.dNq0:1003:100:praktikan1:/home/praktikan1:/bin/sh
  praktikan2:$1$Bxz8mGvi$bXWfsaO1k9zU4XDwcYim2/:1004:100:praktikan2:/home/praktikan2:/bin/sh
  
  nano /myramdisk/etc/group
  
  MASUKKAN INI
  
  root:x:0:
  bin:x:1:root
  sys:x:2:root
  tty:x:5:root,Budiman,guest,praktikan1,praktikan2
  disk:x:6:root
  wheel:x:10:root,Budiman,guest,praktikan1,praktikan2
  users:x:100:Budiman,guest,praktikan1,praktikan2
  ```


- **Explanation:**
  
  ```
  sudo bash
  Masuk ke mode super user
  
  cd /myramdisk/home
  pindah direktori sekarang menjadi /myramdisk/home
  
  mkdir Budiman guest praktikan1 praktikan2
  membuat direktori Budiman, guest, praktikan1, praktikan2
  
  root:$1$hxOKj5F1$VW.7ZGv4P6UNwmm3Pmb3P.:0:0:root:/root:/bin/sh
  Budiman:$1$sC0NMCP.$LAT3f.76keNUVhOXifqB01:1001:100:Budiman:/home/Budiman:/bin/sh
  guest:$1$.UDBAHdm$lQCu8VMvAs7u3aKlletPy0:1002:100:guest:/home/guest:/bin/sh
  praktikan1:$1$hJEEi68z$76/FijjM7L0618RMW.dNq0:1003:100:praktikan1:/home/praktikan1:/bin/sh
  praktikan2:$1$Bxz8mGvi$bXWfsaO1k9zU4XDwcYim2/:1004:100:praktikan2:/home/praktikan2:/bin/sh
  Gunakan openssl untuk membuat password root yang dienkripsi untuk setiap password user.
  
  nano /myramdisk/etc/group
  buka command line text editor untuk group
  
  root:x:0:
  bin:x:1:root
  sys:x:2:root
  tty:x:5:root,Budiman,guest,praktikan1,praktikan2
  disk:x:6:root
  wheel:x:10:root,Budiman,guest,praktikan1,praktikan2
  users:x:100:Budiman,guest,praktikan1,praktikan2
  Masukan pengaturan group untuk user-user baru
  ```
- **Screenshot:**

![Soal 3 Add user](https://github.com/user-attachments/assets/1f96f52e-d2de-4596-a488-e4030101c9c1)

### Soal 4

> Dosen meminta Budiman membuat sistem operasi ini memilki **superuser** layaknya sistem operasi pada umumnya. User root yang sudah kamu buat sebelumnya akan digunakan sebagai superuser dalam sistem operasi milik Budiman. Superuser yang dimaksud adalah user dengan otoritas penuh yang dapat mengakses seluruhnya. Akan tetapi user lain tidak boleh memiliki otoritas yang sama. Dengan begitu user-user selain root tidak boleh mengakses `./root`. Buatlah sehingga tiap user selain superuser tidak dapat mengakses `./root`!

> _The lecturer requests that the OS must have a **superuser** just like other operating systems. The root user created earlier will serve as the superuser in Budiman's OS. The superuser should have full authority to access everything. However, other users should not have the same authority. Therefore, users other than root should not be able to access `./root`. Implement this so that non-superuser accounts cannot access `./root`!_

**Answer:**

- **Code:**

  ```
  sudo bash
  cd /myramdisk
  chown 0:0 root 
  chmod 700 root
  nano init
  ```
  **Isi file init:**
  ```
  #!/bin/sh
  /bin/mount -t proc none /proc
  /bin/mount -t sysfs none /sys
  
  while true
  do
      /bin/getty -L tty1 115200 vt100
      sleep 1
  done
  ```
  
  ```
  chmod +x init
  find . | cpio -oHnewc | gzip > ../myramdisk.gz
  exit
  cd /
  
  cp myramdisk.gz ~/osboot
  cd ~/osboot
  
  qemu-system-x86_64 \
  -smp 2 \
  -m 256 \
  -display curses \
  -vga std \
  -kernel bzImage \
  -initrd myramdisk.gz
   ```

- **Explanation:**

    ```
  sudo bash
  Menjalankan shell sebagai root untuk melakukan konfigurasi sistem.
  
  cd /myramdisk
  Masuk ke direktori initramfs yang akan dikemas.
  
  chown 0:0 root
  Mengatur owner dan group direktori root menjadi root (UID 0, GID 0).
  
  chmod 700 root
  Mengizinkan hanya user root untuk membaca, menulis, dan mengakses direktori /root.
  
  nano init
  Membuat file init yang akan dijalankan pertama kali saat OS boot.
  
  #!/bin/sh
  Shebang: menentukan bahwa skrip dijalankan dengan shell /bin/sh.
  
  /bin/mount -t proc none /proc
  Mount filesystem proc.
  
  /bin/mount -t sysfs none /sys
  Mount filesystem sys.
  
  getty -L tty1 115200 vt100
  Menampilkan login terminal di tty1.
  
  chmod +x init
  Memberi hak eksekusi pada file init.
  
  find . | cpio -oHnewc | gzip > ../myramdisk.gz
  Mengarsipkan seluruh sistem ke file initramfs berformat gzip.
  
  exit, cd /
  Keluar dari shell root, kembali ke direktori root.
  
  cp myramdisk.gz ~/osboot
  Menyalin file initramfs ke folder kerja QEMU.
  
  qemu-system-x86_64 ...
  Menjalankan QEMU dengan kernel dan initrd yang telah dibuat.
  ```

- **Screenshot:**
![Soal 4 Root exc](https://github.com/user-attachments/assets/41d7b43d-e8f1-4683-89f8-06b25897976d)

### Soal 5

> Setiap user rencananya akan digunakan oleh satu orang tertentu. **Privasi dan otoritas tiap user** merupakan hal penting. Oleh karena itu, Budiman ingin membuat setiap user hanya bisa mengakses dirinya sendiri dan tidak bisa mengakses user lain. Buatlah sehingga sistem operasi Budiman dalam melakukan hal tersebut!

> _Each user is intended for an individual. **Privacy and authority** for each user are important. Therefore, Budiman wants to ensure that each user can only access their own files and not those of others. Implement this in Budiman's OS!_

**Answer:**

- **Code:**
  ```
  sudo bash
  cd /myramdisk
  
  chown 1001:100 home/Budiman
  chown 1002:100 home/guest 
  chown 1003:100 home/praktikan1
  chown 1004:100 home/praktikan2
  
  chmod 700 home/Budiman 
  chmod 700 home/guest 
  chmod 700 home/praktikan1 
  chmod 700 home/praktikan2
  
  find . | cpio -oHnewc | gzip > ../myramdisk.gz
  exit
  cd /
  cp myramdisk.gz ~/osboot
  cd ~/osboot
  
  qemu-system-x86_64 \
  -smp 2 \
  -m 256 \
  -display curses \
  -vga std \
  -kernel bzImage \
  -initrd myramdisk.gz
  ```

- **Explanation:**

  ```
  chown UID:GID home/USERNAME
  Mengatur kepemilikan direktori sesuai user dan grup masing-masing.
  
  chmod 700 home/USERNAME
  Memberi izin hanya kepada pemilik untuk mengakses, membaca, dan menulis direktori tersebut.
  
  Langkah find ... hingga qemu-system-x86_64 mirip seperti sebelumnya, yaitu membungkus sistem dan menjalankannya dengan QEMU.
  ```

- **Screenshot:**

![Soal 5 Private](https://github.com/user-attachments/assets/a0a67ff1-fed7-4d85-90c0-f7a8e4d4623e)

### Soal 6

> Dosen Budiman menginginkan sistem operasi yang **stylish**. Budiman memiliki ide untuk membuat sistem operasinya menjadi stylish. Ia meminta kamu untuk menambahkan tampilan sebuah banner yang ditampilkan setelah suatu user login ke dalam sistem operasi Budiman. Banner yang diinginkan Budiman adalah tulisan `"Welcome to OS'25"` dalam bentuk **ASCII Art**. Buatkanlah banner tersebut supaya Budiman senang! (Hint: gunakan text to ASCII Art Generator)

> _Budiman wants a **stylish** operating system. Budiman has an idea to make his OS stylish. He asks you to add a banner that appears after a user logs in. The banner should say `"Welcome to OS'25"` in **ASCII Art**. Use a text to ASCII Art generator to make Budiman happy!_ (Hint: use a text to ASCII Art generator)

**Answer:**

- **Code:**
  ```
  sudo bash
  nano /myramdisk/etc/profile
  ```

  **isi yang dimasukkan ke dalam file**
   ```
  cat << "EOF"
  
  __        __   _                            _
  \ \      / /__| | ___ ___  _ __ ___   ___  | |_ ___
   \ \ /\ / / _ \ |/ __/ _ \| '_ ` _ \ / _ \ | __/ _ \
    \ V  V /  __/ | (_| (_) | | | | | |  __/ | || (_) |
     \_/\_/ \___|_|\___\___/|_| |_| |_|\___|  \__\___/
  
  
     ____   _____ _ ___  _____
    / __ \ / ____( )__ \| ____|
   | |  | | (___ |/   ) | |__
   | |  | |\___ \    / /|___ \
   | |__| |____) |  / /_ ___) |
    \____/|_____/  |____|____/
  
  EOF
  ```
  ```
  cd /myramdisk
  find . | cpio -oHnewc | gzip > ../myramdisk.gz
  exit
  cd /
  
  cp myramdisk.gz ~/osboot
  cd ~/osboot
  
  qemu-system-x86_64 \
  -smp 2 \
  -m 256 \
  -display curses \
  -vga std \
  -kernel bzImage \
  -initrd myramdisk.gz
  ```


- **Explanation:**

  ```
  nano /etc/profile
  File konfigurasi shell untuk semua user yang login.
  
  cat << "EOF" ... EOF
  Menambahkan block ASCII banner agar ditampilkan setiap login shell.
  
  Setelah konfigurasi selesai, sistem kembali dikemas dan dijalankan ulang dengan QEMU agar banner tampil saat booting dan login.
  ```

- **Screenshot:**

![Soal 6 Banner](https://github.com/user-attachments/assets/81f5f994-55c2-410b-88da-4586d16f5b82)

### Soal 7

> Melihat perkembangan sistem operasi milik Budiman, Dosen kagum dengan adanya banner yang telah kamu buat sebelumnya. Kemudian Dosen juga menginginkan sistem operasi Budiman untuk dapat menampilkan **kata sambutan** dengan menyebut nama user yang login. Sambutan yang dimaksud berupa kalimat `"Helloo %USER"` dengan `%USER` merupakan nama user yang sedang menggunakan sistem operasi. Kalimat sambutan ini ditampilkan setelah user login dan setelah banner. Budiman kembali lagi meminta bantuanmu dalam menambahkan fitur ini.

> _Seeing the progress of Budiman's OS, the lecturer is impressed with the banner you created. The lecturer also wants the OS to display a **greeting message** that includes the name of the user who logs in. The greeting should say `"Helloo %USER"` where `%USER` is the name of the user currently using the OS. This greeting should be displayed after user login and after the banner. Budiman asks for your help again to add this feature._

**Answer:**

**Answer:**

- **Code:**
```
sudo bash

nano /myramdisk/etc/profile

# Tambahkan di bawah kode yang sudah ada
echo "Helloo $USER"

cd /myramdisk

find . | cpio -oHnewc | gzip > ../myramdisk.gz

exit
cd /

cp myramdisk.gz ~/osboot

cd ~/osboot

qemu-system-x86_64 \
-smp 2 \
-m 256 \
-display curses \
-vga std \
-kernel bzImage \
-initrd myramdisk.gz
```

- **Explanation:**
```
sudo bash
Masuk ke shell root untuk mendapatkan hak akses penuh selama konfigurasi sistem.

nano /myramdisk/etc/profile
Membuka file /etc/profile di dalam sistem operasi custom. File ini dieksekusi setiap kali user login melalui shell, sehingga cocok untuk menambahkan sambutan.

echo "Helloo $USER"
Menambahkan perintah untuk menyapa user yang sedang login. Variabel $USER otomatis berisi nama user aktif, sehingga output-nya akan menjadi seperti:
Helloo Budiman, Helloo guest, dll.

cd /myramdisk
Pindah ke direktori myramdisk untuk membungkus ulang file initramfs.

find . | cpio -oHnewc | gzip > ../myramdisk.gz
Mengarsipkan isi myramdisk ke dalam format initramfs dan mengompresnya menjadi myramdisk.gz.

exit
Keluar dari shell root setelah proses selesai.

cd /
Pindah ke direktori root agar tidak terjadi konflik direktori saat copy file.

cp myramdisk.gz ~/osboot
Menyalin file initramfs hasil modifikasi ke folder osboot tempat simulasi QEMU dijalankan.

cd ~/osboot
Masuk ke folder kerja osboot.

qemu-system-x86_64 ...
Menjalankan sistem operasi yang telah dimodifikasi dengan menyisipkan initramfs baru (myramdisk.gz) dan kernel 
(bzImage).
Output sambutan akan muncul saat user login ke shell.
```

- **Screenshot:**

![Soal 7 Greeting](https://github.com/user-attachments/assets/8e1383a4-5359-49c7-834a-5f7a1979da55)

### Soal 8

> Dosen Budiman sudah tua sekali, sehingga beliau memiliki kesulitan untuk melihat tampilan terminal default. Budiman menginisiatif untuk membuat tampilan sistem operasi menjadi seperti terminal milikmu. Modifikasilah sistem operasi Budiman menjadi menggunakan tampilan terminal kalian.

> _Budiman's lecturer is quite old and has difficulty seeing the default terminal display. Budiman takes the initiative to make the OS look like your terminal. Modify Budiman's OS to use your terminal appearance!_

**Answer:**

- **Code:**

  ```
  # Rebuild kernel
  cd ~/osboot/linux-6.1.1
  nano .config
  
  # Tambahkan ini di bagian "Serial Driver"
  CONFIG_SERIAL_8250=y
  CONFIG_SERIAL_8250_CONSOLE=y
  
  make -j$(nproc)
  cp arch/x86/boot/bzImage ..
  
  sudo bash
  nano /myramdisk/etc/profile
  
  # Tambahkan dibawah kode yang sudah ada
  export PATH=/bin:$PATH
  export PS1='\[\e[1;32m\][\u@\h \W]\$\[\e[0m\] '
  
  nano /myramdisk/init
  
  # Tambahkan sebelum 'while' loop
  hostname OS25
  
  # Edit "tty1" menjadi "ttyS0"
  
  cd /myramdisk
  find . | cpio -oHnewc | gzip > ../myramdisk.gz
  
  exit
  cd /
  cp myramdisk.gz ~/osboot
  
  cd ~/osboot
  
  qemu-system-x86_64 \                 
    -kernel bzImage \
    -initrd myramdisk.gz \
    -append "console=ttyS0" \
    -nographic \
    -serial mon:stdio
  ```
- **Explanation:**
  ```
  Rebuild kernel
  cd ~/osboot/linux-6.1.1
  Masuk ke direktori sumber Linux.
  
  nano .config
  Buka file konfigurasi kernel.
  
  Tambahkan ini di bagian "Serial Driver"
  CONFIG_SERIAL_8250=y
  CONFIG_SERIAL_8250_CONSOLE=y
  Aktifkan driver serial 8250 dan dukungan konsol dalam konfigurasi kernel.
  
  make -j$(nproc)
  Bangun kernel menggunakan semua core prosesor yang tersedia.
  
  cp arch/x86/boot/bzImage ..
  Salin image kernel yang telah dikompilasi ke direktori atas.
  
  sudo bash
  Masuk ke shell root.
  
  nano /myramdisk/etc/profile
  Edit file profil di ramdisk.
  
  Tambahkan dibawah kode yang sudah ada
  export PATH=/bin:$PATH
  export PS1='\[\e[1;32m\][\u@\h \W]\$\[\e[0m\] '
  Atur path binary dan sesuaikan tampilan prompt terminal.
  
  nano /myramdisk/init
  Buka script init untuk diedit.
  
  Tambahkan sebelum 'while' loop
  hostname OS25
  Atur hostname sistem menjadi "OS25".
  
  Edit "tty1" menjadi "ttyS0"
  Ubah perangkat konsol dari tty1 ke ttyS0 untuk output serial.
  
  cd /myramdisk
  Masuk ke direktori myramdisk.
  
  find . | cpio -oHnewc | gzip > ../myramdisk.gz
  Kemasi ramdisk menjadi initrd yang terkompresi dengan gzip.
  
  exit
  Keluar dari shell root.
  
  cd /
  Kembali ke direktori root.
  
  cp myramdisk.gz ~/osboot
  Salin image ramdisk baru ke folder osboot.
  
  cd ~/osboot
  Masuk ke direktori osboot.
  
  qemu-system-x86_64 \                 
    -kernel bzImage \
    -initrd myramdisk.gz \
    -append "console=ttyS0" \
    -nographic \
    -serial mon:stdio
  Jalankan QEMU dengan kernel dan ramdisk baru menggunakan output konsol serial.
  ```
- **Screenshot:**

![Soal 8](https://github.com/user-attachments/assets/399fa994-a55c-4a26-ac1b-ae1456a167d7)

### Soal 9

> Ketika mencoba sistem operasi buatanmu, Budiman tidak bisa mengubah text file menggunakan text editor. Budiman pun menyadari bahwa dalam sistem operasi yang kamu buat tidak memiliki text editor. Budimanpun menyuruhmu untuk menambahkan **binary** yang telah disiapkan sebelumnya ke dalam sistem operasinya. Buatlah sehingga sistem operasi Budiman memiliki **binary text editor** yang telah disiapkan!

> _When trying your OS, Budiman cannot edit text files using a text editor. He realizes that the OS you created does not have a text editor. Budiman asks you to add the prepared **binary** into his OS. Make sure Budiman's OS has the prepared **text editor binary**!_

**Answer:**

- **Code:**
  ```
  download (main.cpp dan TextEditor.h) dari https://github.com/morisab/budiman-text-editor
  
  cd ~/Download
  
  g++ -static -o myeditor main.cpp 
  
  sudo bash  
  
  cp myeditor /myramdisk/bin/              
  chmod +x /myramdisk/bin/myeditor
  
  nano /myramdisk/etc/profile
  
  TAMBAHKAN DIBAWAH KODE YANG SUDAH ADA
  alias editor='/bin/myeditor'
  
  cd /myramdisk
  
  find . | cpio -oHnewc | gzip > ../myramdisk.gz
  
  exit
  cd /
  
  cp myramdisk.gz ~/osboot
  
  cd ~/osboot
  
  qemu-system-x86_64 \
  -smp 2 \
  -m 256 \
  -display curses \
  -vga std \
  -kernel bzImage \
  -initrd myramdisk.gz
  
  TEST TEXT EDITORMU DI QEMU DENGAN FORMAT SYNTAX
  editor <filename>
  ```


- **Explanation:**

  ```
  download (main.cpp dan TextEditor.h) dari https://github.com/morisab/budiman-text-editor
  Download source code editor dari GitHub.
  
  cd ~/Download
  Masuk ke folder tempat file hasil download berada.
  
  g++ -static -o myeditor main.cpp 
  Compile file `main.cpp` menjadi binary statik bernama `myeditor`.
  
  sudo bash  
  Masuk ke shell root.
  
  cp myeditor /myramdisk/bin/              
  chmod +x /myramdisk/bin/myeditor
  Salin binary ke dalam ramdisk dan ganti permission
  
  nano /myramdisk/etc/profile
  Buka file profile ramdisk untuk diedit.
  
  TAMBAHKAN DIBAWAH KODE YANG SUDAH ADA
  alias editor='/bin/myeditor'
  Tambahkan alias agar perintah `editor` memanggil binary `myeditor`.
  
  cd /myramdisk
  Masuk ke direktori myramdisk.
  
  find . | cpio -oHnewc | gzip > ../myramdisk.gz
  Buat kembali file initrd ramdisk yang sudah diperbarui.
  
  exit
  cd /
  Keluar dari root shell dan kembali ke root directory.
  
  cp myramdisk.gz ~/osboot
  Salin ramdisk baru ke folder osboot.
  
  cd ~/osboot
  Masuk ke direktori osboot.
  
  qemu-system-x86_64 \
  -smp 2 \
  -m 256 \
  -display curses \
  -vga std \
  -kernel bzImage \
  -initrd myramdisk.gz
  Jalankan sistem menggunakan QEMU dengan 2 core, 256MB RAM, dan tampilan teks.
  
  TEST TEXT EDITORMU DI QEMU DENGAN FORMAT SYNTAX
  editor <filename>
  Coba jalankan text editor buatanmu di dalam QEMU dengan memberi nama file.
  ```


- **Screenshot:**
![Soal 9 Open Editor](https://github.com/user-attachments/assets/cdbdff10-6481-4254-9bd9-1a4d27a114f0)

![Soal 9 Text Editor](https://github.com/user-attachments/assets/18f03155-4599-44f2-aca3-437f681bc51b)


### Soal 10

> Setelah seluruh fitur yang diminta Dosen dipenuhi dalam sistem operasi Budiman, sudah waktunya Budiman mengumpulkan tugasnya ini ke Dosen. Akan tetapi, Dosen Budiman tidak mau menerima pengumpulan selain dalam bentuk **.iso**. Untuk terakhir kalinya, Budiman meminta tolong kepadamu untuk mengubah seluruh konfigurasi sistem operasi yang telah kamu buat menjadi sebuah **file .iso**.

> After all the features requested by the lecturer have been implemented in Budiman's OS, it's time for Budiman to submit his assignment. However, Budiman's lecturer only accepts submissions in the form of **.iso** files. For the last time, Budiman asks for your help to convert the entire configuration of the OS you created into a **.iso file**.

**Answer:**

- **Code:**

  ```
  cd ~/osboot

  mkdir -p mylinuxiso/boot/grub

  cp bzImage mylinuxiso/boot
  cp myramdisk.gz mylinuxiso/boot

  nano mylinuxiso/boot/grub/grub.cfg
  
  MASUKKAN INI
  set timeout=5
  set default=0
  
  menuentry "MyLinux" {
    linux /boot/bzImage console=ttyS0,115200
    initrd /boot/myramdisk.gz
  }

  grub-mkrescue -o mylinux.iso mylinuxiso

  qemu-system-x86_64 \
    -cdrom mylinux.iso \
    -nographic \
    -serial mon:stdio
  ```


- **Explanation:**

  ```
  cd ~/osboot
  Masuk ke direktori osboot.
  
  mkdir -p mylinuxiso/boot/grub
  Buat struktur direktori untuk ISO, termasuk folder GRUB.
  
  cp bzImage mylinuxiso/boot
  cp myramdisk.gz mylinuxiso/boot
  Salin kernel dan ramdisk ke direktori boot dalam ISO.
  
  nano mylinuxiso/boot/grub/grub.cfg
  Buka file configuration GRUB untuk diedit.
  
  MASUKKAN INI
  set timeout=5
  set default=0
  
  menuentry "MyLinux" {
    linux /boot/bzImage console=ttyS0,115200
    initrd /boot/myramdisk.gz
  }
  Tambahkan configuration GRUB untuk menjalankan kernel dan ramdisk.
  
  grub-mkrescue -o mylinux.iso mylinuxiso
  Buat file ISO bootable bernama mylinux.iso.
  
  qemu-system-x86_64 \
    -cdrom mylinux.iso \
    -nographic \
    -serial mon:stdio
  Jalankan ISO di QEMU menggunakan serial console dan tanpa tampilan grafis.
  ```


- **Screenshot:**

![Soal 10 Iso](https://github.com/user-attachments/assets/ce9cf29b-ae20-45a4-a73c-1750765fdeb6)

---

Pada akhirnya sistem operasi Budiman yang telah kamu buat dengan susah payah dikumpulkan ke Dosen mengatasnamakan Budiman. Kamu tidak diberikan credit apapun. Budiman pun tidak memberikan kata terimakasih kepadamu. Kamupun kecewa tetapi setidaknya kamu telah belajar untuk menjadi pembuat sistem operasi sederhana yang andal. Selamat!

_At last, the OS you painstakingly created was submitted to the lecturer under Budiman's name. You received no credit. Budiman didn't even thank you. You feel disappointed, but at least you've learned to become a reliable creator of simple operating systems. Congratulations!_
