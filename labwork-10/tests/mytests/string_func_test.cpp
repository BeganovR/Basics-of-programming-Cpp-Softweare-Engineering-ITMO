#include <gtest/gtest.h>

#include "../../lib/interpreter.h"

TEST(StringFunctionsSuite, LowerFunction) {
    std::string code = R"(
        print(lower("HELLO"))
        print(lower("Hello World"))
        print(lower("123"))
    )";

    std::string expected = "hellohello world123";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(StringFunctionsSuite, UpperFunction) {
    std::string code = R"(
        print(upper("hello"))
        print(upper("Hello World"))
        print(upper("123"))
    )";

    std::string expected = "HELLOHELLO WORLD123";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(StringFunctionsSuite, JoinFunction) {
    std::string code = R"(
        print(join(["a", "b", "c"], ","))
        print(join(["hello", "world"], " "))
        print(join([1, 2, 3], "-"))
    )";

    std::string expected = "a,b,chello world1-2-3";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(StringFunctionsSuite, ReplaceFunction) {
    std::string code = R"(
        print(replace("hello world", "world", "ITMO"))
        print(replace("banana", "a", "o"))
        print(replace("no match", "x", "y"))
    )";

    std::string expected = "hello ITMObononono match";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(StringFunctionsSuite, StringOperations) {
    std::string code = R"(
        s1 = "hello"
        s2 = "world"
        print(s1 + " " + s2)
        print(s1 * 3)
        print("hello world" - "world")
    )";

    std::string expected = "hello worldhellohellohellohello ";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
