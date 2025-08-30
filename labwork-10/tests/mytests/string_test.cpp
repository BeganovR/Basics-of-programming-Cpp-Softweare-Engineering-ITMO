#include <gtest/gtest.h>

#include "../../lib/interpreter.h"

TEST(StringOperationsSuite, Concatenation) {
    std::string code = R"(
        s1 = "Hello, "
        s2 = "world!"
        result = s1 + s2
        print(result)
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "Hello, world!");
}

TEST(StringOperationsSuite, Subtraction) {
    std::string code = R"(
        full = "Hello, world!"
        suffix = "world!"
        result = full - suffix
        print(result)
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "Hello, ");
}

TEST(StringOperationsSuite, Multiplication) {
    std::string code = R"(
        s = "abc"
        result = s * 3
        print(result)
    )";

    std::string expected = "abcabcabc";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(StringOperationsSuite, Indexing) {
    std::string code = R"(
        s = "Hello"
        a = s[0]     // Первый символ
        b = s[4]     // Последний символ
        c = s[-1]    // Отрицательный индекс
        print(a)
        print(b)
        print(c)
    )";

    std::string expected = "Hoo";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
