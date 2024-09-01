project(HelloEngine)
# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--verbose")

include_directories(${PROJECT_SOURCE_DIR}/include)

# 手动设置 Vulkan 的路径
set(VULKAN_SDK "C:/VulkanSDK/1.3.290.0")  # 请将此路径替换为您的 Vulkan SDK 安装路径

set(Vulkan_INCLUDE_DIR "${VULKAN_SDK}/Include")
set(Vulkan_LIBRARY "${VULKAN_SDK}/Lib/vulkan-1.lib")

# 添加 Vulkan 的 include 目录
include_directories(${Vulkan_INCLUDE_DIR})

# 指定第三方库目录
set(SPDLOG_DIR "${CMAKE_SOURCE_DIR}/thirdparty/spdlog")

# 添加 spdlog 的 CMakeLists.txt
add_subdirectory(${SPDLOG_DIR} ${CMAKE_BINARY_DIR}/thirdparty/spdlog)


#add source files
file(GLOB_RECURSE ENGINE_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
#file(GLOB ENGINE_SOURCES ${PROJECT_SOURCE_DIR}/src/core_impl/*.cpp)
file(GLOB_RECURSE ENGINE_INCLUDES ${PROJECT_SOURCE_DIR}/include/*.h)
#file(GLOB ENGINE_INCLUDES ${PROJECT_SOURCE_DIR}/include/core/*.h)
#file(GLOB ENGINE_INCLUDES ${PROJECT_SOURCE_DIR}/include/core_impl/*.h)

#Create a static library for the engine
#add_library(HelloEngine STATIC ${ENGINE_SOURCES} ${ENGINE_INCLUDES})
add_library(HelloEngine SHARED ${ENGINE_SOURCES} ${ENGINE_INCLUDES})
target_compile_definitions(HelloEngine PRIVATE HELLO_ENGINE_EXPORTS)


target_link_libraries(HelloEngine PUBLIC spdlog::spdlog
 ${Vulkan_LIBRARY})
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