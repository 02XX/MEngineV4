#include "ConcurrentQueue.hpp"

namespace MEngine::Core
{
template <typename T> bool ConcurrentQueue<T>::Empty() const
{
    return mQueue.is_empty();
}
template <typename T> size_t ConcurrentQueue<T>::Size() const
{
    return mQueue.size_approx();
}
template <typename T> void ConcurrentQueue<T>::Push(T &&item)
{
    mQueue.enqueue(std::move(item));
}
template <typename T> void ConcurrentQueue<T>::Push(const T &item)
{
    mQueue.enqueue(item);
}
template <typename T> bool ConcurrentQueue<T>::TryPop(T &item)
{
    return mQueue.try_dequeue(item);
}
template <typename T> void ConcurrentQueue<T>::WaitAndPop(T &item)
{
    while (!mQueue.try_dequeue(item))
    {
    }
}
template <typename T> void ConcurrentQueue<T>::Clear()
{
    T temp;
    while (mQueue.try_dequeue(temp))
    {
    }
}
} // namespace MEngine::Core