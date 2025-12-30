#pragma once
#include "RenderSystem.hpp"
#include <hello_imgui/runner_params.h>
#include <memory>
#include <queue>
using namespace MEngine::Function;
namespace MEngine::Tool
{
struct EditorConsoleLogEntry
{
    LogLevel Level;
    std::string Message;
    std::source_location Loc;
};
class Editor
{
  private:
    HelloImGui::RunnerParams mParams;
    std::shared_ptr<RenderSystem> mRenderSystem;
    std::shared_ptr<Scene> mScene;
    std::shared_ptr<AssetManager> mAssetManager;

    // Log
    int mMaxLogEntries{1000};
    std::vector<EditorConsoleLogEntry> mLogEntries{};

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
    // CallBacks
    void LogCallback(LogLevel level, const std::source_location &loc, const std::string &message);
};
} // namespace MEngine::Tool