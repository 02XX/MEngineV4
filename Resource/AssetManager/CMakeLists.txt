file(GLOB_RECURSE RESOURCE CONFIGURE_DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

find_package(reflectcpp CONFIG REQUIRED)

find_package(slang CONFIG REQUIRED)


add_library(AssetManager ${RESOURCE})

target_include_directories(AssetManager PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/include/AssetBuilder
)

target_link_libraries(AssetManager PUBLIC Core Asset)
target_link_libraries(AssetManager PUBLIC reflectcpp::reflectcpp Component)
target_link_libraries(AssetManager PRIVATE slang::slang)

