# Task 4
## Pipip's Load Balancer
Pipip, seorang pengembang perangkat lunak yang tengah mengerjakan proyek distribusi pesan dengan sistem load balancing, memutuskan untuk merancang sebuah sistem yang memungkinkan pesan dari client bisa disalurkan secara efisien ke beberapa worker. Dengan menggunakan komunikasi antar-proses (IPC), Pipip ingin memastikan bahwa proses pengiriman pesan berjalan mulus dan terorganisir dengan baik, melalui sistem log yang tercatat dengan rapi.

### **a. Client Mengirimkan Pesan ke Load Balancer**

* Deskripsi

Pipip ingin agar proses `client.c` dapat mengirimkan pesan ke `loadbalancer.c` menggunakan IPC dengan metode **shared memory**. Proses pengiriman pesan dilakukan dengan format input dari pengguna sebagai berikut:

```
Halo A;10
```

**Penjelasan:**

- `"Halo A"` adalah isi pesan yang akan dikirim.
- `10` adalah jumlah pesan yang ingin dikirim, dalam hal ini sebanyak 10 kali pesan yang sama.

Selain itu, setiap kali pesan dikirim, proses `client.c` harus menuliskan aktivitasnya ke dalam **`sistem.log`** dengan format:

```
Message from client: <isi pesan>
Message count: <jumlah pesan>
```

Semua pesan yang dikirimkan dari client akan diteruskan ke `loadbalancer.c` untuk diproses lebih lanjut.

* Untuk memenuhi apa yang diinginkan oleh Pipip maka saya membuat kode client sebagai berikut:

`client.c`
```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>

typedef struct {
    char msgs[256];
    int jml;
    int giliran;
    int state;
} shrm;

void system_log(const char* dat) {
    int fd = open("sistem.log", O_WRONLY | O_APPEND | O_CREAT, 0666);
    flock(fd, LOCK_EX);
    write(fd, dat, strlen(dat));
    flock(fd, LOCK_UN);
    close(fd);
}

int main() {
    char input[300];
    fgets(input, sizeof(input), stdin);

    key_t shrmkey = ftok("progfile", 1234);

    char *token = strtok(input, ";");
    char msgs[256];
    int jml;

    if (token) strcpy(msgs, token);
    token = strtok(NULL, ";");
    if (token) jml = atoi(token);

    int shmid = shmget(shrmkey, sizeof(shrm), IPC_CREAT | 0666);
    shrm* data = (shrm*)shmat(shmid, NULL, 0);

    strcpy(data->msgs, msgs);
    data->jml = jml;
    data->giliran = 0;
    data->state = 0;

    char log[300];
    snprintf(log, sizeof(log), "Message from client: %s\nMessage count: %d\n", msgs, jml);
    system_log(log);

    shmdt(data);
    return 0;
}
```
* Penjelasan
  1. Membuat struct `shrm` sebagai tempat pertukaran data dalam shared memory
  2. Membuat fungsi `system_log` sebagai sebuah alat untuk menuliskan pesan yang diterima dan sudah diformat kedalam `sistem.log`, ada penggunaan `flock()` demi menjaga kondisi dimana file akann di write oleh satu proses saja dan tidak ada proses yang saling tumpang tindih.
  3. Masuk ke `main()` yang pertama dibuat adalah kode untuk meminta innput kepada user lalu membuat key untuk shared memory, lalu selanjutnya adalah memisahkan input yang diterima menjadi 2 bagian dengan strtok, bagian pertama adalah dari awal input sampai delimiter (;) pertama akan disimpan sebagai pesan yang akan diproses, sedangkan bagian kedua adalah dari setelah delimiter (;) pertama sampai akhir input yang akan disimpan sebagai jumlah pesan.
  4. Selanjutnya yaitu membuat id shared memory dalam variable `shmid` dengan `shmget` menggunakan key shared memory yang sudah kita buat di awal.
  5. Membuat variable data yang akan digunakan untuk menyimpan setiap data yang akan dikirim lewat shared memory dengan `shmat` (shared memory attach) menggunakan `shmid` yang dibuat sebelumnya.
  6. Memasukkan setiap data yang diperlukan (Pesan, Jumlah Pesan, giliran diinisialisasi 0, dan state diinisialisasi 0)
  7. Melakukan formating terhadap pesan yang dikirim menggunakan `snprintf` untuk menghindaro buffer overflow ketika pesan terlalu panjang
  8. Memanggil fungsi `system_log` dengan passing pesan yang sudah diformat agar bisa dimasukkan ke file `sistem.log`
  9. `shmdt` untuk melakukan detach dari shared memory.
      
* Problem
  1. Sebelum penggunaan `flock` ada pesan di `sistem.log` yang aneh karena bertumpuk sebelum pesan sebelumnya selesai ditulis

### **b. Load Balancer Mendistribusikan Pesan ke Worker Secara Round-Robin**

* Deskripsi
  
Setelah menerima pesan dari client, tugas `loadbalancer.c` adalah mendistribusikan pesan-pesan tersebut ke beberapa **worker** menggunakan metode **round-robin**. Sebelum mendistribusikan pesan, `loadbalancer.c` terlebih dahulu mencatat informasi ke dalam **`sistem.log`** dengan format:

```
Received at lb: <isi pesan> (#message <indeks pesan>)
```

Contoh jika ada 10 pesan yang dikirimkan, maka output log yang dihasilkan adalah:

```
Received at lb: Halo A (#message 1)
Received at lb: Halo A (#message 2)
...
Received at lb: Halo A (#message 10)
```

Setelah itu, `loadbalancer.c` akan meneruskan pesan-pesan tersebut ke **n worker** secara bergiliran (round-robin), menggunakan **IPC message queue**. Berikut adalah contoh distribusi jika jumlah worker adalah 3:

- Pesan 1 → worker1
- Pesan 2 → worker2
- Pesan 3 → worker3
- Pesan 4 → worker1 (diulang dari awal)

Dan seterusnya.

Proses `worker.c` bertugas untuk mengeksekusi pesan yang diterima dan mencatat log ke dalam file yang sama, yakni **`sistem.log`**.



