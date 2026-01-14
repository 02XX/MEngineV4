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
    bool Empty() const override;
    size_t Size() const override;
    void Push(T &&item) override;
    void Push(const T &item) override;
    bool TryPop(T &item) override;
    void WaitAndPop(T &item) override;
    void Clear() override;
};
} // namespace MEngine::Core