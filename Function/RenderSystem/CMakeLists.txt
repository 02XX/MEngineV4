add_subdirectory(Builder)
add_subdirectory(Manager)
add_subdirectory(RHI)
add_subdirectory(Utils)

add_library(RenderSystem INTERFACE)
target_link_libraries(RenderSystem INTERFACE Builder Manager RHI Utils)