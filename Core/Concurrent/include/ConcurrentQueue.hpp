#pragma once
#include "IConcurrentQueue.hpp"
#include <moodycamel/concurrentqueue.h>
namespace MEngine::Core
{
template <typename T> class ConcurrentQueue final : public IConcurrentQueue<T>
{
  private:
    moodycamel::ConcurrentQueue<T> mQueue;
    std::atomic<size_t> mCount{0};

  public:
    ~ConcurrentQueue() override = default;
    bool Empty() const override
    {
        return Size() == 0;
    }
    size_t Size() const override
    {
        return mCount.load(std::memory_order_relaxed) == 0;
    }
    void Push(T &&item) override
    {
        mQueue.enqueue(std::move(item));
        mCount.fetch_add(1, std::memory_order_relaxed);
    }
    void Push(const T &item) override
    {
        T copy = item;
        Push(std::move(copy));
    }
    bool TryPop(T &item) override
    {
        if (mQueue.try_dequeue(item))
        {
            mCount.fetch_sub(1, std::memory_order_relaxed);
            return true;
        }
        return false;
    }
    void WaitAndPop(T &item) override
    {
        while (!mQueue.try_dequeue(item))
        {
            std::this_thread::yield();
        }
    }
    void Clear() override
    {
        T temp;
        while (mQueue.try_dequeue(temp))
        {
            std::this_thread::yield();
        }
    }
};
} // namespace MEngine::Core