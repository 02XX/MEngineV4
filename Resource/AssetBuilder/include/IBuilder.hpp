#pragma once
#include <memory>
#include <string>
namespace MEngine::Core
{
template <class TAsset> class IBuilder
{
  public:
    virtual ~IBuilder() = default;
    virtual void SetName() = 0;
    virtual void Reset() = 0;
    virtual std::unique_ptr<TAsset> Build() = 0;
};
} // namespace MEngine::Core