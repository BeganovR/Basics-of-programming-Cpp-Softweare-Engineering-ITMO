#include <gtest/gtest.h>
#include "../../lib/interpreter.h"

// Тест на простое условие if
TEST(ConditionalsTest, SimpleIf) {
    std::string code = R"(
        a = 5
        if a > 3 then
            print("true")
        end if
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "true");
}

// Тест на условие if-else (if ветвь)
TEST(ConditionalsTest, IfElseTrue) {
    std::string code = R"(
        a = 5
        if a > 3 then
            print("true")
        else
            print("false")
        end if
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "true");
}

// Тест на условие if-else (else ветвь)
TEST(ConditionalsTest, IfElseFalse) {
    std::string code = R"(
        a = 2
        if a > 3 then
            print("true")
        else
            print("false")
        end if
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "false");
}

// Тест на условие с несколькими ветками
TEST(ConditionalsTest, MultipleConditions) {
    std::string code = R"(
        a = 5
        if a > 10 then
            print("a > 10")
        else if a > 5 then
            print("a > 5")
        else if a == 5 then
            print("a = 5")
        else
            print("a < 5")
        end if
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "a = 5");
}

// Тест на булевы литералы в условиях
TEST(ConditionalsTest, BooleanLiterals) {
    std::string code = R"(
        if true then
            print("true")
        else
            print("false")
        end if
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "true");
}

// Тест на логическое И
TEST(ConditionalsTest, LogicalAnd) {
    std::string code = R"(
        a = 5
        b = 10
        if a > 3 and b > 5 then
            print("true")
        else
            print("false")
        end if
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "true");
}

// Тест на логическое ИЛИ
TEST(ConditionalsTest, LogicalOr) {
    std::string code = R"(
        a = 2
        b = 10
        if a > 3 or b > 5 then
            print("true")
        else
            print("false")
        end if
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "true");
}

// Тест на логическое НЕ
TEST(ConditionalsTest, LogicalNot) {
    std::string code = R"(
        a = 2
        if not (a > 3) then
            print("true")
        else
            print("false")
        end if
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "true");
}