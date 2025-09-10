add_subdirectory(RenderSystem)

add_library(Function INTERFACE)
target_link_libraries(Function INTERFACE RenderSystem)