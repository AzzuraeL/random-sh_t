[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/tPVgLsdF)
| Name | NRP | Class |
| ---- | --- | ----- |
| Very Ardiansyah | 5025241026 | B |

## Task 1

- Flag

  `JARKOM25{Ja0G_Bbbb4ng3t_S1_UTP661J0YUR9KCGVSJO871RR6CWXA60xl0vel1x98yzvk5shf410v7u2ovbb7_2f3f77bae916a5d950e9b3f4e07666f5}`

> a. Berapa banyak packet yang terekam pada file pcapng?

> _a. How many packets are recorded in the pcapng file?_

**Answer:** `9596`

- Filter expression

  `-`

- Explanation

  Dapat dilihat dari gambar berikut ada tuisan `"Packet : 9596 ...."` dibagian bawah kanan
  <img width="1920" height="1080" alt="Screenshot from 2025-09-13 11-32-32" src="https://github.com/user-attachments/assets/76d3c08f-93de-4313-9a46-1160f8186554" />

  

- Output result

  <img width="794" height="207" alt="image" src="https://github.com/user-attachments/assets/45047157-603e-40b4-bf6f-97910b892d0d" />


<br>
<br>

> b. Ada berapa jenis protocol (total) yang terekam pada traffic?

> _b. How many types of protocol (totals) are recorded in the traffic?_

**Answer:** `12`

- Filter expression

  `-`

- Explanation

  Gunakan bar atas untuk masuk ke `Statistics -> Protocol Hierarchy` dan hitung dari atas kebawah ada berapa, dapat terlihat dari gambar ada 12
  <img width="1920" height="1080" alt="Screenshot from 2025-09-13 11-32-12" src="https://github.com/user-attachments/assets/e3a83d68-a188-4b82-8ac9-2d7b565e1efa" />


- Output result

  <img width="794" height="251" alt="image" src="https://github.com/user-attachments/assets/8034973a-18b5-4b83-abda-be6612575211" />


<br>
<br>

> c. Ada berapa jenis protocol berbasis TCP yang terekam pada traffic?

> _c. How many types of TCP-based applications protocol are recorded in the traffic?_

**Answer:** `8`

- Filter expression

  `-`

- Explanation

  Gunakan bar atas untuk masuk ke `Statistics -> Protocol Hierarchy` dan hitung dari atas kebawah yang ada dibawah `"Transmission Control Protocol"` ada berapa, dapat terlihat dari gambar ada 8
  <img width="1920" height="1080" alt="Screenshot from 2025-09-13 11-33-18" src="https://github.com/user-attachments/assets/c4a93bdb-6a38-42c1-a5be-b8bfbdd23426" />


- Output result

  <img width="794" height="309" alt="image" src="https://github.com/user-attachments/assets/833e9a31-59cc-427b-a077-f729b221f2a3" />

  <br>
  <br>

> d. Ada berapa banyak packet dengan protokol TCP murni yang terekam pada traffic (tanpa data)?

> _d. How many packets with pure TCP protocol are recorded in the traffic (without data)?_

**Answer:** `3223`

- Filter expression

  `-`

- Explanation

  Gunakan bar atas untuk masuk ke `Statistics -> Protocol Hierarchy` dan lihat `End Packet` yang ada sebaris dengan `"Transmission Control Protocol"` ada berapa, dapat terlihat dari gambar ada 3223
  <img width="1920" height="1080" alt="Screenshot from 2025-09-13 11-33-18" src="https://github.com/user-attachments/assets/235e5509-62d5-4e9d-be0a-68ff7c08496c" />


- Output result

  <img width="794" height="376" alt="image" src="https://github.com/user-attachments/assets/5b72bf9e-c706-4b1b-9a9d-bf3e01df0302" />


## Task 2

- Flag

  `JARKOM25{N1c3_0ne_b4nggg_MPWWBVFAKDyuMM13yhsvijlqhgidqtpdhuhpkc3r4t0ps12036308977945299358_fbcc76e02b38befb57e8b505c0e68710}`

> a. Berapa banyak packet berhasil yang berbasis murni TCP dan memiliki flag [ACK]?

> _a. How many packets succeed that are pure TCP based and have [ACK] flag?_

**Answer:** `3209`

- Filter expression

  `tcp.len == 0 && !tcp.analysis.lost_segment && !tcp.analysis.retransmission && tcp.flags.ack == 1`

- Explanation

  Gunakan Display Filter yang tercantum di atas untuk melakukan filtering pada packet yang ingin dicari. Dengan demikian akan terlihat apa yang ingin kita cari di kanan bawah dengan tulisan `"...Displayed : 3209 (33.4%)"` maka itulah jawabannya
  <img width="1920" height="1080" alt="Screenshot from 2025-09-13 11-48-03" src="https://github.com/user-attachments/assets/8872c835-d417-4699-9deb-c1472d0712be" />


- Output result

  <img width="794" height="200" alt="image" src="https://github.com/user-attachments/assets/99f4a7cd-d4e5-42b3-aeb6-97548b912a8b" />


  <br>
  <br>

> b. Berapa banyak packet berhasil yang berbasis murni TCP yang hanya memiliki flag [ACK]?

> _b. How many packets succeed that are pure TCP based and have only [ACK] flag?_

**Answer:** `3172`

- Filter expression

  `tcp.len == 0 && !tcp.analysis.lost_segment && !tcp.analysis.retransmission && tcp.flags.ack == 1 && tcp.flags.syn==0 && tcp.flags.fin == 0`

- Explanation

  Gunakan Display Filter yang tercantum di atas untuk melakukan filtering pada packet yang ingin dicari. Dengan demikian akan terlihat apa yang ingin kita cari di kanan bawah dengan tulisan `"...Displayed : 3173 (33.1%)"` , angka ini akan kita kurangi 1 karena ada `"TCP ACKed unseen segment"` jadi jawabannya 3172
  <img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/257cc6bd-6b30-4ea8-a075-cef5175b2aee" />


- Output result
<img width="794" height="293" alt="image" src="https://github.com/user-attachments/assets/9048c78f-3fd5-4e4a-ad17-c1e65ff60358" />

  

  <br>
  <br>

> c. Berapa banyak packet berhasil yang berbasis murni TCP dan memiliki flag selain hanya [ACK]?

> _c. How many packets succeed that are pure TCP based and contain flags other than just [ACK] flag?_

**Answer:** `49`

- Filter expression

  `tcp.len == 0 && !tcp.analysis.lost_segment && !tcp.analysis.retransmission &&  tcp.flags.syn==1 `
  `tcp.len == 0 && !tcp.analysis.lost_segment && !tcp.analysis.retransmission &&  tcp.flags.fin==1 `

- Explanation

 Gunakan Display Filter yang tercantum di atas untuk melakukan filtering pada packet yang ingin dicari. Dengan demikian akan terlihat apa yang ingin kita cari di kanan bawah dengan tulisan `"...Displayed : 24 (0.3%)"` untuk filter pertama dan `"...Displayed : 24 (0.3%)"` untuk filter kedua , angka ini akan kita tambahkan 1 karena ada `"TCP ACKed unseen segment"` jadi totalnya adalah 49
  <img width="1920" height="1080" alt="Screenshot from 2025-09-13 12-15-21" src="https://github.com/user-attachments/assets/354cbd58-e559-46f0-bb31-b5e3fbd9dd66" />
  <img width="1920" height="1080" alt="Screenshot from 2025-09-13 12-17-50" src="https://github.com/user-attachments/assets/fe54a72e-47a5-4446-b921-cc55b525db41" />


- Output result

  <img width="794" height="357" alt="image" src="https://github.com/user-attachments/assets/d4b05d03-f9f6-4ec5-aa97-731662ac6646" />


  <br>
  <br>

## Task 3

- Flag

  `JARKOM25{W0w_Y0uU_h4V33e_d0n3_444_90od_j0bB_OGOP9g0dl1k3cg81nawc8svvricrwlietj_b0964177f3da6a889fd343b29e51c77b}`

> a. Pada port berapa client telnet terbuka?

> _a. In what port is the telnet client open?_

**Answer:** `54184`

- Filter expression

  `telnet`

- Explanation

  Gunakan Display Filter yang tercantum di atas untuk melakukan filtering pada packet yang ingin dicari. Lalu gunakan bar atas untuk masuk ke `Statistiscs -> Conversation -> TCP `disana akan terlihat di `port A `yakni client dengan angka 54184
  <img width="1920" height="1080" alt="Screenshot from 2025-09-13 12-19-28" src="https://github.com/user-attachments/assets/7342d15c-e684-4f32-82a0-626e049708c6" />


- Output result

  <img width="794" height="202" alt="image" src="https://github.com/user-attachments/assets/9b7f05e2-d3a6-4570-81d3-0fef76ae95cf" />


  <br>
  <br>

> b. Berapa byte file response yang dikirim dari server?

> _b. How many bytes of the response files are sent from the server?_

**Answer:** `1449`

- Filter expression

  `telnet`

- Explanation

  Gunakan Display Filter yang tercantum di atas untuk melakukan filtering pada packet yang ingin dicari. Lalu klik kanan manapun dari daftar yang terdisplay masuk ke `Follow -> TCP Stream` kemudian dibagian kiri bawah akan ada `"Entire Conversation"` klik untuk ubah jadi pilihan dimana server mengirim ke client maka akan didapat jawaban 1449
  <img width="1920" height="1080" alt="Screenshot from 2025-09-13 12-20-20" src="https://github.com/user-attachments/assets/5ba3e11f-1a19-4121-acf9-2ce020451cbc" />


- Output result

  <img width="794" height="243" alt="image" src="https://github.com/user-attachments/assets/48304e89-42ad-48c8-bbbf-642bcf724553" />


  <br>
  <br>

> c. Apa username yang digunakan client telnet untuk berhubungan dengan server?

> _c. What telnet client's username is used to connect with the server?_

**Answer:** `jovyan`

- Filter expression

  `telnet`

- Explanation

  Gunakan Display Filter yang tercantum di atas untuk melakukan filtering pada packet yang ingin dicari. Lalu klik kanan manapun dari daftar yang terdisplay masuk ke `Follow -> TCP Stream` kemudian dibagian atas ada tulisan `"login : jovyan"` itulah usernamenya
  <img width="1920" height="1080" alt="Screenshot from 2025-09-13 12-20-20" src="https://github.com/user-attachments/assets/617556ad-6df8-4112-a499-66a7317006e5" />


- Output result

  <img width="794" height="287" alt="image" src="https://github.com/user-attachments/assets/884992f2-8d7f-466a-96dc-4cad3b32ba68" />


  <br>
  <br>

> d. Apa password client telnet?

> _d. What is the telnet client's password?_

**Answer:** `123`

- Filter expression

  `telnet`

- Explanation

  Gunakan Display Filter yang tercantum di atas untuk melakukan filtering pada packet yang ingin dicari. Lalu klik kanan manapun dari daftar yang terdisplay masuk ke `Follow -> TCP Stream` kemudian dibagian atas ada tulisan `"Password : 123"` itulah passwordnya
  <img width="1920" height="1080" alt="Screenshot from 2025-09-13 12-22-07" src="https://github.com/user-attachments/assets/29eebdde-823d-4e6b-a52b-f66815974ef7" />


- Output result

  <img width="794" height="351" alt="image" src="https://github.com/user-attachments/assets/c6876076-6cc3-411a-9817-e349ea440205" />


  <br>
  <br>

## Task 4

- Flag

  `JARKOM25{G04t__a4n4liz333er_7K2X4KUGDEDFYJT6ONSKfr0ga7k7xe39lzenzj2r3ys3539588057_eb012b3df9df2baff54321cb950ed40c}`

> a. Apa perintah pertama yang ditulis client pada koneksi telnet?

> _a. What is the first command that client wrote on telnet connection?_

**Answer:** `echo`

- Filter expression

  `telnet`

- Explanation
Gunakan Display Filter yang tercantum di atas untuk melakukan filtering pada packet yang ingin dicari. Lalu klik kanan manapun dari daftar yang terdisplay masuk ke `Follow -> TCP Stream` kemudian dibagian yang saya blok itu ada petunjuk bahwa client mengetikkan `"echo"` untuk pertama kalinya
<img width="1920" height="1080" alt="Screenshot from 2025-09-13 12-22-07" src="https://github.com/user-attachments/assets/1812dafb-e8e6-4d06-a4e7-d6b96ea66d03" />


- Output result

  <img width="794" height="203" alt="image" src="https://github.com/user-attachments/assets/4659ef15-6c6e-41ad-8e1b-d966ea5b5d53" />


  <br>
  <br>

> b. Apa nama file .txt di server (ditulis bersama ekstensinya)?

> _b. What is the name of .txt file on the server (write with the extension)?_

**Answer:** `test.txt`

- Filter expression

  `telnet`

- Explanation
Gunakan Display Filter yang tercantum di atas untuk melakukan filtering pada packet yang ingin dicari. Lalu klik kanan manapun dari daftar yang terdisplay masuk ke `Follow -> TCP Stream` kemudian dibagian yang saya blok itu ada petunjuk bahwa ketika client mengetikkan `"ls"` untuk keluar output `test.txt`
<img width="1920" height="1080" alt="Screenshot from 2025-09-13 12-22-39" src="https://github.com/user-attachments/assets/840785c5-0fe8-4d84-b85c-bf85bf200447" />



- Output result

  <img width="794" height="244" alt="image" src="https://github.com/user-attachments/assets/095cfdbc-4c57-46ea-8cb7-6c53126d1155" />


  <br>
  <br>

> c. Apa kata pertama dari frasa yang dimasukkan client ke dalam file sebelumnya?

> _c. What is the first word that the client inserted into the previous file?_

**Answer:** `Jarkom`

- Filter expression

  `telnet`

- Explanation

Gunakan Display Filter yang tercantum di atas untuk melakukan filtering pada packet yang ingin dicari. Lalu klik kanan manapun dari daftar yang terdisplay masuk ke `Follow -> TCP Stream` kemudian dibagian yang saya blok itu ada petunjuk bahwa ketika client mengetikkan `echo "Jarkom gampang" >> test.txt` maka kata pertamanya adalah Jarkom
<img width="1920" height="1080" alt="Screenshot from 2025-09-13 12-23-07" src="https://github.com/user-attachments/assets/3d91d0e6-42d0-4a81-8d06-cf85a49da028" />


- Output result

 <img width="794" height="311" alt="image" src="https://github.com/user-attachments/assets/fa69f019-e716-4471-9e3a-fa49665c8a94" />


  <br>
  <br>

## Task 5

- Flag

  `JARKOM25{n4il0ng_m1lk_dr4g000n_M76II0NL935QSS5Z980VTPW8LAHQAAcr0cnktaizrx151gdsdr8lcpb434_2f062b720af882edfd26982fee6c0071}`

> a. Berapa banyak packet berbasis HTTP yang terekam pada file pcapng?

> _a. How many HTTP packets are recorded in the pcapng file?_

**Answer:** `298`

- Filter expression

  `http`

- Explanation

Gunakan Display Filter yang tercantum di atas untuk melakukan filtering pada packet yang ingin dicari. Dengan demikian akan terlihat apa yang ingin kita cari di kanan bawah dengan tulisan `"...Displayed : 298 (0.4%)` maka itulah jawabannya
<img width="1920" height="1080" alt="Screenshot from 2025-09-13 12-24-16" src="https://github.com/user-attachments/assets/a20055f3-080e-48fa-90bc-ebb24c4cc1c9" />


- Output result

<img width="794" height="199" alt="image" src="https://github.com/user-attachments/assets/62fd2066-3d67-40e5-82aa-e0d19e354d75" />


  <br>
  <br>

> b. Ada berapa HTTP packet yang berupa response?

> _b. How many response HTTP packets are recorded in the traffic?_

**Answer:** `149`

- Filter expression

  `http.response`

- Explanation

 Gunakan Display Filter yang tercantum di atas untuk melakukan filtering pada packet yang ingin dicari. Dengan demikian akan terlihat apa yang ingin kita cari di kanan bawah dengan tulisan `"...Displayed : 149 (0.2%)` maka itulah jawabannya
 <img width="1920" height="1080" alt="Screenshot from 2025-09-13 12-24-25" src="https://github.com/user-attachments/assets/327a1e0d-d5cb-48ed-bd5f-30999bd785fa" />


- Output result

  <img width="794" height="238" alt="image" src="https://github.com/user-attachments/assets/ab59d4b9-a21f-4aba-b500-c321d45f47d6" />


  <br>
  <br>

> c. Ada berapa paket berbasis HTTP yang berhasil?

> _c. How many HTTP packets that succeed?_

**Answer:** `296`

- Filter expression

  `http && !tcp.analysis.lost_segment && !tcp.analyis.retransmission`

- Explanation

 Gunakan Display Filter yang tercantum di atas untuk melakukan filtering pada packet yang ingin dicari. Dengan demikian akan terlihat apa yang ingin kita cari di kanan bawah dengan tulisan `"...Displayed : 296 (0.2%)` maka itulah jawabannya
<img width="1920" height="1080" alt="Screenshot from 2025-09-13 12-25-15" src="https://github.com/user-attachments/assets/29554a07-28b5-4084-9d70-cddbf08861e6" />

- Output result

<img width="794" height="284" alt="image" src="https://github.com/user-attachments/assets/ff3a7bc5-9d11-4b71-be5d-58fcc536728c" />


  <br>
  <br>

> d. Apa alamat IP dari client HTTP yang tersambung lokal dengan mesin lain?

> _d. What is the client HTTP IP Address in connection with other local machine?_

**Answer:** `172.16.16.101`

- Filter expression

  `http.request`

- Explanation
  Gunakan Display Filter yang tercantum di atas untuk melakukan filtering pada packet yang ingin dicari. Dalam daftar yang terdisplay (yang ter-highlight biru) dapat terlihat bahwa source (client) yakni `172.16.16.101` sedang melakukan request ke `172.16.16.102` yakni mesin lain secara lokal.
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/96f2c370-3a82-4f86-a779-53c0065213d0" />


- Output result

  <img width="794" height="349" alt="image" src="https://github.com/user-attachments/assets/09f49df6-9691-44d2-883f-afef93a6b7e3" />


  <br>
  <br>

## Task 6

- Flag

  `JARKOM25{br0mb44rdin0u_Cr0ccc0c0c0cdi1l10l_4733999765awaesa1bytjds9w9sh1n0buLRUKWHBFD1QBJUR_73811cb638223ad10ea507f17c6749ea}`

> a. Apakah kamu menemukan fake flag? Tuliskan seluruhnya!

> _a. Did you find the fake flag? Write it whole!_

**Answer:** `FakeFlag{JarkomGampang}`

- Filter expression

  `-`

- Explanation

  Klik kanan pada baris manapun yang ter-display lalu masuk ke `Follow -> TCP Stream` dan jelajahi seluruhnya dengan mengubah angka `"Stream"` dikanan bawah sampai ketemu dapat dilihat bahwa Fake flag ditemukan di stream ke 31
  <img width="1920" height="1080" alt="Screenshot from 2025-09-13 12-26-56" src="https://github.com/user-attachments/assets/7e20fac9-7013-482b-8256-104c8a3553e8" />

  

- Output result

  <img width="794" height="203" alt="image" src="https://github.com/user-attachments/assets/e943b673-cf69-4ca1-a74a-00dd6eddc87a" />


  <br>
  <br>

> b. Tuliskan username dan password yang tertulis! (format username:password)

> _b. Write the written username and password! (format username:password)_

**Answer:** `Rey:123`

- Filter expression

  `-`

- Explanation

Klik kanan pada baris manapun yang ter-display lalu masuk ke `Follow -> TCP Stream` dan jelajahi seluruhnya dengan mengubah angka `"Stream"` dikanan bawah sampai ketemu dapat dilihat bahwa username dan password ditemukan di stream ke 32
  <img width="1920" height="1080" alt="Screenshot from 2025-09-13 12-27-14" src="https://github.com/user-attachments/assets/1be2c5ec-83ca-4427-b718-186e6d5c0d63" />


- Output result
<img width="805" height="272" alt="image" src="https://github.com/user-attachments/assets/852e96d0-6984-4823-a7bc-5180fe51b45d" />


  <br>
  <br>

## Task 7

- Flag

  `JARKOM25{tr4l4lel0_tr1lil1_p63b0k1wpek3b0s0sHWRD7OI3HGVP1C3_f11a7929ddf64e34001f7694057d6338}`

> Apa nama gambar yang direquest oleh client? (tulis dengan ekstensinya)

> _What is the image that is being requested by the client? (write with its extension)_

**Answer:** `donalbebek.jpg`

- Filter expression

  `-`

- Explanation

Klik kanan pada baris manapun yang ter-display lalu masuk ke `Follow -> TCP Stream `dan jelajahi seluruhnya dengan mengubah angka `"Stream"` dikanan bawah sampai ketemu dapat dilihat bahwa client melakukan request dengan `GET /donalbebek.jpg HTTP/1.1` yang ditemukan di stream ke 42
<img width="1920" height="1080" alt="Screenshot from 2025-09-13 12-43-33" src="https://github.com/user-attachments/assets/456c28dc-b3a9-49c0-8f6a-addb81e69924" />

- Output result

 <img width="805" height="230" alt="image" src="https://github.com/user-attachments/assets/563182a1-07e2-42ad-8ba1-a31fc843e52f" />


  <br>
  <br>

## Task 8

- Flag

  `JARKOM25{y0u_4r3_s0_G00d_1n_F0r3nsic_OACKCKCCXYZQ6CTQZVY7DH66Q776OVx45y4n6xp344yrom2vkmww9a9kcaa0_46b6e327b6eddd84b7b71b24e8d93c7f}`

> a. Berapa banyak packet berbasis FTP yang terekam pada file pcapng? (with the data)

> _a. How many FTP packets are recorded in the pcapng file? (with the data)_

**Answer:** `81`

- Filter expression

  `-`

- Explanation
Gunakan bar atas untuk masuk ke `Statistics -> Protocol Hierarchy` dan lihat jumlah packet dari `FTP Data` yakni 10 dan `File Transfer Protocol (FTP)` yakni 71 jika dijumlah maka 81
<img width="1920" height="1080" alt="Screenshot from 2025-09-13 12-53-57" src="https://github.com/user-attachments/assets/021beb38-0f0b-430a-bb8d-4d22e22ec138" />


- Output result

 <img width="805" height="230" alt="image" src="https://github.com/user-attachments/assets/c0f8eb45-c705-4285-9ee9-8600384a6217" />


  <br>
  <br>

> b. Apa username dan password client di koneksi FTP? (tulis dalam format username:password)

> _b. What is the client's username and password in FTP connection? (write in following format username:password)_

**Answer:** `rey:password123lingangu`

- Filter expression

  `ftp`

- Explanation

Gunakan Display Filter yang tercantum di atas untuk melakukan filtering pada packet yang ingin dicari. Lalu klik kanan manapun dari daftar yang terdisplay masuk ke `
