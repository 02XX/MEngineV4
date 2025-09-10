#pragma once
#include "IPipelineLayoutManager.hpp"
#include "Manager.hpp"
#include <memory>

namespace MEngine::Resource
{

class PipelineLayoutManager final : public Manager<PipelineLayout>, public IPipelineLayoutManager
{
  private:
    std::unordered_map<PipelineLayoutType, UUID> mTypeToID;

  public:
    ~PipelineLayoutManager() override = default;
    void Add(std::shared_ptr<PipelineLayout> asset) override;
    std::shared_ptr<PipelineLayout> GetByType(PipelineLayoutType type) const override;
};
} // namespace MEngine::Resource