add_subdirectory(Editor)

add_library(Tool INTERFACE)
target_link_libraries(Tool INTERFACE Editor)