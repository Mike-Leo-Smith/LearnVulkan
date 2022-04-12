#include <iostream>
#include <vector>

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

inline void check_vulkan_impl(VkResult code, const char *file, int line) noexcept {
    if (code != VK_SUCCESS) {
        std::cerr << "Vulkan error: " << file << ":" << line << ": " << vk::to_string(static_cast<vk::Result>(code)) << std::endl;
        std::exit(1);
    }
}

int main() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    auto window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    for (auto &&extension: extensions) {
        std::cout << "Extension '" << extension.extensionName << "': "
                  << extension.specVersion << std::endl;
    }

    std::cout << extensionCount << " extensions supported\n";

    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}