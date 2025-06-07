# Riemann Integral Approximation and Error Calculation
## 📌 Function Overview

### `f(x)`

```python
def f(x):
    return 3 * x**5 - 8 * x**4
```

This is the function we want to integrate:

$$
f(x) = 3x^5 - 8x^4
$$

---

### `riemann_integral(a, b, n)`

```python
def riemann_integral(a, b, n):
    ...
```

Calculates the **Riemann sum** over the interval $[a, b]$ with `n` subintervals.

* `delta_x = (b - a) / n` → width of each subinterval
* Uses the left point of each subinterval to evaluate `f(x)`
* Returns the **approximated area under the curve**, rounded to 2 decimal places.

---

### `nilai sebenarnya(a, b)`

```python
def nilai_sebenarnya(a, b):
    ...
```

Returns the **exact value** of the definite integral of `f(x)` from `a` to `b`.

The antiderivative $F(x)$ of $f(x)$ is calculated as:

$$
F(x) = \frac{1}{2}x^6 - \frac{8}{5}x^5
$$

Then the definite integral is computed as:

$$
\int_a^b f(x)dx = F(b) - F(a)
$$

---

### `(a, b, n)`

```python
def riemann_error(a, b, n):
    ...
```

* Computes the approximate integral using `riemann_integral`
* Computes the exact integral using `nilai_sebenarnya`
* Calculates the **percentage error**:

$$
\text{error} = \left|\frac{\text{exact} - \text{approx}}{\text{exact}}\right| \times 100\%
$$

* Returns a tuple: `(approximate value, exact value, error %)`

---

## 🧪 Example Parameters

```python
a = 4
b = 16
n = 4
```

* `a`: Lower bound of integration
* `b`: Upper bound of integration
* `n`: Number of Riemann subintervals

---

## ✅ Output Example

```
Luas dengan Riemann (n=4): 3412884.00
Luas sebenarnya: 6710476.80
Error true: 49.14
```

* The Riemann sum gives an approximation of the area under the curve.
* The exact integral is computed analytically.
* The error percentage shows how far off the approximation is.

---

## 📚 Notes

* The accuracy improves with larger `n` (more subintervals).
