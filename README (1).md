Program Python ini menghitung luas di bawah kurva fungsi tertentu menggunakan metode **Riemann**, lalu membandingkannya dengan hasil **integral eksak** (nilai sebenarnya). Program juga menghitung **persentase error** antara hasil pendekatan dan nilai sebenarnya.

---

##  Penjelasan Fungsi

### `f(x)`

```python
def f(x):
    return 3 * x**5 - 8 * x**4
```

Fungsi yang ingin dihitung luasnya:

$$
f(x) = 3x^5 - 8x^4
$$

---

### `riemann_integral(a, b, n)`

```python
def riemann_integral(a, b, n):
    delta_x = (b - a) / n
    total = 0
    for i in range(n):
        x_riemann = a + i * delta_x 
        total += f(x_riemann)
    luas = total * delta_x
    return round(luas, 2)
```

Menghitung pendekatan integral (luas) menggunakan **Riemann** pada interval $[a, b]$ dengan `n` bagian.

* `delta_x = (b - a) / n` → lebar tiap bagian
* Menggunakan titik kiri tiap bagian untuk menghitung `f(x)`
* Mengembalikan hasil luas pendekatan (dibulatkan 2 angka di belakang koma)

---

### `nilai_sebenarnya(a, b)`

```python
def nilai_sebenarnya(a, b):
    def F(x):
        return 0.5 * x**6 - (8/5) * x**5
    hasil = F(b) - F(a)
    return round(hasil, 2)
```

Menghitung **nilai eksak (sebenarnya)** dari integral fungsi $f(x)$ dari `a` ke `b`.

Turunan balik (antiturunan) dari $f(x)$ adalah:

$$
F(x) = \frac{1}{2}x^6 - \frac{8}{5}x^5
$$

Integral dihitung dengan:

$$
\int_a^b f(x)dx = F(b) - F(a)
$$

---

### `riemann_error(a, b, n)`

```python
def riemann_error(a, b, n):
    approx = riemann_integral(a, b, n)
    val_sebenarnya = nilai_sebenarnya(a, b)
    error = abs((val_sebenarnya - approx)/val_sebenarnya) * 100
    return round(approx, 2), round(val_sebenarnya, 2), round(error, 2)
```

* Menghitung pendekatan luas dengan `riemann_integral`
* Menghitung nilai sebenarnya dengan `nilai_sebenarnya`
* Menghitung **persentase error**:

$$
\text{error} = \left|\frac{\text{eksak} - \text{aproksimasi}}{\text{eksak}}\right| \times 100\%
$$

* Mengembalikan `(nilai pendekatan, nilai sebenarnya, error%)`

---

##  Contoh Penggunaan

```python
a = 4
b = 16
n = 4
```

* `a`: batas bawah
* `b`: batas atas
* `n`: jumlah bagian (semakin besar, semakin akurat)

---

##  Contoh Output

```bash
Luas dengan Riemann (n=4): 3412884.00
Luas sebenarnya: 6710476.80
Error true: 49.14
```

* Luas Riemann: hasil pendekatan
* Luas sebenarnya: hasil perhitungan eksak
* Error: selisih dalam persen antara dua hasil

---

##  Catatan

* Semakin besar `n`, semakin kecil error-nya (lebih akurat)
* Metode ini cocok untuk memahami konsep integral numerik dasar

---

