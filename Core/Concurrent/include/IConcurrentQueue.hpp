#pragma once

namespace MEngine::Core
{
template <typename T> class IConcurrentQueue
{
  public:
    virtual ~IConcurrentQueue() = default;
    virtual bool Empty() const = 0;
    virtual size_t Size() const = 0;
    virtual void Push(T &&item) = 0;
    virtual void Push(const T &item) = 0;
    virtual bool TryPop(T &item) = 0;
    virtual void WaitAndPop(T &item) = 0;
    virtual void Clear() = 0;
};
} // namespace MEngine::Core