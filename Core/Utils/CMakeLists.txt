file(GLOB_RECURSE RESOURCE CONFIGURE_DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
find_package(Stb REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)
add_library(Utils ${RESOURCE})
target_include_directories(Utils PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(Utils PUBLIC ${Stb_INCLUDE_DIR} unofficial::shaderc::shaderc Component)

