#pragma once
#include "RenderSystem.hpp"
#include <hello_imgui/runner_params.h>
#include <memory>
using namespace MEngine::Function;
namespace MEngine::Tool
{

class Editor
{
  private:
    HelloImGui::RunnerParams mParams;
    std::shared_ptr<RenderSystem> mRenderSystem;
    std::shared_ptr<Scene> mScene;
    std::shared_ptr<AssetManager> mAssetManager;

  public:
    Editor();
    ~Editor();
    void Run();

  private:
    void ViewPort();
    void Hierarchy();
    void AssetBrowser();
    void Console();
    void Inspector();
};
} // namespace MEngine::Tool