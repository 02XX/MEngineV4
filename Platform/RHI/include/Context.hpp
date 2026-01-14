#pragma once
#include "VMA.hpp"
#include <optional>
#include <vulkan/vulkan.hpp>

namespace MEngine::Platform
{

struct ContextConfig
{
    std::vector<const char *> InstanceRequiredExtensions;
    std::vector<const char *> InstanceRequiredLayers;
    std::vector<const char *> DeviceRequiredExtensions;
};
class Context
{
  public:
    ContextConfig Config;
    // properties
    vk::UniqueInstance Instance{};
    vk::PhysicalDevice PhysicalDevice{};
    struct QueueFamilyIndicates
    {
        std::optional<uint32_t> graphicsFamily{};
        std::optional<uint32_t> graphicsFamilyCount{};
        std::optional<uint32_t> transferFamily{};
        std::optional<uint32_t> transferFamilyCount{};
    } QueueFamilyIndicates;
    vk::UniqueDevice Device{};
    vk::Queue GraphicsQueue{};
    vk::Queue TransferQueue{};
    uint32_t Version = 0;
    // VMA
    VmaAllocator VmaAllocator{};

  private:
    void CreateInstance();
    void PickPhysicalDevice();
    void QueryQueueFamilyIndicates();
    void CreateLogicalDevice();
    void GetQueues();
    void CreateVMA();

  public:
    Context(const ContextConfig &config);
    ~Context();
};

} // namespace MEngine::Platform