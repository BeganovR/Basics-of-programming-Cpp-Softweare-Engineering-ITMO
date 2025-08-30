#include <gtest/gtest.h>

#include <sstream>

#include "../../lib/interpreter.h"

TEST(HigherOrderFuncSuite, MapTest) {
    std::string code = R"(
        // Функция для удвоения числа
        double = function(x)
            return x * 2
        end function
        
        numbers = [1, 2, 3, 4, 5]
        doubled = map(double, numbers)
        print(doubled)
    )";

    std::string expected = "[2, 4, 6, 8, 10]";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_TRUE(Interpret(input, output));
    ASSERT_EQ(output.str(), expected);
}
