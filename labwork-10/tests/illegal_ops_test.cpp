#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "../lib/interpreter.h"

std::string kUnreachable = "239";

TEST(IllegalOperationsSuite, TypeMixing) {
    std::vector<std::string> values = {
        "123", "\"string\"", "[1, 2, 3]", "function() end function", "nil",
    };

    for (int a = 0; a < values.size(); ++a) {
        for (int b = a + 1; b < values.size(); ++b) {
            // In the TypeMixing test, replace the current stream handling with:

            std::string code = "a = " + values[a] + "\n" + "b = " + values[b] +
                               "\n" + "c = a + b\n" +
                               "print(239) // unreachable\n";

            std::istringstream input(code);
            std::ostringstream output;

            ASSERT_FALSE(Interpret(input, output));
            ASSERT_FALSE(output.str().ends_with(kUnreachable));
        }
    }
}

TEST(IllegalOperationsSuite, ArgumentCountMismatch) {
    std::string code = R"(
        func = function(value) return 1 end function

        func(1, 2)

        print(239) // unreachable
    )";

    std::istringstream input(code);
    std::ostringstream output;

    ASSERT_FALSE(Interpret(input, output));
    ASSERT_FALSE(output.str().ends_with(kUnreachable));
}