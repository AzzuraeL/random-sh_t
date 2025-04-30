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

* Untuk memenuhi apa yang diinginkan oleh Pipip maka saya membuat kode loadbalancer sebagai berikut:

  `loadbalancer.c`
  
```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/wait.h>

typedef struct {
    char msgs[256];
    int jml;
    int giliran;
    int state;
} shrm;

typedef struct {
    long mtype;
    char mtext[256];
} message;

void system_log(const char* dat) {
    int fd = open("sistem.log", O_WRONLY | O_APPEND | O_CREAT, 0666);
    flock(fd, LOCK_EX);
    write(fd, dat, strlen(dat));
    flock(fd, LOCK_UN);
    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc != 2) return 1;
    int max = atoi(argv[1]);

    key_t shrmkey = ftok("progfile", 1234);
    key_t mquekey = ftok("progfile", 5678);

    int shmid = shmget(shrmkey, sizeof(shrm), 0666);
    shrm* data = (shrm*)shmat(shmid, NULL, 0);
    while (data->jml <= 0) sleep(1);

    int msgid = msgget(mquekey, IPC_CREAT | 0666);

    for (int i = 0; i < data->jml; i++) {
        char log[300];
        snprintf(log, sizeof(log), "Received at lb: %s (#message %d)\n", data->msgs, i + 1);
        system_log(log);

        message msg;
        msg.mtype = (i % max) + 1;
        strcpy(msg.mtext, data->msgs);
        msgsnd(msgid, &msg, sizeof(msg.mtext), 0);
    }

    data->giliran = 1;
    data->state = 0;
    for (int i = 0; i < max; i++) {
        pid_t pid1 = fork();
        if (pid1 == 0) {
            char arg1[30];
            snprintf(arg1, sizeof(arg1), "%d", i + 1);
            execl("./worker", "./worker", arg1, argv[1], NULL);
            exit(1);
        }
    }

    for (int i = 0; i < max; i++) wait(NULL);

    data->state =2;
    data->giliran = 1;
    for (int i = 0; i < data->jml; i++) {
        message msg;
        msg.mtype = (i % max) + 1;
        strcpy(msg.mtext, data->msgs);
        msgsnd(msgid, &msg, sizeof(msg.mtext), 0);
    }
    for (int i = 0; i < max; i++) {
        pid_t pid2 = fork();
        if (pid2 == 0) {
            char arg2[30];
            snprintf(arg2, sizeof(arg2), "%d", i + 1);
            execl("./worker", "./worker", arg2, argv[1], NULL);
            exit(1);
        }
    }

    for (int i = 0; i < max; i++) wait(NULL);    


    shmdt(data);
    shmctl(shmid, IPC_RMID, NULL);
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}
```

* Penjelasan
  1. Membuat struct `shrm` sebagai tempat pertukaran data dalam shared memory
  2. Membuat struct `message` sebagai sarana pengiriman pesan secara message queue
  3. Membuat fungsi `system_log` sebagai sebuah alat untuk menuliskan pesan yang diterima dan sudah diformat kedalam `sistem.log`, ada penggunaan `flock()` demi menjaga kondisi dimana file akann di write oleh satu proses saja dan tidak ada proses yang saling tumpang tindih.
  4. Masuk ke `main()` yang pertama dilakukan adalah mengambil argumen di `argv[1]` sebagai max worker nantinya (jika dieksekusi dengan `./load balancer 3` maka angka 3 lah yang diambil sebagai max worker
  5. Membuat key untuk shared memmory dan message queue serta menginisialisasi untuk attach ke shared memory yang dibuat `client.c` lalu proses akan terus sleep sampai akhirnya jumlah pesan bertambah atau sudah menerima pesan,
  6. Ketika sudah ada pesan yang diterima barulah masuk eksekusi untuk memulai pengiriman ke setiap worker diawali dengan membuat id message queue menggunakan `msgget` dengan key yang sudah kita buat diawal.
  7. Masuk ke dalam looping sejumlah banyak pesan. Yang pertama dilakukan dalam loop adalah mengirimkan pesan ke dalam `sistem.log` bahwa pesan telah diterima sebanyak jumlah pesan keseluruhan, lalu mengirimkan setiap pesan ke setiap worker dengan membedakan message type nya (round robin) dengan melakukan modulo terhadap urutan pesan menggunakan max worker sehingga jika workernya maximal 3 maka message typenya akan berubah ubah (123123123dst). Pesan dikirim dengan `msgsend`.
  8. Mengubah giliran menjadi 1 untuk mulai mengirimkan pesan mulai worker 1 dan mengubah state jadi 0 untuk kondisi nanti, lalu melakukan `fork()` `exec()` dan `wait()` untuk mengeksekusi `worker.c` secara pararel namun tetap berurutan sesuai gilirannya.
  9. Didalam `fork()` akan dilakukan eksekusi terhadap child yakni `worker.c` dengan format tertentu yakni ketika `loadbalancer.c` dieksekusi dengan `./loadbalancer 3` maka child yakni `worker.c` akan dieksekusi dengan format `./worker 1 3` , `./worker 2 3` , dan `./worker 3 3` jadi yang dikirimkan ke worker adalah id workernya dan max workernya.
  10. Jika sudah akan kembali ke `loadbalancer.c` untuk tahap selanjutnya.
 
* Problem
  1. Susahnya membuat proses `worker.c` yang pararel di `fork()` tetap dapat bergantian secara round robin ketika menerima dari `loadbalancer.c` dan menulis ke `sistem.log`
  2. Sering terjadi core dumped karena kesalahan dalam perubahan giliran yang menyebabkan race condition.

### **c. Worker Mencatat Pesan yang Diterima**

* Deskripsi 

Setiap worker yang menerima pesan dari `loadbalancer.c` harus mencatat pesan yang diterima ke dalam **`sistem.log`** dengan format log sebagai berikut:

```
WorkerX: message received
```


* Untuk memenuhi apa yang diinginkan oleh Pipip maka saya membuat kode loadbalancer sebagai berikut:

  `worker.c`
```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>

typedef struct {
    long mtype;
    char mtext[256];
} message;

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

int main(int argc, char* argv[]) {
    if (argc != 3) exit(1);
    int id = atoi(argv[1]);
    int max = atoi(argv[2]);

    key_t mquekey = ftok("progfile", 5678);
    int msgid = msgget(mquekey, 0666);
    key_t shrmkey = ftok("progfile", 1234);
    int shmid = shmget(shrmkey, sizeof(shrm), 0666);
    shrm* data = (shrm*)shmat(shmid, NULL, 0);

    message msg;

    if (data->state == 0) {
        while (msgrcv(msgid, &msg, sizeof(msg.mtext), id, IPC_NOWAIT) >= 0) {
            while (data->giliran != id) usleep(1000);
            char log[300];
            snprintf(log, sizeof(log), "Worker%d: message received\n", id);
            system_log(log);
            usleep(100000);
            data->giliran = (data->giliran % max) + 1;
        }
        
    }if (data->state == 2) {
        int counter = 0;
        while (msgrcv(msgid, &msg, sizeof(msg.mtext), id, IPC_NOWAIT) >= 0) {
            while (data->giliran != id) usleep(1000);
            counter++;
        }
        char sum[300];
        snprintf(sum, sizeof(sum), "Worker %d: %d messages\n", id, counter);
        system_log(sum);
        data->giliran = (data->giliran % max) + 1;
        shmdt(data);
        return 0;
    }

    
}
```

* Penjelasan
  1. Membuat struct `shrm` sebagai tempat pertukaran data dalam shared memory
  2. Membuat struct `message` sebagai sarana pengiriman pesan secara message queue
  3. Membuat fungsi `system_log` sebagai sebuah alat untuk menuliskan pesan yang diterima dan sudah diformat kedalam `sistem.log`, ada penggunaan `flock()` demi menjaga kondisi dimana file akann di write oleh satu proses saja dan tidak ada proses yang saling tumpang tindih.
  4. Masuk ke `main()` yang dilakukan pertama kali adalah menerima `argv[1]` sebagai id dan menerima `argv[2]` sebagai max worker.
  5. Melakukan inisialisasi ke message queue untuk menerima pesan dan inisialisasi ke shared memory untuk berbagi giliran dan state.
  6. Kemudian karena dalam penerimaan pesan dari `loadbalancer.c` tadi statenya adalah 0 maka akan masuk ke kondisi yang pertama
  7. Di dalam kondisi ini akan terjadi looping selama masih menerima pesan , didalam looping besar akan ada looping kecil untuk membuat proses sleep sementara jika idnya belum sama dengan gilirannya, karena tadi giliran di `loadbalancer.c` diinisialisasi 1 maka akan dimulai dari id 1 sampai max worker dan kembali ke 1 lagi, didalam loop yang besar ada proses penulisan format bahwa worker telah menerima pesan ke dalam `sistem.log` lalu melakukan penambahan giliran secara roung robin juga.
  8. Lalu kembali ke `loadbalancer.c` untuk proses selanjutnya.

* Problem
  1. Susah dalam membuat giliran agar tidak saling tumpang tindih

### **d. Catat Total Pesan yang Diterima Setiap Worker di Akhir Eksekusi**

* Deskripsi

Setelah proses selesai (semua pesan sudah diproses), setiap worker akan mencatat jumlah total pesan yang mereka terima ke bagian akhir file **`sistem.log`**.

```
Worker 1: 3 messages
Worker 2: 4 messages
Worker 3: 3 messages
```

* Kode untuk bagian ini adalah lanjutan dari `loadbalancer.c` dan `worker.c`
* Penjelasam
  1. Proses dimulai dari `loadbalancer.c` tepat setelah berhasil mengeksekusi child dalam `fork()` pertama, maka `loadbalancer.c` akan melakukan reset terhadap giliran menjadi 1 dan mengubah state menjadi 2, kemudia melakukan looping kedua untuk mengirimkan pesan tanpa format tetapi hanya untuk melakukan perhitungan jumlah.
  2. Selanjutnya melakukan forking kedua terhadap `worker.c` untuk melakukan perhitungan jumlah.
  3. Ketika `worker.c` dieksekusi dalam forking kedua maka ia akan masuk kedalam kondisi dimana state 2
  4. Dalam state 2 `worker.c` akan melakukan perhitungan terhadap jumlah pesan yang diterima oleh masing masing worker secara round robin dan juga menuliskan hasilnya kedalam `sistem.log`
  5. Setelah selesai dia akan kembali ke `loadbalancer.c` lagi untuk melakukan penghapusan semua shared memory dan message queue yang telah digunakan menggunakan `msgctl` dan `shmctl`

### **OUTPUT**

MISAL INPUT : Halo A;10 

Akan dikirim ke 3 worker.

* Eksekusi Terminal 1
  ```
  ./client
  ```
* Eksekusi Terminal 2
  ```
  ./loadbalancer 3
  ```
* Hasil didalam `sistem.log`
```
Message from client: Halo A
Message count: 10
Received at lb: Halo A (#message 1)
Received at lb: Halo A (#message 2)
Received at lb: Halo A (#message 3)
Received at lb: Halo A (#message 4)
Received at lb: Halo A (#message 5)
Received at lb: Halo A (#message 6)
Received at lb: Halo A (#message 7)
Received at lb: Halo A (#message 8)
Received at lb: Halo A (#message 9)
Received at lb: Halo A (#message 10)
Worker1: message received
Worker2: message received
Worker3: message received
Worker1: message received
Worker2: message received
Worker3: message received
Worker1: message received
Worker2: message received
Worker3: message received
Worker1: message received
Worker 1: 4 messages
Worker 2: 3 messages
Worker 3: 3 messages
```
THANK YOU
  
