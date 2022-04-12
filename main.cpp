#include <iostream>
#include <vector>
#include <array>
#include <string_view>
#include <string>
#include <span>

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>
#include <glm/vec4.hpp>

inline std::string vulkan_error_string(VkResult code) noexcept {
    switch (code) {
        case VK_SUCCESS: return "SUCCESS";
        case VK_NOT_READY: return "NOT_READY";
        case VK_TIMEOUT: return "TIMEOUT";
        case VK_EVENT_SET: return "EVENT_SET";
        case VK_EVENT_RESET: return "EVENT_RESET";
        case VK_INCOMPLETE: return "INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY: return "ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED: return "ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST: return "ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED: return "ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT: return "ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT: return "ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT: return "ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER: return "ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS: return "ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED: return "ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL: return "ERROR_FRAGMENTED_POOL";
        case VK_ERROR_SURFACE_LOST_KHR: return "ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR: return "SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR: return "ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_UNKNOWN: return "ERROR_UNKNOWN";
        case VK_ERROR_OUT_OF_POOL_MEMORY: return "ERROR_OUT_OF_POOL_MEMORY";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "ERROR_INVALID_EXTERNAL_HANDLE";
        case VK_ERROR_FRAGMENTATION: return "ERROR_FRAGMENTATION";
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: return "ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
        case VK_PIPELINE_COMPILE_REQUIRED: return "PIPELINE_COMPILE_REQUIRED";
        case VK_ERROR_VALIDATION_FAILED_EXT: return "ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV: return "ERROR_INVALID_SHADER_NV";
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: return "ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        case VK_ERROR_NOT_PERMITTED_KHR: return "ERROR_NOT_PERMITTED_KHR";
        case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: return "ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
        case VK_THREAD_IDLE_KHR: return "THREAD_IDLE_KHR";
        case VK_THREAD_DONE_KHR: return "THREAD_DONE_KHR";
        case VK_OPERATION_DEFERRED_KHR: return "OPERATION_DEFERRED_KHR";
        case VK_OPERATION_NOT_DEFERRED_KHR: return "OPERATION_NOT_DEFERRED_KHR";
        case VK_RESULT_MAX_ENUM: return "RESULT_MAX_ENUM";
        default: break;
    }
    return "UNKNOWN_ERROR(" + std::to_string(code) + ")";
}

inline void check_vulkan_impl(VkResult code, const char *file, int line) noexcept {
    if (code == VK_SUCCESS) { return; }
    if (code > 0) {// warning
        std::cerr << "Vulkan warning: " << vulkan_error_string(code)
                  << " [" << file << ":" << line << "]" << std::endl;
    } else {
        std::cerr << "Vulkan error: " << vulkan_error_string(code)
                  << " [" << file << ":" << line << "]" << std::endl;
        std::exit(1);
    }
}

#define CHECK_VULKAN(code) check_vulkan_impl(code, __FILE__, __LINE__)

int main() {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "LuisaComputeTest";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "LuisaCompute";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    std::vector enabledExtensions{VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
    std::vector enabledLayers{"VK_LAYER_KHRONOS_validation"};

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = enabledLayers.size();
    createInfo.ppEnabledLayerNames = enabledLayers.data();
    createInfo.enabledExtensionCount = enabledExtensions.size();
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();

    VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{};
    messengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    messengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    messengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    messengerCreateInfo.pUserData = nullptr;
    messengerCreateInfo.pfnUserCallback = [](auto severity, auto type, auto pCallbackData, auto) noexcept {
        using namespace std::string_view_literals;
        auto severity_desc = [severity] {
            switch (severity) {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: return "[VERBOSE]"sv;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: return "[INFO]"sv;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: return "[WARNING]"sv;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: return "[ERROR]"sv;
                default: return "[UNKNOWN]"sv;
            }
        }();
        auto type_desc = [type] {
            std::string desc{"["};
            auto bits = 0u;
            if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
                bits++;
                desc.append("GENERAL|");
            }
            if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
                bits++;
                desc.append("VALIDATION|");
            }
            if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
                bits++;
                desc.append("PERFORMANCE|");
            }
            if (bits > 0u) {
                desc.pop_back();
            } else {
                desc.append("UNKNOWN");
            }
            desc += "]";
            return desc;
        }();
        std::cerr << severity_desc << " " << type_desc << ": "
                  << pCallbackData->pMessage << std::endl;
        if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) { std::exit(1); }
        return VK_FALSE;
    };
    createInfo.pNext = &messengerCreateInfo;

    // create instance
    VkInstance instance;
    CHECK_VULKAN(vkCreateInstance(&createInfo, nullptr, &instance));

    // pick physical device
    auto physicalDeviceCount = 0u;
    CHECK_VULKAN(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr));
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    CHECK_VULKAN(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()));
    for (auto i = 0u; i < physicalDeviceCount; i++) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);
        std::cout << "Physical device #" << i << ": " << properties.deviceName
                  << " (API = " << properties.apiVersion << ", driver = "
                  << properties.driverVersion << ")" << std::endl;
    }
    auto physicalDevice = physicalDevices.front();

    // find queue families
    auto queueFamilyCount = 0u;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    float queuePriority = 1.0f;
    for (auto i = 0u; i < queueFamilyCount; i++) {
        auto q = queueFamilyProperties[i];
        std::string flags;
        if (q.queueFlags & VK_QUEUE_GRAPHICS_BIT) { flags.append("GRAPHICS | "); }
        if (q.queueFlags & VK_QUEUE_COMPUTE_BIT) { flags.append("COMPUTE | "); }
        if (q.queueFlags & VK_QUEUE_TRANSFER_BIT) { flags.append("TRANSFER | "); }
        if (q.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) { flags.append("SPARSE_BINDING | "); }
        if (q.queueFlags & VK_QUEUE_PROTECTED_BIT) { flags.append("PROTECTED | "); }
        if (!flags.empty()) {
            flags.pop_back();
            flags.pop_back();
            flags.pop_back();
        } else {
            flags = "NONE";
        }
        std::cout << "Queue family #" << i
                  << " (count = " << q.queueCount << ", flags = "
                  << flags << ")" << std::endl;
        queueCreateInfos.emplace_back(VkDeviceQueueCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = i,
            .queueCount = q.queueCount,
            .pQueuePriorities = &queuePriority,
        });
    }

    // create logical device
    VkDeviceCreateInfo deviceCreateInfo{};
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledLayerCount = enabledExtensions.size();
    deviceCreateInfo.ppEnabledLayerNames = enabledExtensions.data();
    VkDevice device;
    CHECK_VULKAN(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device));

    // create buffer
    static constexpr auto bufferSize = 1024u;
    std::vector<uint> queueFamilyIndices(queueCreateInfos.size());
    for (auto i = 0u; i < queueCreateInfos.size(); i++) {
        queueFamilyIndices[i] = queueCreateInfos[i].queueFamilyIndex;
    }
    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = sizeof(float) * bufferSize;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                             VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                             VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
    bufferCreateInfo.queueFamilyIndexCount = queueFamilyIndices.size();
    bufferCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    VkBuffer buffer;
    CHECK_VULKAN(vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer));

    // allocate memory
    VkMemoryRequirements memoryRequirements{};
    vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);
    std::cout << "Buffer memory requirements: "
              << "size = " << memoryRequirements.size << "B, "
              << "alignment = " << memoryRequirements.alignment << "B, "
              << "typeBits = " << memoryRequirements.memoryTypeBits << std::endl;
    VkPhysicalDeviceMemoryProperties memoryProperties{};
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
    auto findMemoryTypeIndex = [&](auto properties) noexcept {
        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
            if ((memoryRequirements.memoryTypeBits & (1 << i)) &&
                (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        std::cerr << "Failed to find memory type index." << std::endl;
        exit(1);
    };
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryTypeIndex(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    VkDeviceMemory bufferMemory;
    CHECK_VULKAN(vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory));
    CHECK_VULKAN(vkBindBufferMemory(device, buffer, bufferMemory, 0u));

    // compile shader
    using namespace std::string_view_literals;
    constexpr auto shaderSource = R"(#version 450
layout (local_size_x = 256) in;
layout(set = 0, binding = 0) buffer StorageBuffer {
   float data[];
} block;

void main() {
    uint gID = gl_GlobalInvocationID.x;
    block.data[gID] *= 2.0f;
})"sv;
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetOptimizationLevel(shaderc_optimization_level_performance);
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);
    auto compilationResult = compiler.CompileGlslToSpv(
        shaderSource.data(), shaderSource.size(),
        shaderc_glsl_compute_shader, "shader.comp", options);
    if (compilationResult.GetCompilationStatus() != shaderc_compilation_status_success) {
        std::cerr << "Failed to compile shader: " << compilationResult.GetErrorMessage() << std::endl;
        exit(1);
    }

    // create shader module
    std::span spv{compilationResult.cbegin(), compilationResult.cend()};
    std::cout << "SPIR-V size: " << spv.size_bytes() << "B" << std::endl;
    VkShaderModuleCreateInfo shaderModuleCreateInfo{};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.pCode = spv.data();
    shaderModuleCreateInfo.codeSize = spv.size_bytes();
    VkShaderModule shaderModule;
    CHECK_VULKAN(vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule));

    // create shader stage
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo{};
    shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo.module = shaderModule;
    shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageCreateInfo.pName = "main";

    // clean up
    vkDestroyShaderModule(device, shaderModule, nullptr);
    vkFreeMemory(device, bufferMemory, nullptr);
    vkDestroyBuffer(device, buffer, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
}
