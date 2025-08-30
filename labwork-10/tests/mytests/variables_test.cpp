#include <gtest/gtest.h>
#include "../../lib/interpreter.h"

// Тест на простое присваивание
TEST(VariablesTest, SimpleAssignment) {
    std::string code = R"(
        a = 5
        print(a)
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "5");
}

// Тест на множественные присваивания
TEST(VariablesTest, MultipleAssignments) {
    std::string code = R"(
        a = 5
        b = a
        c = b
        print(c)
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "5");
}

// Тест на перезапись переменной
TEST(VariablesTest, VariableOverwrite) {
    std::string code = R"(
        a = 5
        print(a)
        a = 10
        print(a)
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "510");
}

TEST(VariablesTest, ColTest) {
    std::string code = R"(
        ( function(cal)
            cal(0)
        end function )(print)
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "0");
}

TEST(VariablesTest, ColTest2) {
    std::string code = R"(
        a = [1]
        b = a
        push(a, 2)
        if a == b then
            print("true")
        end if
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "true");
}

TEST(VariablesTest, ColTest3) {
    std::string code = R"(
        a = []
        b = a
        pop(a)
        if len(a) == 0 then
            print("true")
        end if
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "true");
}

TEST(VariablesTest, ColTest4) {
    std::string code = R"(
        arr = [1, "f", [3.14], function (x) x(-.0) end function, ]
        cou = -1
        for i in arr
            cou += 1
            if cou == 3 then
                i(print)
            end if
        end for
        
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "-0.0");
}

TEST(VariablesTest, ColTest5) {
    std::string code = R"(
        f = [1, 2]
        b = f[:]
        push(f, 3)
        print(len(b))
        
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "2");
}

TEST(VariablesTest, ColTest6) {
    std::string code = R"(
        f = -----1
        
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "-1");
}

TEST(VariablesTest, fact) {
    std::string code = R"(
        x =  nil
        if x == nil then
            print(nil)
        end if  

    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), "nil");
}
/* */