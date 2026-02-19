file(GLOB_RECURSE RESOURCE CONFIGURE_DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
find_package(concurrentqueue CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_library(SLANG_LIB NAMES slang PATHS $ENV{VULKAN_SDK}/lib NO_DEFAULT_PATH)


add_library(RHI ${RESOURCE})


target_include_directories(RHI PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/include/RHIResources
${CMAKE_CURRENT_SOURCE_DIR}/include/RHIResourceFactories
${CMAKE_CURRENT_SOURCE_DIR}/include/RenderResource
$ENV{VULKAN_SDK}/include
)
target_link_libraries(RHI PUBLIC ${SLANG_LIB} concurrentqueue::concurrentqueue Vulkan::Vulkan) 
if(WIN32)
    # Windows 平台
    target_compile_definitions(RHI PUBLIC VK_USE_PLATFORM_WIN32_KHR)
elseif(APPLE)
    # macOS 和 iOS 平台 (MoltenVK)
    target_compile_definitions(RHI PUBLIC VK_USE_PLATFORM_METAL_EXT)
elseif(ANDROID)
    # Android 平台
    target_compile_definitions(RHI PUBLIC VK_USE_PLATFORM_ANDROID_KHR)
elseif(UNIX AND NOT APPLE)
    # Linux 平台：建议同时开启 XCB 和 Wayland
    target_compile_definitions(RHI PUBLIC VK_USE_PLATFORM_XCB_KHR)
    target_compile_definitions(RHI PUBLIC VK_USE_PLATFORM_WAYLAND_KHR)
    # 如果你还需要支持老旧的 X11 (Xlib)
    target_compile_definitions(RHI PUBLIC VK_USE_PLATFORM_XLIB_KHR)
endif()