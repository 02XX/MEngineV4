file(GLOB_RECURSE RESOURCE CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)


add_library(Manager ${RESOURCE})
target_include_directories(Manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Manager PUBLIC Resource RHI)


