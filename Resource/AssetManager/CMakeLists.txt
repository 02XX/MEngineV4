file(GLOB_RECURSE RESOURCE CONFIGURE_DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

find_package(nlohmann_json CONFIG REQUIRED)
add_library(AssetManager ${RESOURCE})

target_include_directories(AssetManager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(AssetManager PUBLIC Core Asset)
target_link_libraries(AssetManager PUBLIC nlohmann_json::nlohmann_json)

