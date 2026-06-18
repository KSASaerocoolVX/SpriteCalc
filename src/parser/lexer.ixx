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
    Percent,
    Caret,
    End
};

struct Token {
    TokenKind kind;
    std::string_view lexeme;
};

class Lexer {
public:
    [[nodiscard]] std::vector<Token> tokenize(std::string_view input) const {
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
                case '%':
                    tokens.push_back({TokenKind::Percent, "%"});
                    break;
                case '^':
                    tokens.push_back({TokenKind::Caret, "^"});
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
        ++index; // skip '['
        int openBrackets = 1;
        while (index < input.size()) {
            if (input[index] == '[') {
                ++openBrackets;
            } else if (input[index] == ']') {
                --openBrackets;
                if (openBrackets == 0) {
                    ++index;
                    return {TokenKind::Matrix, input.substr(start, index - start)};
                }
            }
            ++index;
        }
        throw core::SyntaxError("unmatched '[' in matrix literal");
    }

    [[nodiscard]] Token readPolynomial(std::string_view input, std::size_t& index) const {
        const auto start = index;
        ++index; // skip '{'
        int openBraces = 1;
        while (index < input.size()) {
            if (input[index] == '{') {
                ++openBraces;
            } else if (input[index] == '}') {
                --openBraces;
                if (openBraces == 0) {
                    ++index;
                    return {TokenKind::Polynomial, input.substr(start, index - start)};
                }
            }
            ++index;
        }
        throw core::SyntaxError("unmatched '{' in polynomial literal");
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
