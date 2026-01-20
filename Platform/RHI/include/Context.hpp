#pragma once
#include "VMA.hpp"
#include <cstdint>
#include <optional>
#include <queue>
#include <unordered_map>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

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

    // TODO: 专门的DescriptorSetManager来管理描述符集的分配和释放
    constexpr static uint32_t MAX_DESCRIPTOR_COUNT = 1024;
    struct DefaultDescriptorSetLayoutType
    {
        static constexpr const char *TextureBindless = "TextureBindless";
        static constexpr const char *GlobalStorage = "GlobalStorage";
        static constexpr const char *PBR = "PBR";
        static constexpr const char *Phong = "Phong";
    };
    static inline const std::unordered_map<std::string, std::vector<vk::DescriptorSetLayoutBinding>>
        DefaultDescriptorSetLayoutBindings{
            {DefaultDescriptorSetLayoutType::TextureBindless,
             {
                 // binding 0: Texture2D array
                 vk::DescriptorSetLayoutBinding()
                     .setBinding(0)
                     .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                     .setDescriptorCount(MAX_DESCRIPTOR_COUNT) // Texture数组
                     .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
                     .setPImmutableSamplers(nullptr),
             }},
            {DefaultDescriptorSetLayoutType::GlobalStorage,
             {
                 // binding 0: VP
                 vk::DescriptorSetLayoutBinding()
                     .setBinding(0)
                     .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                     .setDescriptorCount(1)
                     .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
                     .setPImmutableSamplers(nullptr),
                 // binding 1: Lights Storage Buffer
                 vk::DescriptorSetLayoutBinding()
                     .setBinding(1)
                     .setDescriptorType(vk::DescriptorType::eStorageBuffer)
                     .setDescriptorCount(1)
                     .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
                     .setPImmutableSamplers(nullptr),
             }},
            {DefaultDescriptorSetLayoutType::PBR,
             {
                 // binding 0
                 vk::DescriptorSetLayoutBinding()
                     .setBinding(0)
                     .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                     .setDescriptorCount(1)
                     .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
                     .setPImmutableSamplers(nullptr),
             }},
            {DefaultDescriptorSetLayoutType::Phong,
             {
                 // binding 0
                 vk::DescriptorSetLayoutBinding()
                     .setBinding(0)
                     .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                     .setDescriptorCount(1)
                     .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
                     .setPImmutableSamplers(nullptr),
             }}

        };
    std::unordered_map<std::string, vk::UniqueDescriptorSetLayout> DefaultDescriptorSetLayouts{};
    std::queue<uint32_t> FreeDescriptorIndices{};
    uint32_t NextDescriptorIndex = 0;
    vk::UniqueDescriptorPool DescriptorPool{};
    vk::UniqueDescriptorSet TextureBindlessDescriptorSet{};

  private:
    void CreateInstance();
    void PickPhysicalDevice();
    void QueryQueueFamilyIndicates();
    void CreateLogicalDevice();
    void GetQueues();
    void CreateVMA();
    void CreateCommandPools();
    void CreateDescriptorPool();
    void CreateDescriptorSetLayouts();
    void CreateTextureBindlessDescriptorSet();

  public:
    Context(const ContextConfig &config);
    ~Context();
    uint32_t AllocateDescriptorIndex();
    void FreeDescriptorIndex(uint32_t index);
};

} // namespace MEngine::Platform