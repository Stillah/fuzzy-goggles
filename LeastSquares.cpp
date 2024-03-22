#include <bits/stdc++.h>
using namespace std;

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
    Matrix(Matrix& matrix) = default;
    ~Matrix() = default;

    bool sameSize(Matrix<T>& diffMatrix) { return diffMatrix.m == m && diffMatrix.n == n; }
    vector<T>& operator[](size_t i) { return matrix[i]; }
    const vector<T>& operator[](size_t i) const{ return matrix[i]; }
    Matrix<T>& operator=(const Matrix<T>& diffMatrix) = default;
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
        for (int i = 0; i < m; i++)
            for (int j = 0; j < diffMatrix.n; j++)
                for (int inner = 0; inner < n; inner++)
                    new_matrix[i][j] += matrix[i][inner] * diffMatrix[inner][j];
        return new_matrix;
    }
    Matrix<T> transpose() const{
        Matrix<T> new_matrix(n, m);
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                new_matrix[j][i] = matrix[i][j];
        return new_matrix;
    }
    friend ostream& operator<<(ostream&, const Matrix<T>& A) {
        for (int i = 0; i < A.m; i++) {
            for (int j = 0; j < A.n; j++) {
                cout << setprecision(4) << fixed << A[i][j] << " ";
            }
            cout << "\n";
        }
        return cout;
    }
    double det() const{
        auto new_matrix = matrix;
        if (n != m) throw invalid_argument("Error: not a square matrix\n");
        int permutations = 0;
        for (int j = 0; j < n; j++) {
            for (int i = j; i < m; i++) {
                if (i == j) { //permutations
                    pair<double, int> maximum{abs(new_matrix[i][j]), i};
                    for (int k = i; k < n; k++) {
                        if (maximum.first < abs(new_matrix[k][j])) {
                            maximum.first = abs(new_matrix[k][j]);
                            maximum.second = k;
                        }
                    }
                    if (maximum != make_pair(abs(new_matrix[i][j]), i)) {
                        permutations++;
                        swap(new_matrix[i], new_matrix[maximum.second]);
                        continue;
                    }
                }

                if (new_matrix[i][j] == 0 || (i == j)) continue;
                int c = 1;
                while (new_matrix[i - c][j] == 0) c++;
                double x = new_matrix[i][j] / new_matrix[i - c][j];

                for (int k = 0; k < n; k++)
                    new_matrix[i][k] -= new_matrix[i - c][k] * x;

            }
        }
        double det = permutations % 2 == 0? 1: -1;
        for (int i = 0; i < n; i++) det *= new_matrix[i][i];
        return det;
    }
    void printSystem(Matrix<T>& augmented) const{ cout << *(Matrix<double> *) this << augmented;}
    void subtractRow(size_t from, size_t what, double mult) {
        for (int i = 0; i < n; i++) {
            matrix[from][i] -= matrix[what][i] * mult;
        }
    }
    T diagonalProduct(bool positive) {
        T prod = positive? 1: -1;
        for (int i = 0; i < n; i++) prod *= matrix[i][i];
        return prod;
    }
    bool makeREF(Matrix<T>& augmented, int& steps) {
        int permutations = 0;
        for (int j = 0; j < n; j++) {
            for (int i = j; i < m; i++) {
                if (i == j) { //permutations
                    pair<double, int> maximum{abs(matrix[i][j]), i};
                    for (int k = i; k < n; k++) {
                        if (maximum.first < abs(matrix[k][j])) {
                            maximum.first = abs(matrix[k][j]);
                            maximum.second = k;
                        }
                    }
                    if (maximum != make_pair(abs(matrix[i][j]), i)) {
                        permutations++;
                        swap(matrix[i], matrix[maximum.second]);
                        swap(augmented[i], augmented[maximum.second]);
//                        cout << fixed << setprecision(4) << "step #" << ++steps << ": permutation\n";
//                        printSystem(augmented);
                        continue;
                    }
                }
                if (matrix[i][j] == 0 || (i == j)) continue;
                int c = 1;
                while (matrix[i - c][j] == 0) c++;
                double x = matrix[i][j] / matrix[i - c][j];

                subtractRow(i, i-c, x);
                augmented.subtractRow(i, i-c, x);
//                cout << fixed << setprecision(4) <<  "step #" << ++steps << ": elimination\n";
//                printSystem(augmented);
            }
        }
        return permutations % 2 == 0;
    }
    void makeDiagonal(Matrix<T>& augmented) { //changes matrix to the identity
        if (n != m || det() == 0) throw invalid_argument("Error: matrix A is singular\n");

        int steps = 0;
        //cout << "Gaussian process:\n";
        makeREF(augmented, steps);

        for (int j = n - 1; j > 0; j--) {
            for (int i = j - 1; i >= 0; i--) {
                if (matrix[i][j] == 0) continue;
                augmented.subtractRow(i, j, matrix[i][j] / matrix[j][j]);
                matrix[i][j] = 0;
                //cout << fixed << setprecision(4) <<  "step #" << ++steps << ": elimination\n";
                //printSystem(augmented);
            }
        }
        //cout << "Diagonal normalization:\n";
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < augmented.n; j++) {
                augmented[i][j] /= matrix[i][i];
            }
            matrix[i][i] = 1;
        }
        //printSystem(augmented);
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

template<typename T> //for now only square matrices are allowed
class EliminationMatrix: public IdentityMatrix<T> {
public:
    EliminationMatrix(Matrix<T> A, size_t row, size_t column): IdentityMatrix<T>(A.getN()) {
        row--, column--;
        for (int i = 0; i < A.getM(); i++) {
            if (i == row) continue;
            if (A[i][column] != 0) {
                this->matrix[row][column] = -A[row][column] / A[i][column];
                break;
            }
        }
    }
};

template<typename T>
class PermutationMatrix: public IdentityMatrix<T> {
public:
    PermutationMatrix(size_t i, size_t j, size_t n): IdentityMatrix<T>(n) {
        i--, j--;
        this->matrix[i][i] = 0;
        this->matrix[j][j] = 0;
        this->matrix[i][j] = 1;
        this->matrix[j][i] = 1;
    }
};

template<typename T>
class ColumnVector: public Matrix<T> {
public:
    explicit ColumnVector(size_t m): Matrix<T>(m, 1) {};
    double norm() {
        double norm = 0;
        for (int i = 0; i < this->m; i++)
            norm += this->matrix[i][0] * this->matrix[i][0];
        return sqrt(norm);
    }
};

int main() {
    int m; cin >> m;
    vector<double> buffer(m);
    Matrix<double> b(m, 1);

    for (int i = 0; i < m; i++)
        cin >> buffer[i] >> b[i][0];

    int n; cin >> n;
    Matrix<double> A(m, n + 1);
    IdentityMatrix<double> I(n+1);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n + 1; j++) {
            A[i][j] = pow(buffer[i], j);
        }
    }

    auto A_T = A.transpose();
    cout << "A:\n" << A;
    b = A_T * b;
    A = A_T * A;
    cout << "A_T*A:\n" << A;
    auto A_copy = A;
    A_copy.makeDiagonal(I);
    cout << "(A_T*A)_-1:\n" << I;
    cout << "A_T*b:\n" << b;
    A.makeDiagonal(b);
    cout << "x~:\n" << b;
    return 0;
}
