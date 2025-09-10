file(GLOB_RECURSE RESOURCE CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)


add_library(RHI ${RESOURCE})
target_include_directories(RHI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(RHI PUBLIC Resource)


