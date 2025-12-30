#pragma once
namespace MEngine::Resource
{
class Component
{
  public:
    virtual ~Component() = default;
    bool dirty = true;
};
} // namespace MEngine::Function
