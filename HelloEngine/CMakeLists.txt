project(HelloEngine)
# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


include_directories(${PROJECT_SOURCE_DIR}/include)
#include_directories("${CMAKE_SOURCE_DIR}/thirdparty/VulkanMemoryAllocator/include")

# 手动设置 Vulkan 的路径
set(VULKAN_SDK "C:/VulkanSDK/1.3.290.0")  # 请将此路径替换为您的 Vulkan SDK 安装路径

set(Vulkan_INCLUDE_DIR "${VULKAN_SDK}/Include")
set(Vulkan_LIBRARY "${VULKAN_SDK}/Lib/vulkan-1.lib")

set(GLFW_VULKAN_STATIC ON)

# 添加 Vulkan 的 include 目录
include_directories(${Vulkan_INCLUDE_DIR})

# 指定第三方库目录
set(SPDLOG_DIR "${CMAKE_SOURCE_DIR}/thirdparty/spdlog")
set(GLM_DIR "${CMAKE_SOURCE_DIR}/thirdparty/glm")
set(GLFW_DIR "${CMAKE_SOURCE_DIR}/thirdparty/glfw")
set(VKMA_DIR "${CMAKE_SOURCE_DIR}/thirdparty/VulkanMemoryAllocator")
set(RAPIDOBJ_DIR "${CMAKE_SOURCE_DIR}/thirdparty/rapidobj")

# 添加 spdlog 的 CMakeLists.txt
add_subdirectory(${SPDLOG_DIR} ${CMAKE_BINARY_DIR}/thirdparty/spdlog)
# Add GLM directory
add_subdirectory(${GLM_DIR} ${CMAKE_BINARY_DIR}/thirdparty/glm)
# Add GLFW directory
add_subdirectory(${GLFW_DIR} ${CMAKE_BINARY_DIR}/thirdparty/glfw)
# Add VKMA directory
add_subdirectory(${VKMA_DIR} ${CMAKE_BINARY_DIR}/thirdparty/VulkanMemoryAllocator)
#add rapidObj directory
add_subdirectory(${RAPIDOBJ_DIR} ${CMAKE_BINARY_DIR}/thirdparty/rapidobj)


#add source files
file(GLOB_RECURSE ENGINE_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE ENGINE_INCLUDES ${PROJECT_SOURCE_DIR}/include/*.h)

#Create a static library for the engine
#add_library(HelloEngine STATIC ${ENGINE_SOURCES} ${ENGINE_INCLUDES})
add_library(HelloEngine SHARED ${ENGINE_SOURCES} ${ENGINE_INCLUDES})
target_compile_definitions(HelloEngine PRIVATE HELLO_ENGINE_EXPORTS)
target_compile_definitions(HelloEngine PRIVATE MATCH_INNER_VISIBLE)


target_link_libraries(HelloEngine PUBLIC 
    spdlog::spdlog
    ${Vulkan_LIBRARY}
    glm
    glfw
    VulkanMemoryAllocator
    rapidobj
)
#target_link_libraries()

#inlcude directories for the engine
target_include_directories(HelloEngine PUBLIC include)

#link third_party libraries
#target_link_libraries(HelloEngine PUBLIC GLFW)
# 添加编译选项（可选）
#target_compile_features(Engine PRIVATE cxx_std_17)

# 在 Windows 平台上定义 _WIN32 宏
if(WIN32)
    target_compile_definitions(HelloEngine PRIVATE _WIN32)
endif()