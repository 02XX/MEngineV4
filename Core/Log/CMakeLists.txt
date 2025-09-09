file(GLOB_RECURSE RESOURCE CONFIGURE_DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

find_package(spdlog CONFIG REQUIRED)
find_package(concurrentqueue CONFIG REQUIRED)
add_library(Log ${RESOURCE})

target_include_directories(Log PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(Log PUBLIC spdlog::spdlog)
target_link_libraries(Log PUBLIC concurrentqueue::concurrentqueue)
