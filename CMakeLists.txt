cmake_minimum_required(VERSION 3.15)
project(HelloEngineProject)

# set c++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Add thirdparty directory
add_subdirectory(thirdparty)

#Add Engine project
add_subdirectory(HelloEngine)

#Add SandBox Project
add_subdirectory(SandBox)
