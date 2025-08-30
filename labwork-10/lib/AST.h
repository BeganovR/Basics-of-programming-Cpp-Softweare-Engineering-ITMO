#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual std::string ToString() const = 0;
    virtual std::string GetValue() const = 0;
};

class ConcreteASTNode : public ASTNode {
   public:
    std::string ToString() const override { return "ConcreteASTNode"; }

    std::string GetValue() const override { return "Some value"; }
};

struct NumberLiteral : ASTNode {
    std::string value;

    NumberLiteral(const std::string& value) : value(value) {}

    std::string ToString() const override { return "Number(" + value + ")"; }
    std::string GetValue() const override { return value; }
};

struct Identifier : ASTNode {
    std::string name;

    Identifier(const std::string& name) : name(name) {}

    std::string ToString() const override { return "Variable(" + name + ")"; }
    std::string GetValue() const override { return name; }
};

struct BinaryExpression : ASTNode {
    std::string op;
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;

    BinaryExpression(std::shared_ptr<ASTNode> left, const std::string& op,
                     std::shared_ptr<ASTNode> right)
        : left(left), op(op), right(right) {}

    std::string ToString() const override {
        return "Binary(" + left->ToString() + " " + op + " " +
               right->ToString() + ")";
    }

    std::string GetValue() const override { return op; }
};

struct BooleanLiteral : ASTNode {
    bool value;

    BooleanLiteral(bool value) : value(value) {}

    std::string ToString() const override { return value ? "true" : "false"; }

    std::string GetValue() const override { return value ? "true" : "false"; }
};

struct FunctionDeclaration : ASTNode {
    std::string name;
    std::vector<std::string> parameters;
    std::shared_ptr<ASTNode> body;

    FunctionDeclaration(std::string name, std::vector<std::string> params,
                        std::shared_ptr<ASTNode> body)
        : name(std::move(name)),
          parameters(std::move(params)),
          body(std::move(body)) {}

    std::string ToString() const override {
        std::string res = "FunctionDeclaration(" + name + ", [";
        for (size_t i = 0; i < parameters.size(); ++i) {
            res += parameters[i];
            if (i + 1 < parameters.size()) res += ", ";
        }
        res += "], " + body->ToString() + ")";
        return res;
    }

    std::string GetValue() const override { return ToString(); }
};

class BlockNode : public ASTNode {
   public:
    std::vector<std::shared_ptr<ASTNode>> statements;

    BlockNode(std::vector<std::shared_ptr<ASTNode>> stmts)
        : statements(std::move(stmts)) {}

    std::string ToString() const override {
        std::string res = "Block(";
        for (const auto& stmt : statements) {
            res += stmt->ToString() + ", ";
        }
        if (!statements.empty()) res.pop_back(), res.pop_back();
        return res + ")";
    }
    std::string GetValue() const override {
        std::string values;
        for (const auto& stmt : statements) {
            values += stmt->GetValue() + ", ";
        }
        if (!values.empty()) values.pop_back(), values.pop_back();
        return values.empty() ? "Empty Block" : values;
    }
};

class UnaryExpression : public ASTNode {
   public:
    std::string operator_;
    std::shared_ptr<ASTNode> operand;

    UnaryExpression(const std::string& op, std::shared_ptr<ASTNode> operand)
        : operator_(op), operand(std::move(operand)) {}

    std::string ToString() const override {
        return "Unary(" + operator_ + " " + operand->ToString() + ")";
    }

    std::string GetValue() const override {
        std::string operand_value = operand->GetValue();

        if (operator_ == "-") {
            return "-" + operand_value;
        } else if (operator_ == "!") {
            return (operand_value == "true" ? "false" : "true");
        }
        return operand_value;
    }
};

struct AnonymousFunction : ASTNode {
    std::vector<std::string> parameters;
    std::shared_ptr<ASTNode> body;

    AnonymousFunction(std::vector<std::string> params,
                      std::vector<std::shared_ptr<ASTNode>> body_stmts)
        : parameters(std::move(params)),
          body(std::make_shared<BlockNode>(std::move(body_stmts))) {}
    AnonymousFunction(std::vector<std::string> params,
                      std::shared_ptr<ASTNode> single_stmt)
        : parameters(std::move(params)),
          body(std::make_shared<BlockNode>(
              std::vector<std::shared_ptr<ASTNode>>{std::move(single_stmt)})) {}

    std::string ToString() const override {
        std::string res = "AnonymousFunction([";
        for (size_t i = 0; i < parameters.size(); ++i) {
            res += parameters[i];
            if (i + 1 < parameters.size()) res += ", ";
        }
        res += "], " + body->ToString() + ")";
        return res;
    }

    std::string GetValue() const override { return ToString(); }
};

struct FunctionCall : ASTNode {
    std::shared_ptr<ASTNode> target;
    std::vector<std::shared_ptr<ASTNode>> arguments;

    FunctionCall(const std::shared_ptr<ASTNode>& target_expr,
                 const std::vector<std::shared_ptr<ASTNode>>& args)
        : target(target_expr), arguments(args) {}

    std::string ToString() const override {
        std::string res = "Call(" + target->ToString() + ", [";
        for (size_t i = 0; i < arguments.size(); ++i) {
            res += arguments[i]->ToString();
            if (i + 1 < arguments.size()) res += ", ";
        }
        res += "])";
        return res;
    }

    std::string GetValue() const override { return target->GetValue(); }
};

struct Assignment : ASTNode {
    std::string name;
    std::shared_ptr<ASTNode> expression;

    Assignment(const std::string& name, const std::shared_ptr<ASTNode>& expr)
        : name(name), expression(expr) {}

    std::string ToString() const override {
        return "Assignment(" + name + ", " + expression->ToString() + ")";
    }

    std::string GetValue() const override { return expression->GetValue(); }
};

struct ReturnStatement : ASTNode {
    std::shared_ptr<ASTNode> expression;

    ReturnStatement(std::shared_ptr<ASTNode> expr)
        : expression(std::move(expr)) {}

    std::string ToString() const override {
        return "Return(" + expression->ToString() + ")";
    }

    std::string GetValue() const override { return expression->GetValue(); }
};

struct PrintStatement : ASTNode {
    std::shared_ptr<ASTNode> expression;

    PrintStatement(std::shared_ptr<ASTNode> expr)
        : expression(std::move(expr)) {}

    std::string ToString() const override {
        return "Print(" + expression->ToString() + ")";
    }

    std::string GetValue() const override { return expression->GetValue(); }
};

struct StringLiteral : ASTNode {
    std::string value;

    StringLiteral(const std::string& value) : value(value) {}

    std::string ToString() const override { return "String(" + value + ")"; }

    std::string GetValue() const override { return value; }
};

struct ArrayLiteral : ASTNode {
    std::vector<std::shared_ptr<ASTNode>> elements;

    ArrayLiteral(const std::vector<std::shared_ptr<ASTNode>>& elements)
        : elements(elements) {}

    std::string ToString() const override {
        std::string result = "Array([";
        for (size_t i = 0; i < elements.size(); ++i) {
            result += elements[i]->ToString();
            if (i + 1 < elements.size()) result += ", ";
        }
        result += "])";
        return result;
    }

    std::string GetValue() const override { return "array"; }
};

struct IndexAccess : ASTNode {
    std::shared_ptr<ASTNode> object;
    std::shared_ptr<ASTNode> index;

    IndexAccess(std::shared_ptr<ASTNode> object, std::shared_ptr<ASTNode> index)
        : object(object), index(index) {}

    std::string ToString() const override {
        return "IndexAccess(" + object->ToString() + "[" + index->ToString() +
               "])";
    }

    std::string GetValue() const override { return "index_access"; }
};

class IfStatement : public ASTNode {
   public:
    std::shared_ptr<ASTNode> condition;
    std::shared_ptr<BlockNode> then_block;
    std::shared_ptr<ASTNode> else_block;

    IfStatement(std::shared_ptr<ASTNode> cond,
                std::shared_ptr<BlockNode> then_blk,
                std::shared_ptr<ASTNode> else_blk = nullptr)
        : condition(std::move(cond)),
          then_block(std::move(then_blk)),
          else_block(std::move(else_blk)) {}

    std::string ToString() const override {
        std::string else_str =
            else_block ? ", else: " + else_block->ToString() : "";
        return "IfStatement(condition: " + condition->ToString() +
               ", then: " + then_block->ToString() + else_str + ")";
    }
    std::string GetValue() const override { return ""; }
};

class ForLoop : public ASTNode {
   public:
    std::string var_name;
    std::shared_ptr<ASTNode> start;
    std::shared_ptr<ASTNode> end;
    std::shared_ptr<ASTNode> step;
    std::shared_ptr<BlockNode> body;

    ForLoop(const std::string& var_name, std::shared_ptr<ASTNode> start,
            std::shared_ptr<ASTNode> end, std::shared_ptr<ASTNode> step,
            std::shared_ptr<BlockNode> body)
        : var_name(var_name), start(start), end(end), step(step), body(body) {}

    std::string ToString() const override {
        return "ForLoop(" + var_name + ", " + start->ToString() + ", " +
               end->ToString() + ", " + step->ToString() + ", " +
               body->ToString() + ")";
    }
    std::string GetValue() const override { return ""; }
};

class WhileLoop : public ASTNode {
   public:
    std::shared_ptr<ASTNode> condition;
    std::shared_ptr<ASTNode> body;

    WhileLoop(std::shared_ptr<ASTNode> condition, std::shared_ptr<ASTNode> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    std::string ToString() const override {
        std::string result = "WhileLoop (";
        result += "Condition: " + condition->ToString();
        result += ", Body: " + body->ToString();
        result += ")";
        return result;
    }

    std::string GetValue() const override { return "WhileLoop"; }
};

class BreakStatement : public ASTNode {
   public:
    std::string ToString() const override { return "BreakStatement()"; }

    std::string GetValue() const override { return "break"; }
};

class ContinueStatement : public ASTNode {
   public:
    std::string ToString() const override { return "ContinueStatement()"; }

    std::string GetValue() const override { return "continue"; }
};

struct NilLiteral : public ASTNode {
    std::string ToString() const override { return "nil"; }

    std::string GetValue() const override { return "nil"; }
};

struct SliceAccess : ASTNode {
    std::shared_ptr<ASTNode> object;
    std::shared_ptr<ASTNode> start; 
    std::shared_ptr<ASTNode> end;   

    SliceAccess(std::shared_ptr<ASTNode> obj, std::shared_ptr<ASTNode> s,
                std::shared_ptr<ASTNode> e)
        : object(std::move(obj)), start(std::move(s)), end(std::move(e)) {}

    std::string ToString() const override {
        return "SliceAccess(" + object->ToString() + ", " +
               (start ? start->ToString() : "nil") + ", " +
               (end ? end->ToString() : "nil") + ")";
    }

    std::string GetValue() const override { return ""; }
};
