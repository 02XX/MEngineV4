file(GLOB_RECURSE RESOURCE CONFIGURE_DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)


add_library(Asset ${RESOURCE})

target_include_directories(Asset PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Asset PUBLIC Core)

