#pragma once
#include <hello_imgui/runner_params.h>
#include <memory>
#include <queue>
namespace MEngine::Tool
{
class Editor
{
  private:
    HelloImGui::RunnerParams mParams;

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