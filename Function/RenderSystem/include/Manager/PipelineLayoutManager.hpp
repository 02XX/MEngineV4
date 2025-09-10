#pragma once
#include "IPipelineLayoutManager.hpp"
#include "Manager.hpp"
#include <memory>
using namespace MEngine::Resource;
namespace MEngine::Function
{

class PipelineLayoutManager final : public Manager<PipelineLayout>, public IPipelineLayoutManager
{
  private:
    std::unordered_map<PipelineLayoutType, UUID> mTypeToID;

  public:
    PipelineLayoutManager(std::shared_ptr<VulkanContext> vulkanContext);
    ~PipelineLayoutManager() override = default;
    void Add(std::shared_ptr<PipelineLayout> asset) override;
    std::shared_ptr<PipelineLayout> GetByType(PipelineLayoutType type) const override;
    void Update(const UUID &id, std::shared_ptr<PipelineLayout> asset) override;
};
} // namespace MEngine::Function