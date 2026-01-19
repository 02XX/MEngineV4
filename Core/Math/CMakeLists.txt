file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
find_package(glm CONFIG REQUIRED) 
add_library(Math ${SOURCES})

target_include_directories(Math 
PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(Math PUBLIC glm::glm)