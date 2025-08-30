#include <gtest/gtest.h>

#include <sstream>

#include "processing.h"

TEST(OutTest, BasicOutput) {
    std::ostringstream output;

    std::vector<int> numbers = {1, 2, 3};
    numbers | Out(output, ", ");

    EXPECT_EQ(output.str(), "1, 2, 3\n");
}

TEST(OutTest, StringOutput) {
    std::ostringstream output;

    std::vector<std::string> words = {"hello", "world"};
    words | Out(output);

    EXPECT_EQ(output.str(), "hello world\n");
}

TEST(OutTest, EmptyRange) {
    std::ostringstream output;

    std::vector<int> empty;
    empty | Out(output, "|");

    EXPECT_EQ(output.str(), "\n"); 
}