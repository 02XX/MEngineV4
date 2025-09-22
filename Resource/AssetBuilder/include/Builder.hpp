#pragma once
#include "AssetManager.hpp"
#include "IBuilder.hpp"
#include <memory>

namespace MEngine::Resource
{
template <class TAsset> class Builder : public virtual IBuilder<TAsset>
{
  protected:
    std::string mName;

  public:
    Builder()
    {
    }
    void SetName() override
    {
        mName = "Unnamed";
    }
    virtual ~Builder() = default;
};
} // namespace MEngine::Resource