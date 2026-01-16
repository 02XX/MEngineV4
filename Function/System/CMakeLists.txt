file(GLOB_RECURSE RESOURCE CONFIGURE_DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library("System" ${RESOURCE})
target_include_directories("System" PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Render
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Camera
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Transform
)
target_link_libraries("System" PUBLIC Resource Component)

