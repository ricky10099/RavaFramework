#pragma once

//#include "Graphics/Context.h"
// #include "Graphics/Vulkan/VKUtils.h"

namespace VK {
struct SwapchainDetails {
  VkSurfaceCapabilitiesKHR SurfaceCapabilities;
  std::vector<VkSurfaceFormatKHR> Formats;
  std::vector<VkPresentModeKHR> PresentModes;
};

struct QueueFamilyIndices {
  int GraphicsFamily = -1;
  int PresentFamily  = -1;
  // bool GraphicsFamilyHasValue = false;
  // bool PresentFamilyHasValue  = false;
  bool IsValid() const { return GraphicsFamily >= 0 && PresentFamily >= 0; }
};

class Context /*: public Rava::Context*/ {
public:
  Context();
  virtual ~Context() /* override*/ { std::print("~Context"); }

  NO_COPY(Context)
  NO_MOVE(Context)

  // virtual void BeginFrame() override {}
  // virtual void EndFrame() override {}
  // virtual void DrawTriangle() override {}
  // virtual void DrawQuad(float x, float y, float w, float h) override {}

  void CreateImageWithInfo(
      const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image,
      VkDeviceMemory& imageMemory
  );

  u32 FindMemoryTypeIndex(u32 allowedTypes, VkMemoryPropertyFlags properties) const;
  VkFormat FindSupportedFormat(
      const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features
  );

  inline VkCommandPool GetCommandPool() const { return _commandPool; }
  inline VkSurfaceKHR GetSurface() const { return _surface; }
  inline VkPhysicalDevice GetPhysicalDevice() const { return _physicalDevice; }
  inline VkDevice GetLogicalDevice() const { return _device; }
  inline VkQueue GetGraphicsQueue() const { return _graphicsQueue; }
  inline VkQueue GetPresentQueue() const { return _presentQueue; }
  inline QueueFamilyIndices GetPhysicalQueueFamilies() { return FindQueueFamilies(_physicalDevice); }
  inline SwapchainDetails GetSwapchainDetails() { return GetSwapchainDetails(_physicalDevice); }
  inline const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const {
    return _physicalDeviceProperties;
  }

  inline bool IsInitialized() const { return _initialized; }

private:
  VkInstance _instance                     = VK_NULL_HANDLE;
  VkSurfaceKHR _surface                    = VK_NULL_HANDLE;
  VkPhysicalDevice _physicalDevice         = VK_NULL_HANDLE;
  VkDevice _device                         = VK_NULL_HANDLE;
  VkQueue _graphicsQueue                   = VK_NULL_HANDLE;
  VkQueue _presentQueue                    = VK_NULL_HANDLE;
  VkCommandPool _commandPool               = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT _debugMessenger = VK_NULL_HANDLE;
  VkPhysicalDeviceProperties _physicalDeviceProperties;
  //QueueFamilyIndices _queueFamilyIndices;

private:
  bool _initialized = false;
  // Initialization
  void CreateInstance();
  void SetupDebugMessenger();
  void CreateSurface();
  void PickPhysicalDevice();
  void CreateLogicalDevice();
  void CreateCommandPool();

  // Validation
  bool IsValidationLayerSupport();

  // Helpers
  std::vector<const char*> GetRequiredExtensions();
  void CheckRequiredInstanceExtensions();
  bool IsDeviceSuitable(VkPhysicalDevice device);
  bool IsDeviceExtensionSupport(VkPhysicalDevice device);
  QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
  SwapchainDetails GetSwapchainDetails(VkPhysicalDevice device);
  // u32 FindMemoryType(u32 typeFilter, VkMemoryPropertyFlags properties);
};
}  // namespace VK