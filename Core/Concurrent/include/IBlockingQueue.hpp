#pragma once

namespace MEngine::Core
{
template <typename T> class IBlockingQueue
{
  public:
    virtual ~IBlockingQueue() = default;
    virtual void Push(T item) = 0;
    virtual T Pop() = 0;
    virtual bool TryPush(T item) = 0;
    virtual bool TryPop(T &item) = 0;
};
} // namespace MEngine::Core