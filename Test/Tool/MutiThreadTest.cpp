#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <gtest/gtest.h>
#include <iostream>
#include <mutex>
#include <thread>

constexpr int MAX_FRAMES = 2;

std::atomic<bool> isRunning{false};

std::array<std::mutex, MAX_FRAMES> frameMutexes;
std::array<std::condition_variable, MAX_FRAMES> frameProduceCVs;
std::array<std::condition_variable, MAX_FRAMES> frameConsumeCVs;
std::array<bool, MAX_FRAMES> hasFrameData = {false, false};
std::array<int, MAX_FRAMES> frameData = {0, 0};

TEST(MultiThreadedRenderingTest, DoubleBufferingSafe)
{
    isRunning = true;

    std::jthread consumerThread([]() {
        int consumeIndex = 0;
        while (isRunning)
        {
            std::unique_lock<std::mutex> lck(frameMutexes[consumeIndex]);
            frameConsumeCVs[consumeIndex].wait(lck, [&] { return hasFrameData[consumeIndex] || !isRunning; });

            if (!isRunning)
                break;

            // 消费帧数据
            int data = frameData[consumeIndex];
            std::cout << "Consumed frame: " << data << " on thread " << std::this_thread::get_id() << std::endl;

            hasFrameData[consumeIndex] = false;
            frameProduceCVs[consumeIndex].notify_one();

            consumeIndex = (consumeIndex + 1) % MAX_FRAMES;
        }
    });

    int produceIndex = 0;
    while (isRunning)
    {
        std::unique_lock<std::mutex> lck(frameMutexes[produceIndex]);
        frameProduceCVs[produceIndex].wait(lck, [&] { return !hasFrameData[produceIndex] || !isRunning; });

        if (!isRunning)
            break;

        // 生产帧数据
        frameData[produceIndex] =
            static_cast<int>(std::chrono::steady_clock::now().time_since_epoch().count() % 100000);
        hasFrameData[produceIndex] = true;

        frameConsumeCVs[produceIndex].notify_one();

        produceIndex = (produceIndex + 1) % MAX_FRAMES;
    }

    // 停止线程
    isRunning = false;
    for (int i = 0; i < MAX_FRAMES; ++i)
    {
        frameConsumeCVs[i].notify_all();
        frameProduceCVs[i].notify_all();
    }
}
