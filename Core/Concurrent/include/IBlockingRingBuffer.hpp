#pragma once
#include "IBlockingQueue.hpp"
#include <concepts>
namespace MEngine::Core
{
template <typename T> class IBlockingRingBuffer : public virtual IBlockingQueue<T>
{
};
} // namespace MEngine::Core