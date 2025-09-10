#pragma once
#include "IManager.hpp"
#include "Logger.hpp"
#include "UUID.hpp"
#include "VulkanContext.hpp"
#include <memory>
#include <ranges>
#include <unordered_map>

namespace MEngine::Function
{
template <std::derived_from<Asset> TAsset> class Manager : public virtual IManager<TAsset>
{

  protected:
    std::shared_ptr<VulkanContext> mVulkanContext{nullptr};

    std::unordered_map<UUID, std::shared_ptr<TAsset>> mAssets;

  public:
    Manager(std::shared_ptr<VulkanContext> vulkanContext) : mVulkanContext(vulkanContext)
    {
    }

    virtual void Add(std::shared_ptr<TAsset> asset) override
    {
        mAssets[asset->GetID()] = asset;
    }
    virtual std::shared_ptr<TAsset> Get(const UUID &id) const override
    {
        if (mAssets.contains(id))
        {
            return mAssets.at(id);
        }
        LogTrace("Asset with ID {} not found", id.ToString());
        return nullptr;
    }
    virtual std::vector<std::shared_ptr<TAsset>> GetAll() const override
    {
        return mAssets | std::views::values | std::ranges::to<std::vector<std::shared_ptr<TAsset>>>();
    }
    virtual void Remove(const UUID &id) override
    {
        if (mAssets.contains(id))
        {
            mAssets.erase(id);
        }
    }
    ~Manager() override = default;
};
} // namespace MEngine::Function