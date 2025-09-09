add_subdirectory(Asset)
add_subdirectory(AssetManager)


add_library(Resource INTERFACE)
target_link_libraries(Resource INTERFACE Asset AssetManager)