#pragma once
#include "Asset.hpp"
#include "Logger.hpp"
#include "SwapChainResource.hpp"

namespace MEngine::Resource
{
class SwapChain : public Asset
{
  private:
  protected:
    SwapChain() : Asset()
    {
        mName = "SwapChain";
        mResource = std::make_unique<SwapChainResource>();
    }

  public:
    SwapChain(const std::string &name) : Asset(name)
    {
        mResource = std::make_unique<SwapChainResource>();
    }
    ~SwapChain() override
    {
        LogDebug("Destroying SwapChain {}", mName);
    }
};
} // namespace MEngine::Resource