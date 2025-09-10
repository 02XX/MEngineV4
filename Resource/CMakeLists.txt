add_subdirectory(Asset)
add_subdirectory(AssetManager)


add_library(Resource INTERFACE)
target_link_libraries(Resource INTERFACE Asset AssetManager)


#Engine
add_custom_target(
    Engine
    ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/Assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Engine ${CMAKE_BINARY_DIR}/bin/Assets
    COMMENT "Copying assets files from source_folder to destination_folder..."
    VERBATIM
)
add_dependencies(Resource Engine)
