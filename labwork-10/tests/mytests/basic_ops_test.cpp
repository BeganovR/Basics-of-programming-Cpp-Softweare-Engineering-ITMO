#include <gtest/gtest.h>
#include "../../lib/interpreter.h"

// Тест на сложение
TEST(BasicOperationsTest, Addition) {
    std::string code = R"(
        a = 5
        b = 3
        c = a + b
        print(c)
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "8");
}

// Тест на вычитание
TEST(BasicOperationsTest, Subtraction) {
    std::string code = R"(
        a = 5
        b = 3
        c = a - b
        print(c)
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "2");
}

// Тест на умножение
TEST(BasicOperationsTest, Multiplication) {
    std::string code = R"(
        a = 5
        b = 3
        c = a * b
        print(c)
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "15");
}

// Тест на деление
TEST(BasicOperationsTest, Division) {
    std::string code = R"(
        a = 10
        b = 2
        c = a / b
        print(c)
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "5");
}

// Тест на остаток от деления
TEST(BasicOperationsTest, Modulo) {
    std::string code = R"(
        a = 10
        b = 3
        c = a % b
        print(c)
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "1");
}

// Тест на возведение в степень
TEST(BasicOperationsTest, Power) {
    std::string code = R"(
        a = 2
        b = 3
        c = a ^ b
        print(c)
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "8");
}

// Тест на сложное выражение
TEST(BasicOperationsTest, ComplexExpression) {
    std::string code = R"(
        a = 5
        b = 3
        c = 2
        d = a + b * c
        print(d)
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "11");
}