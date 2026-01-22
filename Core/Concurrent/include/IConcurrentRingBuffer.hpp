#pragma once
#include "IConcurrentQueue.hpp"
#include <concepts>
namespace MEngine::Core
{
template <typename T> class IConcurrentRingBuffer : public virtual IConcurrentQueue<T>
{
};
} // namespace MEngine::Core