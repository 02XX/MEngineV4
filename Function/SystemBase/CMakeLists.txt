add_library(SystemBase INTERFACE)
target_include_directories(SystemBase INTERFACE 
${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(SystemBase INTERFACE Resource)

