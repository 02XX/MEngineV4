
file(GLOB_RECURSE RESOURCE CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

find_package(imgui CONFIG REQUIRED)
find_package(imguizmo CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Taskflow CONFIG REQUIRED)

add_executable(Editor ${RESOURCE})

target_include_directories(Editor PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(Editor PUBLIC Function imgui::imgui imguizmo::imguizmo  glfw Taskflow::Taskflow)