#pragma once
#include "IConcurrentQueue.hpp"
#include <moodycamel/concurrentqueue.h>
namespace MEngine::Core
{
template <typename T> class ConcurrentQueue final : public IConcurrentQueue<T>
{
  private:
    moodycamel::ConcurrentQueue<T> mQueue;

  public:
    ~ConcurrentQueue() override = default;
    bool Empty() const override
    {
        return Size() == 0;
    }
    size_t Size() const override
    {
        return mQueue.size_approx();
    }
    void Push(T &&item) override
    {
        mQueue.enqueue(std::move(item));
    }
    void Push(const T &item) override
    {
        mQueue.enqueue(item);
    }
    bool TryPop(T &item) override
    {
        return mQueue.try_dequeue(item);
    }
    void WaitAndPop(T &item) override
    {
        while (!mQueue.try_dequeue(item))
        {
        }
    }
    void Clear() override
    {
        T temp;
        while (mQueue.try_dequeue(temp))
        {
        }
    }
};
} // namespace MEngine::Core