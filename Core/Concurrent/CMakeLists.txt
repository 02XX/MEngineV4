file(GLOB_RECURSE RESOURCE CONFIGURE_DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

find_package(concurrentqueue CONFIG REQUIRED)
add_library(Concurrent INTERFACE)

target_include_directories(Concurrent INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Concurrent INTERFACE concurrentqueue::concurrentqueue)
