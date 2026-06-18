module;

#include <cstddef>
#include <string>
#include <vector>

export module math.matrix;

import core.exceptions;
import math.rational;

export namespace math {

class Matrix {
public:
    Matrix() = default;

    Matrix(std::size_t rows, std::size_t cols)
        : rows_(rows),
          cols_(cols),
          data_(rows * cols) {}

    Matrix(std::size_t rows, std::size_t cols, Rational value)
        : rows_(rows),
          cols_(cols),
          data_(rows * cols, value) {}

    [[nodiscard]] std::size_t rows() const noexcept {
        return rows_;
    }

    [[nodiscard]] std::size_t cols() const noexcept {
        return cols_;
    }

    [[nodiscard]] bool empty() const noexcept {
        return rows_ == 0 || cols_ == 0;
    }

    [[nodiscard]] bool isSquare() const noexcept {
        return rows_ == cols_;
    }

    [[nodiscard]] Rational& at(std::size_t row, std::size_t col) {
        checkIndex(row, col);
        return data_[index(row, col)];
    }

    [[nodiscard]] const Rational& at(std::size_t row, std::size_t col) const {
        checkIndex(row, col);
        return data_[index(row, col)];
    }

    [[nodiscard]] Matrix transposed() const {
        Matrix result(cols_, rows_);

        for (std::size_t row = 0; row < rows_; ++row) {
            for (std::size_t col = 0; col < cols_; ++col) {
                result.at(col, row) = at(row, col);
            }
        }

        return result;
    }

    [[nodiscard]] std::string toString() const {
        std::string result;

        for (std::size_t row = 0; row < rows_; ++row) {
            result += "[";

            for (std::size_t col = 0; col < cols_; ++col) {
                if (col != 0) {
                    result += ", ";
                }

                result += at(row, col).toString();
            }

            result += "]";

            if (row + 1 != rows_) {
                result += "\n";
            }
        }

        return result;
    }

    [[nodiscard]] static Matrix identity(std::size_t size) {
        Matrix result(size, size);

        for (std::size_t i = 0; i < size; ++i) {
            result.at(i, i) = Rational{1};
        }

        return result;
    }

    friend Matrix operator+(const Matrix& left, const Matrix& right) {
        checkSameSize(left, right);

        Matrix result(left.rows_, left.cols_);

        for (std::size_t i = 0; i < left.data_.size(); ++i) {
            result.data_[i] = left.data_[i] + right.data_[i];
        }

        return result;
    }

    friend Matrix operator-(const Matrix& left, const Matrix& right) {
        checkSameSize(left, right);

        Matrix result(left.rows_, left.cols_);

        for (std::size_t i = 0; i < left.data_.size(); ++i) {
            result.data_[i] = left.data_[i] - right.data_[i];
        }

        return result;
    }

    friend Matrix operator-(const Matrix& matrix) {
        Matrix result(matrix.rows_, matrix.cols_);

        for (std::size_t i = 0; i < matrix.data_.size(); ++i) {
            result.data_[i] = -matrix.data_[i];
        }

        return result;
    }

    friend Matrix operator*(const Matrix& matrix, const Rational& value) {
        Matrix result(matrix.rows_, matrix.cols_);

        for (std::size_t i = 0; i < matrix.data_.size(); ++i) {
            result.data_[i] = matrix.data_[i] * value;
        }

        return result;
    }

    friend Matrix operator*(const Rational& value, const Matrix& matrix) {
        return matrix * value;
    }

    friend Matrix operator*(const Matrix& left, const Matrix& right) {
        if (left.cols_ != right.rows_) {
            throw core::MathError("matrix sizes do not match for multiplication");
        }

        Matrix result(left.rows_, right.cols_);

        for (std::size_t row = 0; row < left.rows_; ++row) {
            for (std::size_t col = 0; col < right.cols_; ++col) {
                Rational sum;

                for (std::size_t k = 0; k < left.cols_; ++k) {
                    sum = sum + left.at(row, k) * right.at(k, col);
                }

                result.at(row, col) = sum;
            }
        }

        return result;
    }

    friend bool operator==(const Matrix& left, const Matrix& right) noexcept {
        return left.rows_ == right.rows_
            && left.cols_ == right.cols_
            && left.data_ == right.data_;
    }

    friend bool operator!=(const Matrix& left, const Matrix& right) noexcept {
        return !(left == right);
    }

<<<<<<< HEAD
    [[nodiscard]] Rational determinant() const {
        if (!isSquare()) {
            throw core::MathError("matrix must be square to calculate determinant");
        }
        if (empty()) {
            return Rational{0};
        }
        const std::size_t n = rows_;
        if (n == 1) {
            return at(0, 0);
        }
        if (n == 2) {
            return at(0, 0) * at(1, 1) - at(0, 1) * at(1, 0);
        }

        Matrix temp = *this;
        Rational detVal{1};
        bool sign = true; // true = +, false = -

        for (std::size_t i = 0; i < n; ++i) {
            // Find pivot
            std::size_t pivot = i;
            while (pivot < n && temp.at(pivot, i).isZero()) {
                ++pivot;
            }

            if (pivot == n) {
                return Rational{0};
            }

            if (pivot != i) {
                // Swap rows i and pivot
                for (std::size_t col = 0; col < n; ++col) {
                    std::swap(temp.at(i, col), temp.at(pivot, col));
                }
                sign = !sign;
            }

            detVal = detVal * temp.at(i, i);

            // Eliminate column elements below diagonal
            for (std::size_t row = i + 1; row < n; ++row) {
                if (temp.at(row, i).isZero()) {
                    continue;
                }
                Rational factor = temp.at(row, i) / temp.at(i, i);
                for (std::size_t col = i; col < n; ++col) {
                    temp.at(row, col) = temp.at(row, col) - factor * temp.at(i, col);
                }
            }
        }

        return sign ? detVal : -detVal;
    }

=======
>>>>>>> main
private:
    std::size_t rows_ = 0;
    std::size_t cols_ = 0;
    std::vector<Rational> data_;

    [[nodiscard]] std::size_t index(std::size_t row, std::size_t col) const noexcept {
        return row * cols_ + col;
    }

    void checkIndex(std::size_t row, std::size_t col) const {
        if (row >= rows_ || col >= cols_) {
            throw core::MathError("matrix index is out of range");
        }
    }

    static void checkSameSize(const Matrix& left, const Matrix& right) {
        if (left.rows_ != right.rows_ || left.cols_ != right.cols_) {
            throw core::MathError("matrix sizes do not match");
        }
    }
};

}