#include "ConcurrentQueue.hpp"
#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <memory>
#include <print>
#include <thread>
using namespace MEngine::Core;

TEST(ConcurrentTest, MoveOnly)
{
    ConcurrentQueue<std::unique_ptr<int>> queue;
    auto a = std::make_unique<int>(42);
    queue.Push(std::move(a));
    std::unique_ptr<int> b;
    queue.TryPop(b);
    ASSERT_EQ(*b, 42);
}

TEST(ConcurrentTest, BlockingRingBufferTest)
{
}