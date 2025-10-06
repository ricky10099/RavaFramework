#pragma once
#include "RavaFramework.h"

static constexpr int MAX_FRAMES_SYNC = 2;

const std::vector<const char*> DEVICE_EXTENSIONS = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

namespace VK {
struct SwapChainDetails {
  VkSurfaceCapabilitiesKHR SurfaceCapabilities;
  std::vector<VkSurfaceFormatKHR> Formats;
  std::vector<VkPresentModeKHR> PresentModes;
};

struct QueueFamilyIndices {
  int GraphicsFamily = -1;
  int PresentFamily = -1;
  //bool GraphicsFamilyHasValue = false;
  //bool PresentFamilyHasValue  = false;
  bool IsValid() const { return GraphicsFamily >= 0 && PresentFamily >= 0; }
};
}