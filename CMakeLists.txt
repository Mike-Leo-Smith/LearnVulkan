cmake_minimum_required(VERSION 3.22)
project(LearnVulkan)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

add_subdirectory(SPIRV-Headers)
add_subdirectory(SPIRV-Tools)
add_subdirectory(glslang)

set(SHADERC_SKIP_TESTS ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_COPYRIGHT_CHECK ON CACHE BOOL "" FORCE)
set(SHADERC_ENABLE_SHARED_CRT ON CACHE BOOL "" FORCE)
add_subdirectory(shaderc)
set_target_properties(shaderc_combined_genfile PROPERTIES EXCLUDE_FROM_ALL ON) # Problematic on M1 Macs

add_executable(LearnVulkan main.cpp)
target_link_libraries(LearnVulkan PRIVATE Vulkan::Vulkan shaderc)
target_compile_definitions(LearnVulkan PRIVATE
        GLFW_INCLUDE_VULKAN
        GLM_FORCE_RADIANS
        GLM_FORCE_DEPTH_ZERO_TO_ONE)
