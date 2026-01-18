#pragma once
#include "IConcurrentQueue.hpp"
namespace MEngine::Core
{
template <typename T> class IConcurrentRingBuffer
{
  public:
    virtual ~IConcurrentRingBuffer() = default;
    virtual bool Empty() const = 0;
    virtual size_t Size() const = 0;
    virtual void Push(T &&item) = 0;
    virtual void Push(const T &item) = 0;
    virtual T Pop() = 0;
    virtual void Clear() = 0;
};
} // namespace MEngine::Core