module;

#include <cctype>
#include <string>
#include <string_view>
#include <vector>

export module parser.lexer;

import core.exceptions;

export namespace parser {

enum class TokenKind {
    Number,
    Matrix,
    Polynomial,
    Identifier,
    Plus,
    Minus,
    Star,
    Slash,
    Comma,
    LeftParen,
    RightParen,
    Equal,
    End
};

struct Token {
    TokenKind kind;
    std::string_view lexeme;
};

class Lexer {
public:
    [[nodiscard]] std::vector<Token> tokenize(std::string_view input) const {
        if (const auto latexPolynomial = tokenizeWholeLatexPolynomial(input); !latexPolynomial.empty()) {
            return latexPolynomial;
        }

        std::vector<Token> tokens;
        std::size_t index = 0;

        while (index < input.size()) {
            const auto current = input[index];

            if (std::isspace(static_cast<unsigned char>(current))) {
                ++index;
                continue;
            }

            if (std::isdigit(static_cast<unsigned char>(current)) || current == '.') {
                tokens.push_back(readNumber(input, index));
                continue;
            }

            if (current == '\\') {
                tokens.push_back(readLatex(input, index));
                continue;
            }

            if (current == '[') {
                tokens.push_back(readMatrix(input, index));
                continue;
            }

            if (current == '{') {
                tokens.push_back(readPolynomial(input, index));
                continue;
            }

            if (std::isalpha(static_cast<unsigned char>(current)) || current == '_') {
                tokens.push_back(readIdentifier(input, index));
                continue;
            }

            switch (current) {
                case '+':
                    tokens.push_back({TokenKind::Plus, "+"});
                    break;
                case '-':
                    tokens.push_back({TokenKind::Minus, "-"});
                    break;
                case '*':
                    tokens.push_back({TokenKind::Star, "*"});
                    break;
                case '/':
                    tokens.push_back({TokenKind::Slash, "/"});
                    break;
                case ',':
                    tokens.push_back({TokenKind::Comma, ","});
                    break;
                case '(':
                    tokens.push_back({TokenKind::LeftParen, "("});
                    break;
                case ')':
                    tokens.push_back({TokenKind::RightParen, ")"});
                    break;
                case '=':
                    tokens.push_back({TokenKind::Equal, "="});
                    break;
                default:
                    throw core::SyntaxError("unexpected character: " + std::string{current});
            }

            ++index;
        }

        tokens.push_back({TokenKind::End, ""});
        return tokens;
    }

private:
    [[nodiscard]] std::vector<Token> tokenizeWholeLatexPolynomial(std::string_view input) const {
        const auto trimmed = trim(input);
        if (trimmed.empty()) {
            return {};
        }

        const auto equals = trimmed.find('=');
        if (equals != std::string_view::npos) {
            const auto name = trim(trimmed.substr(0, equals));
            const auto polynomial = trim(trimmed.substr(equals + 1));

            if (!isIdentifier(name) || !looksLikeLatexPolynomial(polynomial)) {
                return {};
            }

            return {
                {TokenKind::Identifier, name},
                {TokenKind::Equal, "="},
                {TokenKind::Polynomial, polynomial},
                {TokenKind::End, ""}
            };
        }

        if (!looksLikeLatexPolynomial(trimmed)) {
            return {};
        }

        return {
            {TokenKind::Polynomial, trimmed},
            {TokenKind::End, ""}
        };
    }

    [[nodiscard]] std::string_view trim(std::string_view text) const {
        const auto first = text.find_first_not_of(" \t\r\n");
        if (first == std::string_view::npos) {
            return {};
        }

        const auto last = text.find_last_not_of(" \t\r\n");
        return text.substr(first, last - first + 1);
    }

    [[nodiscard]] bool isIdentifier(std::string_view text) const {
        if (text.empty()) {
            return false;
        }

        const auto first = text.front();
        if (!std::isalpha(static_cast<unsigned char>(first)) && first != '_') {
            return false;
        }

        for (const auto ch : text.substr(1)) {
            if (!std::isalnum(static_cast<unsigned char>(ch)) && ch != '_') {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] bool looksLikeLatexPolynomial(std::string_view text) const {
        if (text.starts_with("\\poly{")) {
            return true;
        }

        bool hasX = false;
        for (const auto ch : text) {
            if (ch == 'x') {
                hasX = true;
                continue;
            }

            if (
                std::isdigit(static_cast<unsigned char>(ch)) ||
                std::isspace(static_cast<unsigned char>(ch)) ||
                ch == '+' || ch == '-' || ch == '.' || ch == '*' ||
                ch == '/' || ch == '^' || ch == '{' || ch == '}'
            ) {
                continue;
            }

            return false;
        }

        return hasX;
    }

    [[nodiscard]] Token readNumber(std::string_view input, std::size_t& index) const {
        const auto start = index;
        bool hasDot = false;
        bool hasDigit = false;

        while (index < input.size()) {
            const auto current = input[index];
            if (std::isdigit(static_cast<unsigned char>(current))) {
                hasDigit = true;
                ++index;
                continue;
            }

            if (current == '.') {
                if (hasDot) {
                    throw core::SyntaxError("invalid number literal");
                }
                hasDot = true;
                ++index;
                continue;
            }

            break;
        }

        if (!hasDigit) {
            throw core::SyntaxError("invalid number literal");
        }

        return {TokenKind::Number, input.substr(start, index - start)};
    }

    [[nodiscard]] Token readMatrix(std::string_view input, std::size_t& index) const {
        const auto start = index;
        ++index;

        while (index < input.size() && input[index] != ']') {
            ++index;
        }

        if (index >= input.size()) {
            throw core::SyntaxError("unclosed matrix literal");
        }

        ++index;
        return {TokenKind::Matrix, input.substr(start, index - start)};
    }

    [[nodiscard]] Token readLatex(std::string_view input, std::size_t& index) const {
        if (input.substr(index).starts_with("\\begin{")) {
            return readLatexMatrix(input, index);
        }

        if (input.substr(index).starts_with("\\poly{")) {
            return readLatexPolynomial(input, index);
        }

        throw core::SyntaxError("unsupported LaTeX command");
    }

    [[nodiscard]] Token readLatexMatrix(std::string_view input, std::size_t& index) const {
        const auto start = index;
        const auto beginClose = input.find('}', index + 7);
        if (beginClose == std::string_view::npos) {
            throw core::SyntaxError("invalid LaTeX matrix begin");
        }

        const auto environment = input.substr(index + 7, beginClose - (index + 7));
        const auto endTag = "\\end{" + std::string{environment} + "}";
        const auto end = input.find(endTag, beginClose + 1);
        if (end == std::string_view::npos) {
            throw core::SyntaxError("unclosed LaTeX matrix literal");
        }

        index = end + endTag.size();
        return {TokenKind::Matrix, input.substr(start, index - start)};
    }

    [[nodiscard]] Token readLatexPolynomial(std::string_view input, std::size_t& index) const {
        const auto start = index;
        index += 6;
        int depth = 1;

        while (index < input.size() && depth > 0) {
            if (input[index] == '{') {
                ++depth;
            } else if (input[index] == '}') {
                --depth;
            }
            ++index;
        }

        if (depth != 0) {
            throw core::SyntaxError("unclosed LaTeX polynomial literal");
        }

        return {TokenKind::Polynomial, input.substr(start, index - start)};
    }

    [[nodiscard]] Token readPolynomial(std::string_view input, std::size_t& index) const {
        const auto start = index;
        ++index;

        while (index < input.size() && input[index] != '}') {
            ++index;
        }

        if (index >= input.size()) {
            throw core::SyntaxError("unclosed polynomial literal");
        }

        ++index;
        return {TokenKind::Polynomial, input.substr(start, index - start)};
    }

    [[nodiscard]] Token readIdentifier(std::string_view input, std::size_t& index) const {
        const auto start = index;

        while (index < input.size()) {
            const auto current = input[index];
            if (!std::isalnum(static_cast<unsigned char>(current)) && current != '_') {
                break;
            }
            ++index;
        }

        return {TokenKind::Identifier, input.substr(start, index - start)};
    }
};

}
