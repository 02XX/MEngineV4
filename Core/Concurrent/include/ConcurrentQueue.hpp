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
    void Push(T item) override
    {
        mQueue.enqueue(std::move(item));
    }

    bool TryPop(T &item) override
    {
        return mQueue.try_dequeue(item);
    }
};
} // namespace MEngine::Core