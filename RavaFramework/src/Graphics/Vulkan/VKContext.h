#pragma once

#include "Graphics/Context.h"
#include "Graphics/Vulkan/VKUtils.h"

namespace VK {
struct QueueFamilyIndices;
struct SwapChainDetails;

class Context /*: public Rava::Context*/ {
public:
  Context();
  virtual ~Context()/* override*/ {}

  NO_COPY(Context)
  NO_MOVE(Context)

  // virtual void BeginFrame() override {}
  // virtual void EndFrame() override {}
  // virtual void DrawTriangle() override {}
  // virtual void DrawQuad(float x, float y, float w, float h) override {}

  inline VkCommandPool GetCommandPool() const { return _commandPool; }
  inline VkSurfaceKHR GetSurface() const { return _surface; }
  inline VkPhysicalDevice GetPhysicalDevice() const { return _physicalDevice; }
  inline VkDevice GetLogicalDevice() const { return _device; }
  inline VkCommandPool GetCommandPool() const { return _commandPool; }
  inline VkQueue GetGraphicsQueue() const { return _graphicsQueue; }
  inline VkQueue GetPresentQueue() const { return _presentQueue; }
  inline const QueueFamilyIndices& GetPhysicalQueueFamilies() { return _queueFamilyIndices; }
  inline const SwapChainDetails& GetSwapChainDetails() { return _swapChainDetails; }
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
  QueueFamilyIndices _queueFamilyIndices;
  SwapChainDetails _swapChainDetails;

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
  QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) const;
  SwapChainDetails GetSwapChainDetails(VkPhysicalDevice device) const;
  // u32 FindMemoryType(u32 typeFilter, VkMemoryPropertyFlags properties);
};
}  // namespace VK