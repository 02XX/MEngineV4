#pragma once
#include "VMA.hpp"
#include <optional>
#include <vulkan/vulkan.hpp>

namespace MEngine::Platform
{

struct ContextConfig
{
    std::vector<const char *> InstanceRequiredExtensions{};
    std::vector<const char *> InstanceRequiredLayers{};
    std::vector<const char *> DeviceRequiredExtensions{};
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

    // Command Pool
    // Command Pool非线程安全，因此需要保证CommandPool在单线程中使用!!!
    // Command Pool的创建和销毁是耗时操作，因此在Context初始化时创建好需要的Command Pool
    vk::UniqueCommandPool GraphicsCommandPool{};
    vk::UniqueCommandPool TransferCommandPool{};

  private:
    void CreateInstance();
    void PickPhysicalDevice();
    void QueryQueueFamilyIndicates();
    void CreateLogicalDevice();
    void GetQueues();
    void CreateVMA();
    void CreateCommandPools();

  public:
    Context(const ContextConfig &config);
    ~Context();
};

} // namespace MEngine::Platform