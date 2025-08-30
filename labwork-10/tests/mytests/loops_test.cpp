#include <gtest/gtest.h>
#include "../../lib/interpreter.h"

// Тест на простой while цикл
TEST(LoopsTest, SimpleWhile) {
    std::string code = R"(
        i = 0
        while i < 5
            print(i)
            i = i + 1
        end while
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "01234");
}

// Тест на цикл for
TEST(LoopsTest, SimpleFor) {
    std::string code = R"(
        for i in range(0, 5, 1)
            print(i)
        end for
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "01234");
}

// Тест на break в while
TEST(LoopsTest, BreakInWhile) {
    std::string code = R"(
        i = 0
        while i < 10
            if i == 5 then
                break
            end if
            print(i)
            i = i + 1
        end while
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "01234");
}

// Тест на continue в while
TEST(LoopsTest, ContinueInWhile) {
    std::string code = R"(
        i = 0
        while i < 5
            i = i + 1
            if i == 3 then
                continue
            end if
            print(i)
        end while
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "1245");
}

// Тест на break в for
TEST(LoopsTest, BreakInFor) {
    std::string code = R"(
        for i in range(0, 10, 1)
            if i == 5 then
                break
            end if
            print(i)
        end for
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "01234");
}

// Тест на continue в for
TEST(LoopsTest, ContinueInFor) {
    std::string code = R"(
        for i in range(1, 6, 1)
            if i == 3 then
                continue
            end if
            print(i)
        end for
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "1245");
}
