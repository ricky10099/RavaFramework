#include "RavaFramework.h"

#include "Graphics/Vulkan/VKContext.h"

#include "Core/Config.h"
#include "Core/Window.h"
#include "Graphics/Vulkan/VKUtils.h"
#include "Graphics/Vulkan/VKValidation.h"

namespace VK {
Context::Context() {
  _initialized = true;

  CreateInstance();
  SetupDebugMessenger();
  CreateSurface();
  PickPhysicalDevice();
  CreateLogicalDevice();
  CreateCommandPool();
}

void Context::CreateInstance() {
  if (ENABLE_VALIDATION && IsValidationLayerSupport()) {
    std::print("Validation layers requested, but not available!\n");
  }

  // Application info (optional)
  VkApplicationInfo appInfo{};
  appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName   = Config::WindowTitle.data();
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName        = "RavaFramework";
  appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion         = VK_API_VERSION_1_4;

  // Instance create info
  VkInstanceCreateInfo createInfo{};
  createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  // Extensions
  auto extensions = GetRequiredExtensions();
  CheckRequiredInstanceExtensions();

  createInfo.enabledExtensionCount   = static_cast<u32>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  // Validation layers
  if (ENABLE_VALIDATION) {
    createInfo.enabledLayerCount   = static_cast<u32>(VALIDATION_LAYERS.size());
    createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    PopulateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
  } else {
    createInfo.enabledLayerCount   = 0;
    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.pNext               = nullptr;
  }

  // Create instance
  VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance);
  _initialized    = IsResultValid(result, "Failed to Create Vulkan Instance!\n");
}

void Context::SetupDebugMessenger() {
  if (!ENABLE_VALIDATION) {
    return;
  }

  VkDebugUtilsMessengerCreateInfoEXT createInfo;
  PopulateDebugMessengerCreateInfo(createInfo);
  VkResult result = CreateDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugMessenger);
  _initialized    = IsResultValid(result, "Failed to Setup Debug Messenger!\n");
}

void Context::CreateSurface() {
  VkResult result = glfwCreateWindowSurface(
      _instance, Rava::Window::Instance->GetGLFWWindow(), nullptr, &_surface
  );
  _initialized = IsResultValid(result, "Failed to Create Surface!\n");
}

void Context::PickPhysicalDevice() {
  u32 deviceCount = 0;
  vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    std::print("Failed to Find GPUs with Vulkan support!\n");
  }
  std::print("Device count: {0}\n", deviceCount);

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

  for (const auto& device : devices) {
    if (IsDeviceSuitable(device)) {
      _physicalDevice = device;
      break;
    }
  }

  if (_physicalDevice == VK_NULL_HANDLE) {
    std::print("Failed to Find a suitable GPU!\n");
  }

  vkGetPhysicalDeviceProperties(_physicalDevice, &_physicalDeviceProperties);
  std::print("Physical Device: {}\n", _physicalDeviceProperties.deviceName);
}

void Context::CreateLogicalDevice() {
  QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(_physicalDevice);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<int> uniqueQueueFamilies
      = {queueFamilyIndices.GraphicsFamily, queueFamilyIndices.PresentFamily};

  float queuePriority = 1.0f;
  for (u32 queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex        = queueFamily;
    queueCreateInfo.queueCount              = 1;
    queueCreateInfo.pQueuePriorities        = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures = {};
  deviceFeatures.samplerAnisotropy        = VK_TRUE;

  VkDeviceCreateInfo createInfo      = {};
  createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount    = static_cast<u32>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos       = queueCreateInfos.data();
  createInfo.pEnabledFeatures        = &deviceFeatures;
  createInfo.enabledExtensionCount   = static_cast<u32>(DEVICE_EXTENSIONS.size());
  createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

  VkResult result = vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device);
  _initialized    = IsResultValid(result, "Failed to Create Logical Device!\n");

  vkGetDeviceQueue(_device, queueFamilyIndices.GraphicsFamily, 0, &_graphicsQueue);
  vkGetDeviceQueue(_device, queueFamilyIndices.PresentFamily, 0, &_presentQueue);
}

void Context::CreateCommandPool() {
  QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(_physicalDevice);

  VkCommandPoolCreateInfo poolInfo = {};
  poolInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags
      = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily;

  VkResult result = vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool);
  _initialized    = IsResultValid(result, "Failed to Create Command Pool!\n");
}

bool Context::IsValidationLayerSupport() {
  u32 layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  bool layerFound = false;
  for (const char* layerName : VALIDATION_LAYERS) {
    for (const auto& layerProperties : availableLayers) {
      if (strcmp(layerName, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }
  }

  return layerFound;
}

void Context::CreateImageWithInfo(
    const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image,
    VkDeviceMemory& imageMemory
) {
  if (vkCreateImage(_device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
    throw std::runtime_error("failed to create image!");
  }

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(_device, image, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize  = memRequirements.size;
  allocInfo.memoryTypeIndex = FindMemoryTypeIndex(memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(_device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate image memory!");
  }

  if (vkBindImageMemory(_device, image, imageMemory, 0) != VK_SUCCESS) {
    throw std::runtime_error("failed to bind image memory!");
  }
}

u32 Context::FindMemoryTypeIndex(u32 allowedTypes, VkMemoryPropertyFlags properties) const {
  // Get properties of physical device memory
  VkPhysicalDeviceMemoryProperties memoryProperties;
  vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memoryProperties);

  for (u32 i = 0; i < memoryProperties.memoryTypeCount; ++i) {
    if ((allowedTypes & (1 << i))
        && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }
}

VkFormat Context::FindSupportedFormat(
    const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features
) {
  for (VkFormat format : candidates) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(_physicalDevice, format, &props);

    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
      return format;
    } else if (tiling == VK_IMAGE_TILING_OPTIMAL
               && (props.optimalTilingFeatures & features) == features) {
      return format;
    }
  }
  throw std::runtime_error("failed to find supported format!");
}

std::vector<const char*> Context::GetRequiredExtensions() {
  u32 glfwExtensionCount = 0;
  const char** glfwExtensions;

  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

  if (ENABLE_VALIDATION) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

void Context::CheckRequiredInstanceExtensions() {
  u32 extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

  std::print("Available Extensions:\n");
  std::unordered_set<std::string_view> available;
  for (const auto& extension : extensions) {
    std::print("\t{0}\n", extension.extensionName);
    available.insert(extension.extensionName);
  }

  std::print("Required Extensions:\n");
  auto requiredExtensions = GetRequiredExtensions();
  for (const auto& required : requiredExtensions) {
    std::print("\t{0}\n", required);
    if (available.find(required) == available.end()) {
      std::print("Missing required glfw extension\n");
    }
  }
}

bool Context::IsDeviceSuitable(VkPhysicalDevice device) {
  QueueFamilyIndices indices = FindQueueFamilies(device);

  bool extensionsSupported = IsDeviceExtensionSupport(device);

  bool swapChainAdequate = false;
  if (extensionsSupported) {
    SwapchainDetails swapChainDetails = GetSwapchainDetails(device);
    swapChainAdequate = !swapChainDetails.Formats.empty() && !swapChainDetails.PresentModes.empty();
  }

  VkPhysicalDeviceFeatures supportedFeatures;
  vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

  return indices.IsValid() && extensionsSupported && swapChainAdequate
      && supportedFeatures.samplerAnisotropy;
}

bool Context::IsDeviceExtensionSupport(VkPhysicalDevice device) {
  u32 extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

  if (extensionCount == 0) {
    return false;
  }

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(
      device, nullptr, &extensionCount, availableExtensions.data()
  );

  std::set<std::string_view> requiredExtensions(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());
  for (const auto& extension : availableExtensions) {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}

QueueFamilyIndices Context::FindQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices;

  u32 queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

  int i = 0;
  for (const auto& queueFamily : queueFamilies) {
    if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.GraphicsFamily = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);
    if (queueFamily.queueCount > 0 && presentSupport) {
      indices.PresentFamily = i;
    }

    if (indices.IsValid()) {
      break;
    }

    i++;
  }

  return indices;
}

SwapchainDetails Context::GetSwapchainDetails(VkPhysicalDevice device) {
  SwapchainDetails swapChainDetails;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      device, _surface, &swapChainDetails.SurfaceCapabilities
  );

  u32 formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);

  if (formatCount != 0) {
    swapChainDetails.Formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        device, _surface, &formatCount, swapChainDetails.Formats.data()
    );
  }

  u32 presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);

  if (presentModeCount != 0) {
    swapChainDetails.PresentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, _surface, &presentModeCount, swapChainDetails.PresentModes.data()
    );
  }

  return swapChainDetails;
}
}  // namespace VK