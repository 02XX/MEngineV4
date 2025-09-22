file(GLOB_RECURSE RESOURCE CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)


add_library(AssetBuilder ${RESOURCE})
target_include_directories(AssetBuilder PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(AssetBuilder PUBLIC Asset Utils)


