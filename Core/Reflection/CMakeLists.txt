file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_library(Reflection ${SOURCES})

target_include_directories(Reflection PUBLIC "include")

