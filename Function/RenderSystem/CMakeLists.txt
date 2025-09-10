file(GLOB_RECURSE RESOURCE CONFIGURE_DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(RenderSystem ${RESOURCE})
target_include_directories(RenderSystem PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/include/Builder
${CMAKE_CURRENT_SOURCE_DIR}/include/Builder/Pipeline
${CMAKE_CURRENT_SOURCE_DIR}/include/Builder/PipelineLayout
${CMAKE_CURRENT_SOURCE_DIR}/include/Builder/Shader
${CMAKE_CURRENT_SOURCE_DIR}/include/Builder/Texture
${CMAKE_CURRENT_SOURCE_DIR}/include/Builder/Director
${CMAKE_CURRENT_SOURCE_DIR}/include/Builder/FrameBuffer
${CMAKE_CURRENT_SOURCE_DIR}/include/Builder/Interface
${CMAKE_CURRENT_SOURCE_DIR}/include/Manager
${CMAKE_CURRENT_SOURCE_DIR}/include/RHI
${CMAKE_CURRENT_SOURCE_DIR}/include/Utils
)
target_link_libraries(RenderSystem PUBLIC Resource SystemBase ${Stb_INCLUDE_DIR} Component)

