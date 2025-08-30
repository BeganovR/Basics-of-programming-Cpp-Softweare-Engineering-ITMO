#pragma once
#include <cctype>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum class TokenType {
    Number,
    String,
    Identifier,
    Keyword,
    Operator,
    Symbol,
    EndOfFile,
    Boolean,
    Nil,
    Break,
    Continue
};

struct Position {
    int line = 1;
    int column = 1;
    Position(int line, int column) : line(line), column(column) {}
};

struct Token {
    TokenType type;
    std::string value;
    Position position;
    std::string ToString() const {
        return "Token(" + std::to_string(static_cast<int>(type)) + ", \"" +
               value + "\")";
    }
};
std::ostream& operator<<(std::ostream& os, const Token& token);

class Lexer {
   public:
    Lexer(const std::string& input) : input_(input), pos_(0), current_(' ') {}
    Lexer() = default;

    std::vector<Token> Tokenize() {
        std::vector<Token> tokens;
        Advance();

        while (!IsAtEnd()) {
            SkipWhitespaceAndComments();
            if (IsAtEnd()) break;

            Position start = CurrentPosition();
            std::cout << "[DEBUG] Current char: '" << current_
                      << "' at position: " << start.line << ":" << start.column
                      << std::endl;

            if ((current_ == '+' || current_ == '-') &&
                (std::isdigit(Peek()) ||
                 (Peek() == '.' && pos_ < input_.size() &&
                  std::isdigit(input_[pos_])))) {
                std::string value = ReadNumber();
                tokens.push_back(Token{TokenType::Number, value, start});
                std::cout << "[DEBUG] Number token: " << value << std::endl;
            }

            else if (std::isdigit(current_) ||
                     (current_ == '.' && std::isdigit(Peek()))) {
                std::string value = ReadNumber();
                tokens.push_back(Token{TokenType::Number, value, start});
                std::cout << "[DEBUG] Number token: " << value << std::endl;
            } else if (std::isalpha(current_)) {
                std::string word = ExtractWhile(
                    [](char c) { return std::isalnum(c) || c == '_'; });

                if (word == "true" || word == "false") {
                    tokens.emplace_back(TokenType::Boolean, word, start);
                    std::cout << "[DEBUG] Boolean: " << word << "\n";
                } else if (word == "nil") {
                    tokens.emplace_back(TokenType::Nil, word, start);
                    std::cout << "[DEBUG] Nil: " << word << "\n";
                }

                else if (keywords_.count(word)) {
                    tokens.emplace_back(TokenType::Keyword, word, start);
                    std::cout << "[DEBUG] Keyword: " << word << "\n";
                }

                else {
                    tokens.emplace_back(TokenType::Identifier, word, start);
                    std::cout << "[DEBUG] Identifier: " << word << "\n";
                }
                continue;
            }

            else if (std::isalpha(current_) || current_ == '_') {
                std::string word = ReadIdentifier();
                TokenType type = keywords_.count(word) ? TokenType::Keyword
                                                       : TokenType::Identifier;
                tokens.push_back(Token{type, word, start});
                std::cout << "[DEBUG] Identifier/Keyword token: " << word
                          << std::endl;
            }

            else if (current_ == '"') {
                std::string str = ReadString();
                tokens.push_back(Token{TokenType::String, str, start});
                std::cout << "[DEBUG] String token: " << str << std::endl;
            }

            else if (IsOperatorStart(current_)) {
                std::string op = ReadOperator();
                tokens.push_back(Token{TokenType::Operator, op, start});
                std::cout << "[DEBUG] Operator token: " << op << std::endl;
            }

            else if (current_ == '=') {
                std::string op(1, current_);
                tokens.push_back(Token{TokenType::Operator, op, start});
                Advance();
                std::cout << "[DEBUG] Operator token: " << op << std::endl;
            }

            else if (current_ == ';' || current_ == ',' || current_ == '(' ||
                     current_ == ')' || current_ == '[' || current_ == ']') {
                std::string sym(1, current_);
                tokens.push_back(Token{TokenType::Symbol, sym, start});
                Advance();
                std::cout << "[DEBUG] Symbol token: " << sym << std::endl;
            }

            else {
                throw std::runtime_error("Invalid character: " +
                                         std::string(1, current_));
            }
        }

        tokens.push_back(Token{TokenType::EndOfFile, "", CurrentPosition()});
        return tokens;
    }

    std::string ExtractWhile(std::function<bool(char)> predicate) {
        std::string result;
        while (!IsAtEnd() && predicate(current_)) {
            result += current_;
            Advance();
        }
        return result;
    }

    bool IsOperatorStart(char c) const {
        return c == '=' || c == '<' || c == '>' || c == '!' || c == '+' ||
               c == '-' || c == '*' || c == '/' || c == '&' || c == '|' ||
               c == '^' || c == ':' || c == '%';
    }

    std::string ReadOperator() {
        std::string op(1, current_);
        Advance();

        if ((op == "=" && current_ == '=') || (op == "<" && current_ == '=') ||
            (op == ">" && current_ == '=') || (op == "!" && current_ == '=') ||
            (op == "&" && current_ == '&') || (op == "|" && current_ == '|') ||
            (op == "%" && current_ == '=')) {
            op += current_;
            Advance();
        }

        return op;
    }

   private:
    const std::string input_;
    size_t pos_;
    char current_;
    int line_ = 1;
    int column_ = 0;

    const std::unordered_map<std::string, bool> keywords_ = {
        {"if", true},       {"else", true},  {"then", true}, {"end", true},
        {"function", true}, {"while", true}, {"for", true},  {"in", true},
        {"return", true},   {"and", true},   {"or", true},   {"not", true},
        {"true", true},     {"false", true}, {"nil", true},  {"break", true},
        {"continue", true}};

    char Peek() const { return (pos_ < input_.size()) ? input_[pos_] : '\0'; }

    char PeekNext() const {
        return (pos_ + 1 < input_.size()) ? input_[pos_ + 1] : '\0';
    }

    void Advance() {
        if (pos_ < input_.size()) {
            current_ = input_[pos_++];
        } else {
            current_ = '\0';
        }

        if (current_ == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
    }

    void SkipWhitespaceAndComments() {
        while (true) {
            while (std::isspace(current_)) {
                Advance();
            }

            if (current_ == '/' && Peek() == '/') {
                Advance();
                Advance();
                while (current_ != '\n' && !IsAtEnd()) {
                    Advance();
                }
            }

            else if (current_ == '/' && Peek() == '*') {
                Advance();
                Advance();
                while (!IsAtEnd()) {
                    if (current_ == '*' && Peek() == '/') {
                        Advance();
                        Advance();
                        break;
                    }
                    Advance();
                }
            } else {
                break;
            }
        }
    }

    std::string ReadNumber() {
        std::stringstream ss;

        if (current_ == '+' || current_ == '-') {
            ss << current_;
            Advance();
        }

        bool dot_seen = false;
        bool e_seen = false;

        while (std::isdigit(current_) || current_ == '.' || current_ == 'e' ||
               current_ == 'E') {
            if (current_ == '.') {
                if (dot_seen) {
                    throw std::runtime_error(
                        "Invalid number format: multiple dots");
                }
                if (e_seen) {
                    throw std::runtime_error(
                        "Invalid number format: dot after exponent");
                }
                dot_seen = true;
                ss << current_;
                Advance();
            } else if ((current_ == 'e' || current_ == 'E') && !e_seen) {
                e_seen = true;
                ss << current_;
                Advance();

                if (current_ == '+' || current_ == '-') {
                    ss << current_;
                    Advance();
                }

                if (!std::isdigit(current_)) {
                    throw std::runtime_error("Invalid exponent format");
                }

                while (std::isdigit(current_)) {
                    ss << current_;
                    Advance();
                }

                break;
            } else if (std::isdigit(current_)) {
                ss << current_;
                Advance();
            } else {
                break;
            }
        }

        std::string result = ss.str();
        if (result.empty()) {
            throw std::runtime_error("Failed to read a valid number");
        }
        return result;
    }

    std::string ReadIdentifier() {
        std::string result;
        while (std::isalnum(current_) || current_ == '_') {
            result += current_;
            Advance();
        }
        return result;
    }

    std::string ReadString() {
        std::string result;
        Advance();

        while (current_ != '"' && !IsAtEnd()) {
            if (current_ == '\\') {
                Advance();
                if (current_ == 'n')
                    result += '\n';
                else if (current_ == 't')
                    result += '\t';
                else if (current_ == '\\')
                    result += '\\';
                else if (current_ == '"')
                    result += '"';
                else
                    result += current_;
            } else {
                result += current_;
            }
            Advance();
        }

        if (current_ != '"') {
            throw std::runtime_error("Unterminated string literal");
        }

        Advance();
        return result;
    }

    bool IsAtEnd() const { return current_ == '\0'; }

    Position CurrentPosition() const { return Position{line_, column_}; }

    void ValidateCharacter() {
        if (!std::isalnum(current_) && !IsOperatorStart(current_) &&
            current_ != '"' && current_ != ' ' && current_ != '\t' &&
            current_ != '\n') {
            throw std::runtime_error("Invalid character: " +
                                     std::string(1, current_));
        }
    }
};

inline bool operator==(const Position& lhs, const Position& rhs) {
    return lhs.line == rhs.line && lhs.column == rhs.column;
}

inline bool operator==(const Token& lhs, const Token& rhs) {
    return lhs.type == rhs.type && lhs.value == rhs.value &&
           lhs.position == rhs.position;
}

inline bool operator!=(const Token& lhs, const Token& rhs) {
    return !(lhs == rhs);
}