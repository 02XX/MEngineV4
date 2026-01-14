#include "Context.hpp"
#include <algorithm>
#include <set>
#include <vector>
#include <vulkan/vulkan_to_string.hpp>

namespace MEngine::Platform
{
Context::Context(const ContextConfig &config) : Config(config)
{
    CreateInstance();
    PickPhysicalDevice();
    QueryQueueFamilyIndicates();
    CreateLogicalDevice();
    GetQueues();
    CreateVMA();
}
Context::~Context()
{
    if (VmaAllocator)
    {
        vmaDestroyAllocator(VmaAllocator);
    }
}
void Context::CreateInstance()
{
    vk::InstanceCreateInfo instanceCreateInfo;
    vk::ApplicationInfo appInfo;

    Version = vk::enumerateInstanceVersion();
    auto variant = vk::apiVersionVariant(Version);
    auto major = vk::apiVersionMajor(Version);
    auto minor = vk::apiVersionMinor(Version);
    auto patch = vk::apiVersionPatch(Version);
    auto appVersion = vk::makeApiVersion(0, 0, 0, 1);
    appInfo.setPApplicationName("MEngine")
        .setApplicationVersion(appVersion)
        .setPEngineName({})
        .setEngineVersion({})
        .setApiVersion(Version);
    // 获取所有可用的实例扩展
    auto availableExtensions = vk::enumerateInstanceExtensionProperties();
    for (auto &ext : Config.InstanceRequiredExtensions)
    {
        if (std::ranges::find_if(availableExtensions, [&ext](const vk::ExtensionProperties &availableExt) {
                return std::string(availableExt.extensionName) == ext;
            }) == availableExtensions.end())
        {
            throw std::runtime_error("Vulkan instance extension not available");
        }
    }
    instanceCreateInfo.setFlags({})
        .setPApplicationInfo(&appInfo)
        .setPEnabledLayerNames(Config.InstanceRequiredLayers)
        .setPEnabledExtensionNames(Config.InstanceRequiredExtensions);
    Instance = vk::createInstanceUnique(instanceCreateInfo);
    if (!Instance)
    {
        throw std::runtime_error("Failed to create Vulkan instance");
    }
}

void Context::PickPhysicalDevice()
{
    auto PhysicalDevices = Instance->enumeratePhysicalDevices();
    if (PhysicalDevices.empty())
    {
        throw std::runtime_error("No physical devices found");
    }
    auto compare = [](const vk::PhysicalDevice &a, const vk::PhysicalDevice &b) {
        // 计算a的分数
        auto aScore = (a.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu) ? 1000 : 0;
        aScore += a.getProperties().limits.maxImageDimension2D;

        // 计算b的分数
        auto bScore = (b.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu) ? 1000 : 0;
        bScore += b.getProperties().limits.maxImageDimension2D;

        return aScore > bScore; // 降序排列
    };
    auto bestDevice = std::max_element(PhysicalDevices.begin(), PhysicalDevices.end(), compare);
    if (bestDevice == PhysicalDevices.end())
    {
        throw std::runtime_error("No suitable physical device found");
    }
    PhysicalDevice = *bestDevice;
}
void Context::QueryQueueFamilyIndicates()
{
    auto queueFamilyProperties = PhysicalDevice.getQueueFamilyProperties();
    for (uint32_t i = 0; i < queueFamilyProperties.size(); i++)
    {
        auto &queueFamily = queueFamilyProperties[i];
        auto queueCount = queueFamily.queueCount;
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
        {
            QueueFamilyIndicates.graphicsFamily = i;
            QueueFamilyIndicates.graphicsFamilyCount = queueCount;
        }
        if (queueFamily.queueFlags & vk::QueueFlagBits::eTransfer)
        {
            QueueFamilyIndicates.transferFamily = i;
            QueueFamilyIndicates.transferFamilyCount = queueCount;
        }
        if (QueueFamilyIndicates.graphicsFamily.has_value() && QueueFamilyIndicates.transferFamily.has_value())
        {
            break;
        }
    }
}
void Context::CreateLogicalDevice()
{
    vk::DeviceCreateInfo deviceCreateInfo;
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    std::set<uint32_t> uniqueQueueFamilies = {QueueFamilyIndicates.graphicsFamily.value()};

    if (QueueFamilyIndicates.transferFamily.has_value() &&
        QueueFamilyIndicates.transferFamily.value() != QueueFamilyIndicates.graphicsFamily.value())
    {
        uniqueQueueFamilies.insert(QueueFamilyIndicates.transferFamily.value());
    }
    const float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo.setQueueFamilyIndex(queueFamily).setQueueCount(1).setPQueuePriorities(&queuePriority);
        queueCreateInfos.push_back(queueCreateInfo);
    }
    // extension
    auto availableExtensions = PhysicalDevice.enumerateDeviceExtensionProperties();
    std::vector<const char *> extensions = {"VK_KHR_maintenance1", "VK_EXT_host_image_copy",
                                            "VK_KHR_dynamic_rendering"};
    vk::PhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.setIndependentBlend(vk::True);
    vk::PhysicalDeviceHostImageCopyFeaturesEXT hostImageCopyFeatures{};
    vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{};
    dynamicRenderingFeatures.setDynamicRendering(vk::True);
    hostImageCopyFeatures.setHostImageCopy(vk::True).setPNext(&dynamicRenderingFeatures);

    Config.DeviceRequiredExtensions.insert_range(Config.DeviceRequiredExtensions.end(), extensions);
    deviceCreateInfo.setQueueCreateInfos(queueCreateInfos)
        .setPEnabledExtensionNames(Config.DeviceRequiredExtensions)
        .setPEnabledFeatures(&deviceFeatures)
        .setPNext(&hostImageCopyFeatures);
    Device = PhysicalDevice.createDeviceUnique(deviceCreateInfo);
    if (!Device)
    {
        throw std::runtime_error("Failed to create Vulkan logical device");
    }
}
void Context::GetQueues()
{
    if (QueueFamilyIndicates.graphicsFamily.has_value())
    {
        auto graphicQueueIndex = QueueFamilyIndicates.graphicsFamily.value();
        GraphicsQueue = Device->getQueue(graphicQueueIndex, 0);
        if (!GraphicsQueue)
        {
            throw std::runtime_error("Failed to get graphics queue from Vulkan device");
        }
    }
    if (QueueFamilyIndicates.transferFamily.has_value())
    {
        auto transferQueueIndex = QueueFamilyIndicates.transferFamily.value();
        TransferQueue = Device->getQueue(transferQueueIndex, 0);
        if (!TransferQueue)
        {
            throw std::runtime_error("Failed to get transfer queue from Vulkan device");
        }
    }
}
void Context::CreateVMA()
{
    VmaAllocatorCreateInfo allocatorCreateInfo{};
    allocatorCreateInfo.device = Device.get();
    allocatorCreateInfo.physicalDevice = PhysicalDevice;
    allocatorCreateInfo.instance = Instance.get();
    allocatorCreateInfo.flags =
        VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT | VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
    auto variant = vk::apiVersionVariant(Version);
    auto major = vk::apiVersionMajor(Version);
    auto minor = vk::apiVersionMinor(Version);
    auto patch = vk::apiVersionPatch(Version);
    allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    // allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;
    vmaCreateAllocator(&allocatorCreateInfo, &VmaAllocator);
}
} // namespace MEngine::Platform