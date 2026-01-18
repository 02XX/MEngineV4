#include "ConcurrentRingBuffer.hpp"
#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <print>
#include <thread>
using namespace MEngine::Core;
TEST(ConcurrentTest, ConcurrentRingBufferTest)
{
    std::atomic<bool> isRunning{true};
    ConcurrentRingBuffer<int> ringBuffer(2);
    auto consumer = std::jthread([&ringBuffer, &isRunning]() {
        while (isRunning || !ringBuffer.Empty())
        {
            int item = ringBuffer.Pop();
            std::print("Consumed: {}\n", item);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
    auto producer = std::jthread([&ringBuffer, &isRunning]() {
        int i = 0;
        while (isRunning)
        {
            ringBuffer.Push(i);
            std::print("Produced: {}\n", i);
            ++i;
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(10));
    isRunning = false;
}