#include "Editor.hpp"
using namespace MEngine::Tool;
int main()
{
    {
        Editor editor;
        editor.Run();
    }
    while (PendingDeletes.Size() > 0)
    {
        auto resource = PendingDeletes.Consume();
        delete resource;
    }
    return 0;
}