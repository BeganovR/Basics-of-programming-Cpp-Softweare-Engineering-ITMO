#include <gtest/gtest.h>

#include "../lib/interpreter.h"

TEST(TypesTestSuite, FloatTest) {
    std::string code = R"(
        x = 3.14
        y = 2.5
        z = x * y
        print(z)
    )";

    std::string expected = "7.85";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(TypesTestSuite, StringTest) {
    std::string code = R"(
        s1 = "Hello"
        s2 = " World"
        s3 = s1 + s2
        print(s3)
    )";

    std::string expected = "Hello World";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(TypesTestSuite, ListTest) {
    std::string code = R"(
        list = [1, 2, 3, 4, 5]
        print(list[0])
        print(list[4])
    )";

    std::string expected = "15";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(TypesTestSuite, NilTest) {
    std::string code = R"(
        x = nil
        print(x == nil)
        print(x != 5)
    )";

    std::string expected = "truetrue";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(OperatorsTestSuite, ComparisonTest) {
    std::string code = R"(
        a = 5
        b = 10
        print(a == a)
        print(a == b)
        print(a != b)
        print(a < b)
        print(a > b)
        print(a <= a)
        print(a >= b)
    )";

    std::string expected = "truefalsetruetruefalsetruefalse";
    
    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionTestSuite, BasicFunctionTest) {
    std::string code = R"(
        add = function(a, b)
            return a + b
        end function
        
        print(add(5, 3))
    )";

    std::string expected = "8";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(FunctionTestSuite, FunctionAsParameterTest) {
    std::string code = R"(
        apply = function(func, x)
            return func(x)
        end function
        
        double = function(x)
            return x * 2
        end function
        
        print(apply(double, 5))
    )";

    std::string expected = "10";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ScopeTestSuite, GlobalVsLocalTest) {
    std::string code = R"(
        x = 10
        
        test = function()
            x = 5  // Local variable shadowing global x
            return x
        end function
        
        print(test())
        print(x)  // Global x unchanged
    )";

    std::string expected = "510";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(StringFunctionsTestSuite, LenTest) {
    std::string code = R"(
        s = "Hello"
        print(len(s))
    )";

    std::string expected = "5";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(StringFunctionsTestSuite, CaseConversionTest) {
    std::string code = R"(
        s = "Hello"
        print(upper(s))
        print(lower(s))
    )";

    std::string expected = "HELLOhello";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(StringFunctionsTestSuite, SplitJoinTest) {
    std::string code = R"(
        s = "a,b,c,d"
        parts = split(s, ",")
        print(len(parts))
        print(join(parts, "-"))
    )";

    std::string expected = "4a-b-c-d";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(ListFunctionsTestSuite, RangeTest) {
    std::string code = R"(
        list = range(1, 6, 1)
        print(len(list))
        print(list[0])
        print(list[4])
    )";

    std::string expected = "515";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
