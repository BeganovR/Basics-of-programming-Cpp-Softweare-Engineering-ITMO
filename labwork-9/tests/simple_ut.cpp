#include <gtest/gtest.h>

#include "../lib/any.h"
#include "../lib/scheduler.h"

int Sum(int a, int b) { return a + b; }

TEST(SchedulerTest, BasicDependencies) {
    TTaskScheduler sc;
    auto id1 = sc.add(Sum, 1, 2);
    auto id2 = sc.add(Sum, 3, sc.getFutureResult<int>(id1));

    auto result_1 = sc.getResult<int>(id1);
    auto result_2 = sc.getResult<int>(id2);

    EXPECT_EQ(result_1, 3);
    EXPECT_EQ(result_2, 6);
}

TEST(SchedulerTest, MultipleDependencies) {
    TTaskScheduler sc;

    auto id1 = sc.add([](int a, int b) { return a * b; }, 10, 10);
    auto id2 = sc.add([](int a, int b) { return a + b; }, 10,
                      sc.getFutureResult<int>(id1));
    auto id3 = sc.add([](int a, int b) { return a + b; }, 10,
                      sc.getFutureResult<int>(id2));

    auto result_1 = sc.getResult<int>(id1);
    auto result_2 = sc.getResult<int>(id2);
    auto result_3 = sc.getResult<int>(id3);
    EXPECT_EQ(result_1, 100);
    EXPECT_EQ(result_2, 110);
    EXPECT_EQ(result_3, 120);
}

TEST(SchedulerTest, OneArgument) {
    TTaskScheduler sc;

    auto id1 = sc.add([](int a) { return a * a; }, 10);
    auto id2 =
        sc.add([](int a) { return a + 1; }, sc.getFutureResult<int>(id1));
    auto id3 =
        sc.add([](int a) { return a - 1; }, sc.getFutureResult<int>(id2));

    auto result_1 = sc.getResult<int>(id1);
    auto result_2 = sc.getResult<int>(id2);
    auto result_3 = sc.getResult<int>(id3);

    EXPECT_EQ(result_1, 100);
    EXPECT_EQ(result_2, 101);
    EXPECT_EQ(result_3, 100);
}

TEST(SchedulerTest, BasicExecuteAll) {
    TTaskScheduler sc;

    auto id1 = sc.add(Sum, 1, 2);
    auto id2 = sc.add(Sum, 3, sc.getFutureResult<int>(id1));

    sc.executeAll();

    auto result_1 = sc.getResult<int>(id1);
    auto result_2 = sc.getResult<int>(id2);

    EXPECT_EQ(result_1, 3);
    EXPECT_EQ(result_2, 6);
}

TEST(SchedulerTest, VectorTaskOneArgumentTest) {
    TTaskScheduler sc;
    auto id = sc.add([](int a) { return std::vector<int>(5, a); }, 10);
    ASSERT_EQ(sc.getResult<std::vector<int>>(id),
              std::vector<int>({10, 10, 10, 10, 10}));
}

TEST(SchedulerTest, ExpectSituatios) {
    TTaskScheduler sc;
    auto id = sc.add([](int a) { return std::vector<int>(5, a); }, 10);
    ASSERT_THROW(sc.getResult<std::vector<float>>(id), Any::BadAnyCast);
}

TEST(SchedulerTest, DefaultTaskOneArgumentTest) {
    TTaskScheduler sc;
    auto id = sc.add([](std::string a) { return a.size(); }, "hello!");
    ASSERT_EQ(sc.getResult<size_t>(id), 6);
}

TEST(SchedulerTest, BasicNoArgTest) {
    TTaskScheduler sc;
    auto id = sc.add([]() { return 1; });
    ASSERT_EQ(sc.getResult<int>(id), 1);
}

TEST(SchedulerTest, LongTest) {
    TTaskScheduler sc;
    float a = 1;
    float b = -2;
    float c = 0;
    auto id1 = sc.add([](float a, float c) { return (-4 * a * c); }, a, c);
    auto id2 = sc.add([](float b, float v) { return b * b + v; }, b,
                      sc.getFutureResult<float>(id1));
    auto id3 = sc.add([](float b, float d) { return -b + std::sqrt(d); }, b,
                      sc.getFutureResult<float>(id2));
    auto id4 = sc.add([](float b, float d) { return -b - std::sqrt(d); }, b,
                      sc.getFutureResult<float>(id2));
    auto id5 = sc.add([](float a, float v) { return v / (2 * a); }, a,
                      sc.getFutureResult<float>(id3));
    auto id6 = sc.add([](float a, float v) { return v / (2 * a); }, a,
                      sc.getFutureResult<float>(id4));
    sc.executeAll();

    ASSERT_EQ(sc.getResult<float>(id6), 0);
}

TEST(SchedulerTest, MultiTest) {
    TTaskScheduler sc;
    auto id1 = sc.add([](int a, int c) { return a + c; }, 3, 5);
    auto id2 =
        sc.add([](int a) { return a * 10; }, sc.getFutureResult<int>(id1));
    auto id3 = sc.add([]() { return 4 + 6; });
    auto id4 =
        sc.add([](int a, int b) { return a + b; }, sc.getFutureResult<int>(id2),
               sc.getFutureResult<int>(id3));
    sc.executeAll();
    ASSERT_EQ(sc.getResult<int>(id4), 90);
}

TEST(MyAnyTestSuite, DefaultIntTest) {
    Any any;
    any = 5;
    ASSERT_EQ(any.Cast<int>(), 5);
}

TEST(MyAnyTestSuite, DefaultStringTest) {
    Any any;
    std::string s = "pupupu";
    any = s;
    ASSERT_EQ(any.Cast<std::string>(), "pupupu");
}

TEST(MyAnyTestSuite, CopyTest) {
    Any any_1;
    any_1 = 5;
    Any any_2 = any_1;
    ASSERT_EQ(any_2.Cast<int>(), 5);
}

TEST(MyAnyTestSuite, TypeChangeTest) {
    std::stringstream output;
    Any any;
    int a = 5;
    any = a;
    output << any.Cast<int>();
    float b = 7.0;
    any = b;
    output << any.Cast<float>();
    ASSERT_EQ(output.str(), "57");
}

TEST(MyAnyTestSuite, TypeChange2Test) {
    std::stringstream output;
    Any any;
    int a = 28;
    any = a;
    output << any.Cast<int>();
    std::string b = "hello!";
    any = b;
    output << any.Cast<std::string>();
    ASSERT_EQ(output.str(), "28hello!");
}
