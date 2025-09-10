find_package(EnTT CONFIG REQUIRED)
add_library(ECS INTERFACE)

target_include_directories( ECS
INTERFACE 
${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(ECS INTERFACE EnTT::EnTT)