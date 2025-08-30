#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <unrolled_list.h>

#include <list>
#include <vector>

TEST(UnrolledLinkedList, ClearSwapTest) {
    unrolled_list<int> list1;
    unrolled_list<int> list2;

    for (int i = 0; i < 10; ++i) {
        list1.push_back(i);
        list2.push_back(i + 10);
    }

    list1.clear();
    ASSERT_TRUE(list1.empty());

    list1.swap(list2);
    ASSERT_EQ(list1.front(), 10);
    ASSERT_EQ(list2.empty(), true);
}

TEST(UnrolledLinkedList, ComparisonOperatorsTest) {
    unrolled_list<int> list1;
    unrolled_list<int> list2;

    for (int i = 0; i < 10; ++i) {
        list1.push_back(i);
        list2.push_back(i);
    }

    ASSERT_TRUE(list1 == list2);
    list2.push_back(10);
    ASSERT_TRUE(list1 != list2);
    ASSERT_TRUE(list1 < list2);
    ASSERT_TRUE(list2 > list1);
    ASSERT_TRUE(list1 <= list2);
    ASSERT_TRUE(list2 >= list1);
}

TEST(UnrolledLinkedList, CustomAllocatorTest) {
    using CustomAllocator = std::allocator<int>;
    unrolled_list<int, 10, CustomAllocator> list;

    for (int i = 0; i < 10; ++i) {
        list.push_back(i);
    }

    ASSERT_EQ(list.size(), 10);
    ASSERT_EQ(list.front(), 0);
    ASSERT_EQ(list.back(), 9);
}

TEST(UnrolledLinkedList, NoDefaultConstructibleTest) {
    struct NoDefault {
        NoDefault(int x) : value(x) {}
        int value;
    };

    unrolled_list<NoDefault> list;
    list.push_back(NoDefault(1));
    list.push_front(NoDefault(2));

    ASSERT_EQ(list.size(), 2);
    ASSERT_EQ(list.front().value, 2);
    ASSERT_EQ(list.back().value, 1);
}

TEST(UnrolledLinkedList, SizeTest) {
    unrolled_list<int> list;
    ASSERT_EQ(list.size(), 0);

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    ASSERT_EQ(list.size(), 3);

    list.pop_back();
    ASSERT_EQ(list.size(), 2);
}

TEST(UnrolledLinkedList, EmptyTest) {
    unrolled_list<int> list;
    ASSERT_TRUE(list.empty());

    list.push_back(1);
    ASSERT_FALSE(list.empty());

    list.clear();
    ASSERT_TRUE(list.empty());
}

TEST(UnrolledLinkedList, MaxSizeTest) {
    unrolled_list<int> list;
    ASSERT_GT(list.max_size(), 0);
}

TEST(UnrolledLinkedList, SwapTest) {
    unrolled_list<int> list1;
    unrolled_list<int> list2;

    list1.push_back(1);
    list1.push_back(2);
    list1.push_back(3);

    list2.push_back(4);
    list2.push_back(5);

    list1.swap(list2);

    ASSERT_EQ(list1.size(), 2);
    ASSERT_EQ(list2.size(), 3);

    ASSERT_EQ(list1.front(), 4);
    ASSERT_EQ(list2.front(), 1);
}

TEST(UnrolledLinkedList, GetAllocatorTest) {
    unrolled_list<int> list;
    auto alloc = list.get_allocator();

    int* p = alloc.allocate(1);
    alloc.deallocate(p, 1);
}