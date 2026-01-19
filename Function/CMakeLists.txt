add_subdirectory("System")

add_library(Function INTERFACE)
target_link_libraries(Function INTERFACE "System" )