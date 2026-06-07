module;

#include <cstddef>
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

    [[nodiscard]] Rational& at(std::size_t row, std::size_t col) {
        checkIndex(row, col);
        return data_[index(row, col)];
    }

    [[nodiscard]] const Rational& at(std::size_t row, std::size_t col) const {
        checkIndex(row, col);
        return data_[index(row, col)];
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