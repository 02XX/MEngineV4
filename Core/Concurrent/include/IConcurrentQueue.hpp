#pragma once

#include <cstddef>
namespace MEngine::Core
{
template <typename T> class IConcurrentQueue
{
  public:
    virtual ~IConcurrentQueue() = default;
    virtual void Push(T item) = 0;
    virtual bool TryPop(T &item) = 0;
};
} // namespace MEngine::Core