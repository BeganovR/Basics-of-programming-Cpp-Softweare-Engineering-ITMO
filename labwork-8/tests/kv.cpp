#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <processing.h>

TEST(SimpleKVTest, CreateKV) {
    KV<int, std::string> kv{1, "test"};
    ASSERT_EQ(kv.key, 1);
    ASSERT_EQ(kv.value, "test");
}