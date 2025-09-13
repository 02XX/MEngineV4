file(GLOB_RECURSE RESOURCE CONFIGURE_DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
find_package(concurrentqueue CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
add_library(RHI ${RESOURCE})

target_include_directories(RHI PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/include/RHIResources
${CMAKE_CURRENT_SOURCE_DIR}/include/RHIResourceFactories
${CMAKE_CURRENT_SOURCE_DIR}/include/RenderResource
)
target_link_libraries(RHI PUBLIC concurrentqueue::concurrentqueue Vulkan::Vulkan GPUOpen::VulkanMemoryAllocator)

