#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "AST.h"
#include "lexer.h"

class Parser {
   public:
    explicit Parser(const std::vector<Token>& tokens)
        : tokens_(tokens), current_(0) {}

    std::shared_ptr<ASTNode> Parse() {
        std::vector<std::shared_ptr<ASTNode>> statements;

        while (!IsAtEnd()) {
            std::cout << "[DEBUG] Peek 0: " << Peek().ToString() << std::endl;
            if (current_ + 1 < tokens_.size())
                std::cout << "[DEBUG] Peek 1: "
                          << tokens_[current_ + 1].ToString() << std::endl;
            if (current_ + 2 < tokens_.size())
                std::cout << "[DEBUG] Peek 2: "
                          << tokens_[current_ + 2].ToString() << std::endl;

            auto stmt = ParseStatement();
            if (stmt) {
                std::cout << "[DEBUG] Parsed statement: " << stmt->ToString()
                          << std::endl;
                statements.push_back(stmt);
            } else {
                std::cout << "[DEBUG] Skipped null statement or reached end.\n";
            }
        }
        if (!IsAtEnd()) {
            std::cout
                << "[DEBUG] Остались необработанные токены после Parse():\n";
            while (!IsAtEnd()) {
                std::cout << "  - " << Peek().ToString() << "\n";
                Advance();
            }
        }

        auto block = std::make_shared<BlockNode>(statements);
        std::cout << "[DEBUG] Final BlockNode: " << block->ToString()
                  << std::endl;
        return block;
    }

   private:
    const std::vector<Token>& tokens_;
    size_t current_;

    std::shared_ptr<BlockNode> ParseBlock() {
        std::vector<std::shared_ptr<ASTNode>> statements;
        while (!Check(TokenType::Keyword, "end") &&
               !Check(TokenType::Keyword, "else") && !IsAtEnd()) {
            statements.push_back(ParseStatement());
        }
        return std::make_shared<BlockNode>(statements);
    }

    std::shared_ptr<ASTNode> ParseStatement() {
        while (!IsAtEnd() && Peek().type == TokenType::Symbol &&
               (Peek().value == ";" || Peek().value == ",")) {
            Advance();
        }

        if (IsAtEnd()) {
            std::cout << "[DEBUG] End of input reached in ParseStatement()\n";
            return nullptr;
        }

        if (Match(TokenType::Keyword, "break")) {
            return std::make_shared<BreakStatement>();
        }

        if (Match(TokenType::Keyword, "continue")) {
            return std::make_shared<ContinueStatement>();
        }

        if (Match(TokenType::Keyword, "for")) {
            std::cout << "[DEBUG] --- Начало FOR ---\n";
            auto var_name = Consume(TokenType::Identifier).value;

            if (Match(TokenType::Keyword, "in")) {
                // Существующий синтаксис: for i in range(start,end,step)
                Consume(TokenType::Identifier, "range");
                Consume(TokenType::Symbol, "(");

                auto start = ParseExpression();
                Consume(TokenType::Symbol, ",");
                auto end = ParseExpression();
                Consume(TokenType::Symbol, ",");
                auto step = ParseExpression();
                Consume(TokenType::Symbol, ")");

                auto body = ParseBlock();
                Consume(TokenType::Keyword, "end");
                Consume(TokenType::Keyword, "for");

                std::cout << "[DEBUG] --- Конец FOR ---\n";
                return std::make_shared<ForLoop>(var_name, start, end, step,
                                                 body);
            } else if (Match(TokenType::Operator, "=")) {
                auto init = ParseExpression();
                Consume(TokenType::Symbol, ",");
                auto condition = ParseExpression();
                Consume(TokenType::Symbol, ",");
                auto step = ParseExpression();

                auto body = ParseBlock();
                Consume(TokenType::Keyword, "end");
                Consume(TokenType::Keyword, "for");

                std::cout << "[DEBUG] --- Конец FOR ---\n";
                return std::make_shared<ForLoop>(var_name, init, condition,
                                                 step, body);
            } else {
                throw std::runtime_error(
                    "Expected 'in' or '=' after variable name in for loop");
            }
        }
        if (Peek().type == TokenType::Nil) {
            Advance();
            return std::make_shared<NilLiteral>();
        }

        if (Match(TokenType::Keyword, "while")) {
            std::cout << "[DEBUG] --- Начало While ---\n";

            auto condition = ParseExpression();
            std::cout << "[DEBUG] Условие While: " << condition->ToString()
                      << "\n";

            auto body = ParseBlock();
            std::cout << "[DEBUG] Тело While содержит "
                      << body->statements.size() << " инструкций\n";

            if (!Match(TokenType::Keyword, "end")) {
                throw std::runtime_error("Expected 'end' after while block");
            }
            if (!Match(TokenType::Keyword, "while")) {
                throw std::runtime_error("Expected 'while' after 'end'");
            }

            std::cout << "[DEBUG] --- Конец While ---\n";
            return std::make_shared<WhileLoop>(condition, body);
        }
        if (Match(TokenType::Keyword, "if")) {
            std::cout << "[DEBUG] --- Начало IF ---\n";
            auto condition = ParseExpression();
            std::cout << "[DEBUG] Условие IF: " << condition->ToString()
                      << "\n";
            Consume(TokenType::Keyword, "then");
            auto then_block = ParseBlock();
            std::cout << "[DEBUG] THEN блок содержит "
                      << then_block->statements.size() << " инструкций\n";

            std::shared_ptr<ASTNode> else_block = nullptr;

            if (Match(TokenType::Keyword, "else")) {
                if (Check(TokenType::Keyword, "if")) {
                    std::cout << "[DEBUG] ELSE IF вложенный\n";
                    else_block = ParseStatement();
                    return std::make_shared<IfStatement>(condition, then_block,
                                                         else_block);
                } else {
                    std::cout << "[DEBUG] ELSE обычный блок\n";
                    else_block = ParseBlock();
                    std::cout
                        << "[DEBUG] ELSE блок содержит "
                        << std::dynamic_pointer_cast<BlockNode>(else_block)
                               ->statements.size()
                        << " инструкций\n";
                }
            }

            std::cout << "[DEBUG] --- Конец IF ---\n";
            Consume(TokenType::Keyword, "end");
            Consume(TokenType::Keyword, "if");

            return std::make_shared<IfStatement>(condition, then_block,
                                                 else_block);
        }

        if (Check(TokenType::Identifier) &&
            CheckNext(TokenType::Operator, "=") &&
            CheckLookahead(2, TokenType::Keyword, "function")) {
            std::string func_name = Peek().value;
            std::cout << "[DEBUG] Parsing function declaration: " << func_name
                      << "\n";

            Advance();
            Consume(TokenType::Operator, "=");
            Consume(TokenType::Keyword, "function");

            return ParseFunctionDeclarationWithGivenName(func_name);
        }

        if (Check(TokenType::Keyword, "function")) {
            std::cout << "[DEBUG] Parsing anonymous function declaration\n";
            Advance();
            return ParseFunctionDeclaration();
        }

        if (Match(TokenType::Keyword, "return")) {
            std::cout << "[DEBUG] Parsing return statement\n";
            auto expr = ParseExpression();
            Match(TokenType::Symbol, ";");
            return std::make_shared<ReturnStatement>(expr);
        }

        if (Check(TokenType::Identifier) &&
            CheckNext(TokenType::Operator, "=")) {
            std::cout << "[DEBUG] Parsing assignment\n";
            return ParseAssignment();
        }

        std::cout << "[DEBUG] Parsing expression\n";
        auto expr = ParseExpression();
        Match(TokenType::Symbol, ";");
        return expr;
    }

    bool CheckLookahead(int offset, TokenType type,
                        const std::string& value = "") const {
        if (current_ + offset >= tokens_.size()) return false;
        const Token& t = tokens_[current_ + offset];
        if (t.type != type) return false;
        if (!value.empty() && t.value != value) return false;
        return true;
    }

    std::shared_ptr<ASTNode> ParseArrayLiteral() {
        Consume(TokenType::Symbol, "[");
        std::vector<std::shared_ptr<ASTNode>> elements;

        while (!Check(TokenType::Symbol, "]") && !IsAtEnd()) {
            elements.push_back(ParseExpression());

            if (Match(TokenType::Symbol, ",")) {
                continue;
            } else if (!Check(TokenType::Symbol, "]")) {
                throw std::runtime_error(
                    "Expected ',' or ']' in array literal");
            }
        }

        Consume(TokenType::Symbol, "]");
        return std::make_shared<ArrayLiteral>(elements);
    }

    std::shared_ptr<FunctionDeclaration> ParseFunctionDeclarationWithGivenName(
        const std::string& name) {
        Consume(TokenType::Symbol, "(");

        std::vector<std::string> parameters;
        if (!Check(TokenType::Symbol, ")")) {
            do {
                auto param_token = Consume(TokenType::Identifier);
                parameters.push_back(param_token.value);
            } while (Match(TokenType::Symbol, ","));
        }
        Consume(TokenType::Symbol, ")");

        std::vector<std::shared_ptr<ASTNode>> body_statements;

        while (!Check(TokenType::Keyword, "end") && !IsAtEnd()) {
            auto stmt = ParseStatement();
            body_statements.push_back(stmt);
            std::cout << "[DEBUG] Added statement to function body: "
                      << stmt->ToString() << std::endl;
        }

        Consume(TokenType::Keyword, "end");
        Match(TokenType::Keyword, "function");

        std::cout << "[DEBUG] Total number of statements in function body: "
                  << body_statements.size() << std::endl;

        return std::make_shared<FunctionDeclaration>(
            name, parameters,
            std::make_shared<BlockNode>(std::move(body_statements)));
    }

    std::shared_ptr<FunctionDeclaration> ParseFunctionDeclarationWithoutName() {
        std::cout << "[DEBUG] Starting ParseFunctionDeclarationWithoutName()\n";

        if (!Check(TokenType::Symbol, "(")) {
            std::cout << "[ERROR] Expected '(' but found: " << Peek().value
                      << "\n";
            throw std::runtime_error("Expected '('");
        }

        Consume(TokenType::Symbol, "(");
        std::cout << "[DEBUG] Consumed '('\n";

        std::vector<std::string> parameters;

        if (!Check(TokenType::Symbol, ")")) {
            do {
                auto param_token = Consume(TokenType::Identifier);
                parameters.push_back(param_token.value);
                std::cout << "[DEBUG] Parsed parameter: " << param_token.value
                          << "\n";
            } while (Match(TokenType::Symbol, ","));
        }

        Consume(TokenType::Symbol, ")");
        std::cout << "[DEBUG] Consumed ')'\n";

        std::vector<std::shared_ptr<ASTNode>> body_statements;
        while (!Check(TokenType::Keyword, "end") && !IsAtEnd()) {
            body_statements.push_back(ParseStatement());
            std::cout << "[DEBUG] Parsing statement inside function body\n";
        }

        Consume(TokenType::Keyword, "end");
        std::cout << "[DEBUG] Consumed 'end'\n";

        Match(TokenType::Keyword, "function");
        std::cout << "[DEBUG] Consumed 'function'\n";

        std::cout << "[DEBUG] Returning FunctionDeclaration\n";
        return std::make_shared<FunctionDeclaration>(
            "", parameters,
            std::make_shared<BlockNode>(std::move(body_statements)));
    }

    bool CheckNextNext(TokenType type, const std::string& value = "") {
        return current_ + 2 < tokens_.size() &&
               tokens_[current_ + 2].type == type &&
               (value.empty() || tokens_[current_ + 2].value == value);
    }

    std::shared_ptr<ASTNode> ParseFunctionDeclaration() {
        auto name_token = Consume(TokenType::Identifier);
        std::string name = name_token.value;

        Consume(TokenType::Symbol, "(");
        std::vector<std::string> parameters;
        if (!Check(TokenType::Symbol, ")")) {
            do {
                auto param_token = Consume(TokenType::Identifier);
                parameters.push_back(param_token.value);
            } while (Match(TokenType::Symbol, ","));
        }
        Consume(TokenType::Symbol, ")");

        std::vector<std::shared_ptr<ASTNode>> body_statements;
        while (!Check(TokenType::Keyword, "end") && !IsAtEnd()) {
            body_statements.push_back(ParseStatement());
        }

        Consume(TokenType::Keyword, "end");
        Match(TokenType::Keyword, "function");

        return std::make_shared<FunctionDeclaration>(
            name, parameters,
            std::make_shared<BlockNode>(std::move(body_statements)));
    }

    std::shared_ptr<ASTNode> ParseAssignment() {
        std::string name = Advance().value;
        std::string op = Consume(TokenType::Operator).value;
        auto expr = ParseExpression();
        Match(TokenType::Symbol, ";");

        if (op == "=") {
            return std::make_shared<Assignment>(name, expr);
        } else if (op == "+=" || op == "-=" || op == "*=" || op == "/=" ||
                   op == "%=" || op == "^=") {
            std::string bin_op = op.substr(0, op.size() - 1);

            auto left = std::make_shared<Identifier>(name);
            auto bin_expr =
                std::make_shared<BinaryExpression>(left, bin_op, expr);
            return std::make_shared<Assignment>(name, bin_expr);
        } else {
            throw std::runtime_error("Unsupported assignment operator: " + op);
        }
    }

    std::shared_ptr<ASTNode> ParseExpression(int precedence = 0) {
        auto left = ParsePrimary();

        while (!IsAtEnd() &&
               ((Peek().type == TokenType::Operator) ||
                (Peek().type == TokenType::Keyword &&
                 (Peek().value == "and" || Peek().value == "or"))) &&
               GetPrecedence(Peek().value) >= precedence) {
            std::string op = Advance().value;
            int op_precedence = GetPrecedence(op);
            auto right = ParseExpression(op_precedence + 1);
            left = std::make_shared<BinaryExpression>(left, op, right);
        }

        return left;
    }

    int GetPrecedence(const std::string& op) {
        if (op == "*" || op == "/" || op == "%") return 10;
        if (op == "+" || op == "-") return 5;
        if (op == "==" || op == "!=" || op == ">" || op == "<" || op == ">=" ||
            op == "<=")
            return 1;
        if (op == "and" || op == "or") return 0;
        if (op == "not") return 15; 
        return 0;
    }

    std::shared_ptr<ASTNode> ParsePrimary() {
        std::shared_ptr<ASTNode> expr;
        if (Match(TokenType::Keyword, "not")) {
            auto operand = ParsePrimary();
            return std::make_shared<UnaryExpression>("not", operand);
        } else if (Match(TokenType::Keyword, "function")) {
            return ParseAnonymousFunction();
        } else if (Match(TokenType::Number)) {
            return std::make_shared<NumberLiteral>(Previous().value);
        } else if (Match(TokenType::String)) {
            return std::make_shared<StringLiteral>(Previous().value);
        } else if (Match(TokenType::Identifier)) {
            expr = std::make_shared<Identifier>(Previous().value);
        } else if (Match(TokenType::Symbol, "(")) {
            expr = ParseExpression();
            Consume(TokenType::Symbol, ")");
        } else if (Match(TokenType::Boolean)) {
            bool value = (Previous().value == "true");
            return std::make_shared<BooleanLiteral>(value);
        } else if (Match(TokenType::Nil, "nil")) {
            return std::make_shared<NilLiteral>();
        } else if (Check(TokenType::Symbol, "[")) {
            return ParseArrayLiteral();
        } else {
            throw std::runtime_error("Unexpected token in expression: " +
                                     Peek().value);
        }

        // обработка вызовов функций, индексирования и срезов
        while (true) {
            if (Match(TokenType::Symbol, "[")) {
                if (Check(TokenType::Symbol, ":")) {
                    // [:end] или [:]
                    Consume(TokenType::Symbol, ":");
                    std::shared_ptr<ASTNode> end = nullptr;
                    if (!Check(TokenType::Symbol, "]")) {
                        end = ParseExpression();
                    }
                    Consume(TokenType::Symbol, "]");
                    expr = std::make_shared<SliceAccess>(expr, nullptr, end);
                } else if (Check(TokenType::Symbol, "]")) {
                    // [] — невалидно
                    throw std::runtime_error("Unexpected empty brackets []");
                } else {
                    auto start = ParseExpression();

                    if (Match(TokenType::Symbol, ":")) {
                        std::shared_ptr<ASTNode> end = nullptr;
                        if (!Check(TokenType::Symbol, "]")) {
                            end = ParseExpression();
                        }
                        Consume(TokenType::Symbol, "]");
                        expr = std::make_shared<SliceAccess>(expr, start, end);
                    } else {
                        Consume(TokenType::Symbol, "]");
                        expr = std::make_shared<IndexAccess>(expr, start);
                    }
                }
            } else if (Match(TokenType::Symbol, "(")) {
                std::vector<std::shared_ptr<ASTNode>> args;
                if (!Check(TokenType::Symbol, ")")) {
                    args.push_back(ParseExpression());
                    while (Match(TokenType::Symbol, ",")) {
                        args.push_back(ParseExpression());
                    }
                }
                Consume(TokenType::Symbol, ")");
                expr = std::make_shared<FunctionCall>(expr, args);
            } else {
                break;
            }
        }

        return expr;
    }

    std::shared_ptr<ASTNode> ParseFunctionCall(const std::string& name) {
        Consume(TokenType::Symbol, "(");
        std::vector<std::shared_ptr<ASTNode>> args;

        if (!Check(TokenType::Symbol, ")")) {
            args.push_back(ParseExpression());
            while (Match(TokenType::Symbol, ",")) {
                args.push_back(ParseExpression());
            }
        }

        Consume(TokenType::Symbol, ")");

        return std::make_shared<FunctionCall>(
            std::make_shared<Identifier>(name), args);
    }

    std::shared_ptr<ASTNode> ParseAnonymousFunction() {
        Consume(TokenType::Symbol, "(");
        std::vector<std::string> parameters;
        while (Peek().type == TokenType::Identifier) {
            parameters.push_back(Advance().value);
            if (Check(TokenType::Symbol, ",")) {
                Advance();
            }
        }
        Consume(TokenType::Symbol, ")");

        Consume(TokenType::Keyword, "return");
        auto expr = ParseExpression();
        auto return_stmt = std::make_shared<ReturnStatement>(expr);

        Consume(TokenType::Keyword, "end");
        Consume(TokenType::Keyword, "function");

        std::vector<std::shared_ptr<ASTNode>> body_stmts = {return_stmt};
        return std::make_shared<AnonymousFunction>(parameters, body_stmts);
    }

    const Token& Peek() const { return tokens_[current_]; }

    const Token& Previous() const { return tokens_[current_ - 1]; }

    const Token& Advance() {
        if (!IsAtEnd()) ++current_;
        return tokens_[current_ - 1];
    }

    bool Match(TokenType type, const std::string& value = "") {
        if (Check(type, value)) {
            Advance();
            return true;
        }
        return false;
    }

    bool Check(TokenType type, const std::string& value = "") const {
        if (IsAtEnd()) return false;
        const Token& t = Peek();
        if (t.type != type) return false;
        if (!value.empty() && t.value != value) return false;
        return true;
    }

    bool CheckNext(TokenType type, const std::string& value = "") const {
        if (current_ + 1 >= tokens_.size()) return false;
        const Token& t = tokens_[current_ + 1];
        if (t.type != type) return false;
        if (!value.empty() && t.value != value) return false;
        return true;
    }

    Token Consume(TokenType type, const std::string& value = "") {
        if (value == "end function") {
            if (!Check(TokenType::Keyword, "end")) {
                throw std::runtime_error("Expected 'end'");
            }
            Advance();

            if (!Check(TokenType::Keyword, "function")) {
                throw std::runtime_error("Expected 'function' after 'end'");
            }
            Advance();

            return tokens_[current_ - 2];
        }

        if (!Check(type, value)) {
            throw std::runtime_error("Expected " + value);
        }
        return Advance();
    }

    bool IsAtEnd() const {
        return current_ >= tokens_.size() ||
               Peek().type == TokenType::EndOfFile;
    }
};