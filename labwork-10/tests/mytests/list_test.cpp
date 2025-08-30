#include <gtest/gtest.h>

#include "../../lib/interpreter.h"

TEST(ListOperationsSuite, Creation) {
    std::string code = R"(
        a = [1, 2, 3, 4]
        print(a)
    )";

    std::string expected = "[1, 2, 3, 4]";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ListOperationsSuite, Concatenation) {
    std::string code = R"(
        a = [1, 2]
        b = [3, 4]
        c = a + b
        print(c)
    )";

    std::string expected = "[1, 2, 3, 4]";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ListOperationsSuite, Multiplication) {
    std::string code = R"(
        a = [1, 2]
        b = a * 3
        print(b)
    )";

    std::string expected = "[1, 2, 1, 2, 1, 2]";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ListOperationsSuite, Indexing) {
    std::string code = R"(
        a = [10, 20, 30, 40, 50]
        first = a[0]     // Первый элемент
        last = a[4]      // Последний элемент
        neg = a[-1]      // Отрицательный индекс
        print(first)
        print(last)
        print(neg)
    )";

    std::string expected = "105050";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ListOperationsSuite, Functions) {
    std::string code = R"(
        // range(start, end, step)
        a = range(1, 5, 1)
        print(a)

        // push
        b = push(a, 99)
        print(b)

        // remove
        c = remove(b, 2)  // Удаляет 3
        print(c)

        // sort
        d = [3, 1, 4, 2]
        e = sort(d)
        print(e)
    )";

    std::string expected =
        "[1, 2, 3, 4][1, 2, 3, 4, 99][1, 2, 4, 99][1, 2, 3, 4]";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}