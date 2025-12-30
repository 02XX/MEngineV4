#include "RHISemaphore.hpp"
#include "RHIContext.hpp"

namespace MEngine::Platform
{
RHISemaphore::RHISemaphore() : RHIResource()
{
    auto &rhiContext = RHIContext::Instance();
    auto &device = rhiContext.GetDevice();
    vk::SemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.setFlags({});
    mSemaphore = device.createSemaphoreUnique(semaphoreCreateInfo);
    if (!mSemaphore)
    {
        throw std::runtime_error("Failed to create semaphore!");
    }
}
} // namespace MEngine::Platform