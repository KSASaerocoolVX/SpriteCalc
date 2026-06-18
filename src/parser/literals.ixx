module;

#include <string>
#include <string_view>
#include <vector>
#include <stdexcept>
#include <cctype>
#include <cstddef>

export module parser.literals;

import core.exceptions;
import math.rational;
import math.matrix;
import math.poly;

export namespace parser {

inline math::Rational parseDecimalOrInt(const std::string& text) {
    if (text.empty()) {
        throw core::SyntaxError("empty number literal");
    }
    const auto dot = text.find('.');
    if (dot == std::string::npos) {
        try {
            return math::Rational(std::stoll(text));
        } catch (...) {
            throw core::SyntaxError("integer literal is too large or invalid: " + text);
        }
    }
    std::string whole = text.substr(0, dot);
    std::string frac = text.substr(dot + 1);
    long long wholeVal = 0;
    if (!whole.empty()) {
        try {
            wholeVal = std::stoll(whole);
        } catch (...) {
            throw core::SyntaxError("invalid integer literal in decimal: " + whole);
        }
    }
    if (frac.empty()) {
        return math::Rational(wholeVal);
    }
    long long fracVal = 0;
    try {
        fracVal = std::stoll(frac);
    } catch (...) {
        throw core::SyntaxError("invalid fractional literal in decimal: " + frac);
    }
    long long denom = 1;
    for (std::size_t i = 0; i < frac.size(); ++i) {
        denom *= 10;
    }
    math::Rational w(wholeVal);
    math::Rational f(fracVal, denom);
    return wholeVal >= 0 ? w + f : w - f;
}

inline math::Matrix parseMatrix(std::string_view text) {
    if (text.size() < 2 || text.front() != '[' || text.back() != ']') {
        throw core::SyntaxError("invalid matrix syntax");
    }
    std::string_view inner = text.substr(1, text.size() - 2);
    std::vector<std::vector<math::Rational>> rows;
    std::size_t start = 0;
    while (start < inner.size()) {
        auto end = inner.find(';', start);
        if (end == std::string_view::npos) {
            end = inner.size();
        }
        std::string_view row_text = inner.substr(start, end - start);
        std::vector<math::Rational> row;
        std::size_t idx = 0;
        while (idx < row_text.size()) {
            while (idx < row_text.size() && (std::isspace(static_cast<unsigned char>(row_text[idx])) || row_text[idx] == ',')) {
                ++idx;
            }
            if (idx >= row_text.size()) break;
            std::size_t num_start = idx;
            if (row_text[idx] == '-') {
                ++idx;
            }
            while (idx < row_text.size() && (std::isdigit(static_cast<unsigned char>(row_text[idx])) || row_text[idx] == '.')) {
                ++idx;
            }
            if (num_start == idx || (idx - num_start == 1 && row_text[num_start] == '-')) {
                throw core::SyntaxError("invalid number in matrix literal: " + std::string(row_text.substr(num_start, 1)));
            }
            std::string num_str{row_text.substr(num_start, idx - num_start)};
            row.push_back(parseDecimalOrInt(num_str));
        }
        if (!row.empty()) {
            rows.push_back(std::move(row));
        }
        start = end + 1;
    }
    if (rows.empty()) {
        throw core::SyntaxError("empty matrix");
    }
    std::size_t row_count = rows.size();
    std::size_t col_count = rows[0].size();
    for (const auto& r : rows) {
        if (r.size() != col_count) {
            throw core::SyntaxError("matrix rows must have the same number of columns");
        }
    }
    math::Matrix m(row_count, col_count);
    for (std::size_t r = 0; r < row_count; ++r) {
        for (std::size_t c = 0; c < col_count; ++c) {
            m.at(r, c) = rows[r][c];
        }
    }
    return m;
}

inline math::Polynomial parsePolynomial(std::string_view text) {
    if (text.size() < 2 || text.front() != '{' || text.back() != '}') {
        throw core::SyntaxError("invalid polynomial syntax");
    }
    std::string_view inner = text.substr(1, text.size() - 2);
    std::vector<math::Rational> coeffs;
    std::size_t idx = 0;
    while (idx < inner.size()) {
        while (idx < inner.size() && (std::isspace(static_cast<unsigned char>(inner[idx])) || inner[idx] == ',' || inner[idx] == ';')) {
            ++idx;
        }
        if (idx >= inner.size()) break;
        std::size_t num_start = idx;
        if (inner[idx] == '-') {
            ++idx;
        }
        while (idx < inner.size() && (std::isdigit(static_cast<unsigned char>(inner[idx])) || inner[idx] == '.')) {
            ++idx;
        }
        if (num_start == idx || (idx - num_start == 1 && inner[num_start] == '-')) {
            throw core::SyntaxError("invalid number in polynomial literal: " + std::string(inner.substr(num_start, 1)));
        }
        std::string num_str{inner.substr(num_start, idx - num_start)};
        coeffs.push_back(parseDecimalOrInt(num_str));
    }
    return math::Polynomial(std::move(coeffs));
}

}
