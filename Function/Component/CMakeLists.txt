
add_library(Component INTERFACE)
target_include_directories(Component INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(Component INTERFACE Resource)

