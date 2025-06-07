# FINAL PROJECT KOMPUTASI NUMERIK (A) 2025 / Kelompok A13
Program Python Final Project Komputasi Numerik

Anggota Kelompok A13 :
|    NRP     |      Name      |
| :--------: | :------------: |
| 5025241017 | Mitra Partogi   |

<h4>Penulis README = Mitra Partogi</h4>
Tujuan : Pemenuhan Tugas Final Project Komputasi Numerik. <br>
Soal : 
<div align="left">
  <img src="https://github.com/user-attachments/assets/acbaa9b3-92c0-42bc-b9fd-0bec5c7b1d2d" width="600" />
</div>

📐 Perkiraan Integral Riemann dan Perhitungan Error
Program Python ini menghitung luas di bawah kurva fungsi tertentu menggunakan metode Riemann Sum Titik Kiri, lalu membandingkannya dengan hasil integral eksak (nilai sebenarnya). Program juga menghitung persentase error antara hasil pendekatan dan nilai sebenarnya.

🔧 Penjelasan Fungsi
f(x)
python
Copy
Edit
def f(x):
    return 3 * x**5 - 8 * x**4
Fungsi yang ingin dihitung luasnya:

𝑓
(
𝑥
)
=
3
𝑥
5
−
8
𝑥
4
f(x)=3x 
5
 −8x 
4
 
riemann_integral(a, b, n)
python
Copy
Edit
def riemann_integral(a, b, n):
    ...
Menghitung pendekatan integral (luas) menggunakan Riemann Sum Titik Kiri pada interval 
[
𝑎
,
𝑏
]
[a,b] dengan n bagian.

delta_x = (b - a) / n → lebar tiap bagian

Menggunakan titik kiri tiap bagian untuk menghitung f(x)

Mengembalikan hasil luas pendekatan (dibulatkan 2 angka di belakang koma)

nilai_sebenarnya(a, b)
python
Copy
Edit
def nilai_sebenarnya(a, b):
    ...
Menghitung nilai eksak (sebenarnya) dari integral fungsi 
𝑓
(
𝑥
)
f(x) dari a ke b.

Turunan balik (antiturunan) dari 
𝑓
(
𝑥
)
f(x) adalah:

𝐹
(
𝑥
)
=
1
2
𝑥
6
−
8
5
𝑥
5
F(x)= 
2
1
​
 x 
6
 − 
5
8
​
 x 
5
 
Integral dihitung dengan:

∫
𝑎
𝑏
𝑓
(
𝑥
)
𝑑
𝑥
=
𝐹
(
𝑏
)
−
𝐹
(
𝑎
)
∫ 
a
b
​
 f(x)dx=F(b)−F(a)
riemann_error(a, b, n)
python
Copy
Edit
def riemann_error(a, b, n):
    ...
Menghitung pendekatan luas dengan riemann_integral

Menghitung nilai sebenarnya dengan nilai_sebenarnya

Menghitung persentase error:

error
=
∣
eksak
−
aproksimasi
eksak
∣
×
100
%
error= 
​
  
eksak
eksak−aproksimasi
​
  
​
 ×100%
Mengembalikan (nilai pendekatan, nilai sebenarnya, error%)

🔢 Contoh Penggunaan
python
Copy
Edit
a = 4
b = 16
n = 4
a: batas bawah

b: batas atas

n: jumlah bagian (semakin besar, semakin akurat)

🧾 Contoh Output
java
Copy
Edit
Luas dengan Riemann (n=4): 3412884.00
Luas sebenarnya: 6710476.80
Error true: 49.14
Luas Riemann: hasil pendekatan

Luas sebenarnya: hasil perhitungan eksak

Error: selisih dalam persen antara dua hasil

ℹ️ Catatan
Semakin besar n, semakin kecil error-nya (lebih akurat)

Metode ini cocok untuk memahami konsep integral numerik dasar
