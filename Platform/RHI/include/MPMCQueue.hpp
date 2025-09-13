#pragma once
#include <moodycamel/concurrentqueue.h>
namespace MEngine::Platform
{
template <typename T> class MPMCQueue
{
  private:
    moodycamel::ConcurrentQueue<T> mQueue;

  public:
    void Produce(const T &item)
    {
        mQueue.enqueue(item);
    }
    T Consume()
    {
        T item;
        if (mQueue.try_dequeue(item))
        {
            return item;
        }
        return T();
    }
    bool IsEmpty() const
    {
        return mQueue.size_approx() == 0;
    }
    size_t Size() const
    {
        return mQueue.size_approx();
    }
};
} // namespace MEngine::Platform