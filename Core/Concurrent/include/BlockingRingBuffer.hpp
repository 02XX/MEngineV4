#pragma once
#include "IBlockingRingBuffer.hpp"
#include <atomic>
#include <cstddef>
#include <readerwriterqueue/readerwritercircularbuffer.h>
#include <thread>

namespace MEngine::Core
{
template <typename T> class BlockingRingBuffer final : public IBlockingRingBuffer<T>
{
  private:
    moodycamel::BlockingReaderWriterCircularBuffer<T> mRingBuffer{};

  public:
    BlockingRingBuffer(size_t capacity) : mRingBuffer(capacity)
    {
    }
    void Push(T item) override
    {
        mRingBuffer.wait_enqueue(std::move(item));
    }
    T Pop() override
    {
        T item;
        mRingBuffer.wait_dequeue(item);
        return item;
    }
    bool TryPush(T item) override
    {
        return mRingBuffer.try_enqueue(std::move(item));
    }
    bool TryPop(T &item) override
    {
        return mRingBuffer.try_dequeue(item);
    }
};
} // namespace MEngine::Core