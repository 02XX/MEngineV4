
add_subdirectory(RHI)
add_library(Platform INTERFACE)
target_include_directories(Platform INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Platform INTERFACE RHI)