#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll mod = 1e9 + 7;

template<typename T>
class IdentityMatrix;

template<typename T>
class SquareMatrix;

template<typename T>
class Matrix {
  protected:
  size_t n, m;
  vector<vector<T>> matrix;
  public:
  Matrix(size_t m, size_t n): n(n), m(m) { matrix.resize(m, vector<T>(n)); };
  Matrix(const IdentityMatrix<T>& diffMatrix): matrix(diffMatrix.matrix), n(diffMatrix.n), m(diffMatrix.m) { }
  Matrix(Matrix& matrix) = default;
  ~Matrix() = default;

  bool sameSize(Matrix<T>& diffMatrix) { return diffMatrix.m == m && diffMatrix.n == n; }
  vector<T>& operator[](size_t i) { return matrix[i]; }
  const vector<T>& operator[](size_t i) const{ return matrix[i]; }
  //Matrix<T>& operator=(const Matrix<T>& diffMatrix) = default;
  Matrix<T> operator+(Matrix<T>& diffMatrix) {
    if (!sameSize(diffMatrix)) { throw invalid_argument("Error: the dimensional problem occurred\n"); }
    Matrix<T> new_matrix(m, n);
    for (int i = 0; i < m; i++)
      for (int j = 0; j < n; j++)
        new_matrix[i][j] = matrix[i][j] + diffMatrix[i][j];
    return new_matrix;
  }
  Matrix<T> operator-(Matrix<T>& diffMatrix) {
    if (!sameSize(diffMatrix)) throw invalid_argument("Error: the dimensional problem occurred\n");
    Matrix<T> new_matrix(m, n);
    for (int i = 0; i < m; i++)
      for (int j = 0; j < n; j++)
        new_matrix[i][j] = matrix[i][j] - diffMatrix[i][j];
    return new_matrix;
  }
  Matrix<T> operator*(Matrix<T>& diffMatrix) {
    if (diffMatrix.m != n) throw invalid_argument("Error: the dimensional problem occurred\n");
    Matrix<T> new_matrix(m, diffMatrix.n);
    for (int i = 0; i < m; i++) {
      for (int j = 0; j < diffMatrix.n; j++) {
        for (int inner = 0; inner < n; inner++) {
          new_matrix[i][j] += ((matrix[i][inner] % mod) * (diffMatrix[inner][j] % mod)) % mod;
          new_matrix[i][j] %= mod;
        }
      }
    }
    return new_matrix;
  }

  Matrix<T>& operator=(const Matrix<T>& diffMatrix) {
    m = diffMatrix.m;
    n = diffMatrix.n;
    for (int i = 0; i < m; i++)
      for (int j = 0; j < n; j++)
        matrix[i][j] = diffMatrix[i][j];
    return *this;
  }

  size_t getN() const { return n; }
  size_t getM() const { return m; }
};

template<typename T>
class SquareMatrix: public Matrix<T> {
  public:
  explicit SquareMatrix(size_t n): Matrix<T>(n, n) {};
};

template<typename T> //for now only square matrices are allowed
class IdentityMatrix: public SquareMatrix<T> {
  public:
  explicit IdentityMatrix(size_t n): SquareMatrix<T>(n) {
    for (int i = 0; i < n; i++) this->matrix[i][i] = T(1);
  }
};

Matrix<ll> power(Matrix<ll> A, ll n) {
  if (n == 0) return IdentityMatrix<ll>(A.getM());
  auto u = power(A, n/2);
  u = u * u;
  if (n & 1) u = u * A;
  return u;
}
