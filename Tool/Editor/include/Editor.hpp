#pragma once
#include "Context.hpp"
#include "RenderSystem.hpp"
#include <hello_imgui/runner_params.h>
#include <memory>
#include <queue>

using namespace MEngine::Platform;
using namespace MEngine::Function;
namespace MEngine::Tool
{
class Editor
{
  private:
    std::shared_ptr<Context> mContext{};
    HelloImGui::RunnerParams mParams{};
    std::shared_ptr<RenderSystem> mRenderSystem{};

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