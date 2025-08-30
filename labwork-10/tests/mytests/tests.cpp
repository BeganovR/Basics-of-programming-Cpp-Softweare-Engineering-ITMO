#include <gtest/gtest.h>

#include "../../lib/interpreter.h"

TEST(MaxFunctionSuite, BasicPositiveNumbers) {
    std::string code = R"(
        max_func = function(arr)
            if len(arr) == 0 then
                return nil
            end if

            m = arr[0]

            for i in arr
                if i > m then m = i end if
            end for

            return m
        end function

        print(max_func([10, 5, 7, 3]))
    )";

    std::string expected = "10";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(MaxFunctionSuite, NegativeNumbers) {
    std::string code = R"(
        max_func = function(arr)
            if len(arr) == 0 then
                return nil
            end if

            m = arr[0]

            for i in arr
                if i > m then m = i end if
            end for

            return m
        end function

        print(max_func([-10, -5, -7, -3]))
    )";

    std::string expected = "-3";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(MaxFunctionSuite, MixedNumbers) {
    std::string code = R"(
        max_func = function(arr)
            if len(arr) == 0 then
                return nil
            end if

            m = arr[0]

            for i in arr
                if i > m then m = i end if
            end for

            return m
        end function

        print(max_func([10, -1, 0, 2, 2025, 239]))
    )";

    std::string expected = "2025";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(MaxFunctionSuite, SingleElement) {
    std::string code = R"(
        max_func = function(arr)
            if len(arr) == 0 then
                return nil
            end if

            m = arr[0]

            for i in arr
                if i > m then m = i end if
            end for

            return m
        end function

        print(max_func([42]))
    )";

    std::string expected = "42";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(MaxFunctionSuite, EmptyArray) {
    std::string code = R"(
        max_func = function(arr)
            if len(arr) == 0 then
                return nil
            end if

            m = arr[0]

            for i in arr
                if i > m then m = i end if
            end for

            return m
        end function

        result = max_func([])
        if result == nil then
            print("nil")
        else
            print(result)
        end if
    )";

    std::string expected = "nil";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(MaxFunctionSuite, DuplicateMaxValues) {
    std::string code = R"(
        max_func = function(arr)
            if len(arr) == 0 then
                return nil
            end if

            m = arr[0]

            for i in arr
                if i > m then m = i end if
            end for

            return m
        end function

        print(max_func([5, 10, 10, 3, 10]))
    )";

    std::string expected = "10";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(MaxFunctionSuite, FloatingPointNumbers) {
    std::string code = R"(
        max_func = function(arr)
            if len(arr) == 0 then
                return nil
            end if

            m = arr[0]

            for i in arr
                if i > m then m = i end if
            end for

            return m
        end function

        print(max_func([1.5, 2.7, 0.5, 2.2]))
    )";

    std::string expected = "2.7";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(MaxFunctionSuite, MaxAtDifferentPositions) {
    std::string code = R"(
        max_func = function(arr)
            if len(arr) == 0 then
                return nil
            end if

            m = arr[0]

            for i in arr
                if i > m then m = i end if
            end for

            return m
        end function

        // Max at beginning
        print(max_func([100, 2, 3, 4]))
        // Max in middle
        print(max_func([1, 2, 300, 4]))
        // Max at end
        print(max_func([1, 2, 3, 400]))
    )";

    std::string expected = "1003004400";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(MaxFunctionSuite, VeryLargeNumbers) {
    std::string code = R"(
        max_func = function(arr)
            if len(arr) == 0 then
                return nil
            end if

            m = arr[0]

            for i in arr
                if i > m then m = i end if
            end for

            return m
        end function

        print(max_func([1000000, 9999999, 100]))
    )";

    std::string expected = "9999999";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(MaxFunctionSuite, SmallNumbersNearZero) {
    std::string code = R"(
        max_func = function(arr)
            if len(arr) == 0 then
                return nil
            end if

            m = arr[0]

            for i in arr
                if i > m then m = i end if
            end for

            return m
        end function

        print(max_func([0.001, 0.0001, 0.01, 0.0005]))
    )";

    std::string expected = "0.01";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(MaxFunctionSuite, ScientificNotation) {
    std::string code = R"(
        max_func = function(arr)
            if len(arr) == 0 then
                return nil
            end if

            m = arr[0]

            for i in arr
                if i > m then m = i end if
            end for

            return m
        end function

        print(max_func([1.2e3, 1.2e2, 1.2e4]))
    )";

    std::string expected = "12000";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(MaxFunctionSuite, BooleanValues) {
    std::string code = R"(
        max_func = function(arr)
            if len(arr) == 0 then
                return nil
            end if

            m = arr[0]

            for i in arr
                if i > m then m = i end if
            end for

            return m
        end function

        print(max_func([true, false]))
    )";

    std::string expected = "1";  // true is represented as 1

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(MaxFunctionSuite, MaxWithArrayManipulation) {
    std::string code = R"(
        max_func = function(arr)
            if len(arr) == 0 then
                return nil
            end if

            m = arr[0]

            for i in arr
                if i > m then m = i end if
            end for

            return m
        end function

        array = [5, 10, 15]
        push(array, 20)
        print(max_func(array))
        
        pop(array)
        print(max_func(array))
    )";

    std::string expected = "2015";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}

TEST(MaxFunctionSuite, MaxWithRangeGeneration) {
    std::string code = R"(
        max_func = function(arr)
            if len(arr) == 0 then
                return nil
            end if

            m = arr[0]

            for i in arr
                if i > m then m = i end if
            end for

            return m
        end function

        print(max_func(range(1, 10)))
        print(max_func(range(5, 15)))
    )";

    std::string expected = "914";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}