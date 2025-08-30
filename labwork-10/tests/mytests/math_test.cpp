#include <gtest/gtest.h>

#include "../../lib/interpreter.h"

TEST(MathFunctionsSuite, AbsFunction) {
    std::string code = R"(
        print(abs(5))
        print(abs(-5))
        print(abs(0))
    )";

    std::string expected = "550";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(MathFunctionsSuite, CeilAndFloorFunctions) {
    std::string code = R"(
        print(ceil(5.3))
        print(ceil(-5.8))
        print(floor(5.3))
        print(floor(-5.8))
    )";

    std::string expected = "6-55-6";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(MathFunctionsSuite, RoundFunction) {
    std::string code = R"(
        print(round(5.3))
        print(round(5.7))
        print(round(-5.8))
    )";

    std::string expected = "56-6";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(MathFunctionsSuite, SqrtFunction) {
    std::string code = R"(
        print(sqrt(4))
        print(sqrt(9))
        print(sqrt(0))
    )";

    std::string expected = "230";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(MathFunctionsSuite, RndFunction) {
    std::string code = R"(
        n = 10
        r = rnd(n)
        print(r)
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_FALSE(output.str().empty());
}

TEST(MathFunctionsSuite, ToStringFunction) {
    std::string code = R"(
        print(to_string(123))
        print(to_string(3.14))
        print(to_string(0))
    )";

    std::string expected = "123.0000003.1400000.000000";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}