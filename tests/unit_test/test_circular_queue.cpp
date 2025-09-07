/**
 * @file test_circular_queue.cpp
 * @brief Test Circular Queue
 * @version 0.1
 * @date 2025-05-13
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <gtest/gtest.h>
#include <COREFLOW/circular_queue.hpp>

class CircularQueueTest : public ::testing::Test
{
protected:
    static constexpr size_t kDepth = 4;
    CircularQueue<int, kDepth> queue;
};

TEST_F(CircularQueueTest, ConstructEmpty)
{
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0u);
    EXPECT_FALSE(queue.full());
}

TEST_F(CircularQueueTest, EnqueueDequeueSingle)
{
    int value = 42;
    EXPECT_TRUE(queue.enqueue(value));
    EXPECT_FALSE(queue.empty());
    EXPECT_EQ(queue.size(), 1u);

    int out = 0;
    EXPECT_TRUE(queue.dequeue(out));
    EXPECT_EQ(out, value);
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0u);
}

TEST_F(CircularQueueTest, FillAndFull)
{
    for (size_t i = 0; i < kDepth - 1; ++i)
    {
        EXPECT_TRUE(queue.enqueue(i));
    }
    EXPECT_TRUE(queue.full());
    EXPECT_EQ(queue.size(), kDepth - 1);

    // Should not enqueue when full
    EXPECT_FALSE(queue.enqueue(100));
}

TEST_F(CircularQueueTest, DequeueEmpty)
{
    int out = 0;
    EXPECT_FALSE(queue.dequeue(out));
}

TEST_F(CircularQueueTest, WrapAround)
{
    // Fill the queue
    for (size_t i = 0; i < kDepth - 1; ++i)
    {
        EXPECT_TRUE(queue.enqueue(i));
    }
    EXPECT_TRUE(queue.full());

    // Dequeue two elements
    int out = 0;
    EXPECT_TRUE(queue.dequeue(out));
    EXPECT_EQ(out, 0);
    EXPECT_TRUE(queue.dequeue(out));
    EXPECT_EQ(out, 1);

    // Enqueue two more elements (should wrap around)
    EXPECT_TRUE(queue.enqueue(100));
    EXPECT_TRUE(queue.enqueue(101));
    EXPECT_TRUE(queue.full());

    // Dequeue all and check order
    EXPECT_TRUE(queue.dequeue(out));
    EXPECT_EQ(out, 2);
    EXPECT_TRUE(queue.dequeue(out));
    EXPECT_EQ(out, 100);
    EXPECT_TRUE(queue.dequeue(out));
    EXPECT_EQ(out, 101);
    EXPECT_TRUE(queue.empty());
}

TEST_F(CircularQueueTest, SizeAfterOperations)
{
    EXPECT_EQ(queue.size(), 0u);
    EXPECT_TRUE(queue.enqueue(1));
    EXPECT_EQ(queue.size(), 1u);
    EXPECT_TRUE(queue.enqueue(2));
    EXPECT_EQ(queue.size(), 2u);

    int out = 0;
    EXPECT_TRUE(queue.dequeue(out));
    EXPECT_EQ(queue.size(), 1u);
    EXPECT_TRUE(queue.dequeue(out));
    EXPECT_EQ(queue.size(), 0u);
}

TEST_F(CircularQueueTest, EnqueueDequeueAlternating)
{
    int out = 0;
    for (int i = 0; i < 10; ++i)
    {
        EXPECT_TRUE(queue.enqueue(i));
        EXPECT_TRUE(queue.dequeue(out));
        EXPECT_EQ(out, i);
        EXPECT_TRUE(queue.empty());
    }
}