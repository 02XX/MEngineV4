
file(GLOB_RECURSE RESOURCE CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
find_package(glfw3 CONFIG REQUIRED)

add_library(Editor ${RESOURCE})
target_include_directories(Editor PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(Editor PUBLIC Function glfw)