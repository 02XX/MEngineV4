#pragma once
#include "IConcurrentRingBuffer.hpp"
#include <atomic>
#include <cstddef>
#include <readerwriterqueue/readerwritercircularbuffer.h>
#include <thread>

namespace MEngine::Core
{
template <typename T> class ConcurrentRingBuffer final : public IConcurrentRingBuffer<T>
{
  private:
    moodycamel::BlockingReaderWriterCircularBuffer<T> mRingBuffer{};
    std::atomic<size_t> mCount{0};

  public:
    ConcurrentRingBuffer(size_t capacity) : mRingBuffer(capacity)
    {
    }
    ~ConcurrentRingBuffer() override = default;
    bool Empty() const override
    {
        return Size() == 0;
    }
    size_t Size() const override
    {
        return mCount.load(std::memory_order_relaxed);
    }
    void Push(T &&item) override
    {

        mRingBuffer.wait_enqueue(std::move(item));
        mCount.fetch_add(1, std::memory_order_relaxed);
    }
    void Push(const T &item) override
    {
        mRingBuffer.wait_enqueue(item);
        mCount.fetch_add(1, std::memory_order_relaxed);
    }
    T Pop() override
    {
        T item;
        mRingBuffer.wait_dequeue(item);
        mCount.fetch_sub(1, std::memory_order_relaxed);
        return item;
    }
    void Clear() override
    {
        T temp;
        while (mRingBuffer.try_dequeue(temp))
        {
            std::this_thread::yield();
        }
    }
};
} // namespace MEngine::Core