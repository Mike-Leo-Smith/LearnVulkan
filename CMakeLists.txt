cmake_minimum_required(VERSION 3.22)
project(LearnVulkan)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(LearnVulkan main.cpp)
target_link_libraries(LearnVulkan PRIVATE Vulkan::Vulkan glm::glm glfw)
target_compile_definitions(LearnVulkan PRIVATE
        GLFW_INCLUDE_VULKAN
        GLM_FORCE_RADIANS
        GLM_FORCE_DEPTH_ZERO_TO_ONE)
