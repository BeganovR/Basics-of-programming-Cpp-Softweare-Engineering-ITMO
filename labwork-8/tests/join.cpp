#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <processing.h>

TEST(SimpleJoinResultTest, CreateWithValue) {
    JoinResult<int, std::string> res{42, "answer"};
    ASSERT_EQ(res.base, 42);
    ASSERT_EQ(res.joined.value(), "answer");
}