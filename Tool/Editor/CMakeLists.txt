
file(GLOB_RECURSE RESOURCE CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
find_package(glfw3 CONFIG REQUIRED)

find_package(hello-imgui CONFIG REQUIRED)

hello_imgui_add_app(Editor ${RESOURCE})

target_include_directories(Editor PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)