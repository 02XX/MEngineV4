file(GLOB_RECURSE RESOURCE CONFIGURE_DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

find_package(concurrentqueue CONFIG REQUIRED)
add_library(Concurrent ${RESOURCE})

target_include_directories(Concurrent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Concurrent PUBLIC concurrentqueue::concurrentqueue)
