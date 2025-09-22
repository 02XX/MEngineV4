#include "FrameBuffer.hpp"
#include "Logger.hpp"
#include "MaterialComponent.hpp"
#include "MeshComponent.hpp"
#include "MeshManager.hpp"
#include "PBRMaterial.hpp"
#include "PBRMaterialManager.hpp"
#include "RHIContext.hpp"
#include "RenderSystem.hpp"
#include "StaticMesh.hpp"
#include "SwapChain.hpp"
#include "TextureRenderTarget2D.hpp"
#include "TransformComponent.hpp"
#include <GLFW/glfw3.h>
#include <cstddef>
#include <memory>
#include <print>
#include <vector>

using namespace MEngine::Platform;
using namespace MEngine::Function;
using namespace MEngine::Resource;
using namespace MEngine::Core;

class SandBox
{
  public:
    GLFWwindow *mWindow{nullptr};
    std::shared_ptr<RenderSystem> mRenderSystem{nullptr};
    std::shared_ptr<Scene> mScene{nullptr};
    std::shared_ptr<AssetManager> mAssetManager{nullptr};
    std::shared_ptr<FrameBuffer> mFrameBuffer{nullptr};
    std::shared_ptr<SwapChain> mSwapChain{nullptr};
    struct WindowConfig
    {
        int width = 1280;
        int height = 720;
        std::string title = "MEngineV4 SandBox";
    } mWindowConfig;
    SandBox()
    {
        if (!glfwInit())
        {
            LogError("Failed to initialize GLFW");
            return;
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        mWindow =
            glfwCreateWindow(mWindowConfig.width, mWindowConfig.height, mWindowConfig.title.c_str(), nullptr, nullptr);
        if (!mWindow)
        {
            LogError("Failed to create GLFW window");
            glfwTerminate();
            return;
        }
        glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
        uint32_t vulkanInstanceExtensionCount = 0;
        const char **vulkanInstanceExtensions = glfwGetRequiredInstanceExtensions(&vulkanInstanceExtensionCount);
        std::vector<const char *> extensions(vulkanInstanceExtensions,
                                             vulkanInstanceExtensions + vulkanInstanceExtensionCount);

        RHIContextConfig rhiConfig{};
        rhiConfig.InstanceRequiredExtensions = extensions;
        rhiConfig.InstanceRequiredLayers = {"VK_LAYER_KHRONOS_validation"};
        rhiConfig.DeviceRequiredExtensions = {"VK_KHR_swapchain"};

        RHIContext::Instance().InitInstance(rhiConfig);
        VkSurfaceKHR surface;
        glfwCreateWindowSurface(RHIContext::Instance().GetInstance(), mWindow, nullptr, &surface);
        RHIContext::Instance().InitSurface(surface);
        RHIContext::Instance().InitContext();
        mAssetManager = std::make_shared<AssetManager>();

        mSwapChain = std::make_shared<SwapChain>("MainSwapChain");
        mSwapChain->GetResource()->InitResource();
        mScene = std::make_shared<Scene>("SampleScene");
        mFrameBuffer = std::make_shared<FrameBuffer>("MRT", mWindowConfig.width, mWindowConfig.height);
        mRenderSystem = std::make_shared<RenderSystem>(mScene, mAssetManager);
        mRenderSystem->SetFrameBuffers({mFrameBuffer});
    }
    ~SandBox()
    {
        if (mWindow)
        {
            glfwDestroyWindow(mWindow);
            mWindow = nullptr;
        }
        glfwTerminate();
    }
    void Run()
    {
        mRenderSystem->Init();
        vk::UniqueSemaphore imageAvailableSemaphore = RHIContext::Instance().GetDevice().createSemaphoreUnique({});
        auto registry = mScene->GetRegistry();
        auto cube = registry->create();
        auto cubeMesh = mAssetManager->GetManager<StaticMesh, MeshManager>()->GetMesh(DefaultMeshType::Sphere);
        auto cubeMaterial = mAssetManager->GetManager<PBRMaterial, PBRMaterialManager>()->GetMaterial(
            DefaultMaterialType::GBufferPBROpaque);
        TransformComponent transformComponent;
        MeshComponent meshComponent;
        meshComponent.Mesh = cubeMesh;
        MaterialComponent materialComponent;
        materialComponent.Material = cubeMaterial;
        registry->emplace<TransformComponent>(cube, transformComponent);
        registry->emplace<MeshComponent>(cube, meshComponent);
        registry->emplace<MaterialComponent>(cube, materialComponent);
        auto rhiSwapChain = mSwapChain->GetResourceAs<SwapChainResource>()->GetRHISwapChain();
        while (!glfwWindowShouldClose(mWindow))
        {
            glfwPollEvents();
            mRenderSystem->Update(0.016);
            RHIContext::Instance().GetDevice().waitIdle();
            auto swapChainResources = mSwapChain->GetResourceAs<SwapChainResource>()->GetSwapChainTextures();
            uint32_t imageIndex = 0;
            auto result = RHIContext::Instance().GetDevice().acquireNextImageKHR(
                rhiSwapChain->GetSwapChain(), std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore.get(),
                nullptr, &imageIndex);
            if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
            {
            }
            // result = RHIContext::Instance().GetDevice().waitForFences({fence.get()}, vk::True,
            //                                                           std::numeric_limits<uint64_t>::max());
            if (result != vk::Result::eSuccess)
            {
            }
            auto colorAttachments = mFrameBuffer->GetColorAttachments()
                                        .front()
                                        ->GetResourceAs<TextureRenderTarget2DResource>()
                                        ->GetTexture();
            colorAttachments->CopyTo(swapChainResources[imageIndex].Get());
            vk::PresentInfoKHR presentInfo;
            presentInfo.setSwapchains(mSwapChain->GetResourceAs<SwapChainResource>()->GetRHISwapChain()->GetSwapChain())
                .setImageIndices(imageIndex)
                .setWaitSemaphores({imageAvailableSemaphore.get()});
            auto presentResult = RHIContext::Instance().GetPresentQueue().presentKHR({presentInfo});
            if (presentResult != vk::Result::eSuccess)
            {
            }
        }
        mRenderSystem->Shutdown();
        RHIContext::Instance().GetDevice().waitIdle();
    }
};

int main()
{
    {
        SandBox sandbox;
        sandbox.Run();
    }
    // RHIContext::Instance().GetDevice().waitIdle();
    while (PendingDeletes.Size() > 0)
    {
        auto resource = PendingDeletes.Consume();
        std::println("Deleting resource");
        delete resource;
    }
    return 0;
}