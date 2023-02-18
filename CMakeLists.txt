cmake_minimum_required(VERSION 3.19 FATAL_ERROR)
project(GL_UI VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/ThirdParty)

add_subdirectory(Source)
add_subdirectory(ThirdParty)
add_subdirectory(PathTrace)
add_subdirectory(TestImGui)