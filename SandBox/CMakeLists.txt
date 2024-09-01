project(SandBox)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

file (GLOB SANDBOX_SOURCES src/*.cpp)
file (GLOB SANDBOX_INCLUDES include/*.h)

#Create the excutable for project
add_executable(SandBox ${SANDBOX_SOURCES} ${SANDBOX_INCLUDES})

#inlcude direactories for project
target_include_directories(SandBox PUBLIC include)

#Link the engine and third party libraries
target_link_libraries(SandBox PRIVATE
    HelloEngine
    #GLFW
)
# 添加编译选项（可选）
#target_compile_features(Game PRIVATE cxx_std_17)

# 在 Windows 平台上定义 _WIN32 宏
if(WIN32)
    target_compile_definitions(SandBox PRIVATE _WIN32)
endif()

add_custom_command(TARGET SandBox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:HelloEngine>
    $<TARGET_FILE_DIR:SandBox>)