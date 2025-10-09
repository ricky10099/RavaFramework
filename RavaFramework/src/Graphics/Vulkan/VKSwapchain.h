#pragma once

namespace VK {
class Context;
class Swapchain {
public:
  Swapchain(Shared<Context> context);
  Swapchain(Shared<Context> context, Shared<Swapchain> oldSwapchain);
  ~Swapchain();

  NO_COPY(Swapchain)

  VkResult AcquireNextImage(uint32_t* imageIndex);
  VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

  inline VkFramebuffer GetFrameBuffer(int index) { return _swapchainFramebuffers[index]; }
  inline VkRenderPass GetRenderPass() const { return _renderPass; }
  inline VkImageView GetImageView(int index) { return _swapchainImageViews[index]; }
  inline size_t ImageCount() { return _swapchainImages.size(); }
  inline VkFormat GetSwapChainImageFormat() const { return _swapchainImageFormat; }
  inline VkExtent2D GetSwapChainExtent() const { return _swapchainExtent; }
  inline u32 Width() const { return _swapchainExtent.width; }
  inline u32 Height() const { return _swapchainExtent.height; }
  inline u32 GetCurrentFrameIndex() const { return _currentFrameIndex; }

  inline float ExtentAspectRatio() const {
    return static_cast<float>(_swapchainExtent.width) / static_cast<float>(_swapchainExtent.height);
  }

  VkFormat FindDepthFormat();
  inline bool CompareSwapFormats(const Swapchain& swapChain) const {
    return swapChain._swapchainDepthFormat == _swapchainDepthFormat
        && swapChain._swapchainImageFormat == _swapchainImageFormat;
  }
  bool IsInitialized() const { return _initialized; }

private:
  bool _initialized = false;
  Shared<Context> _context;
  VkSwapchainKHR _swapchain;
  Shared<Swapchain> _oldSwapchain;
  VkExtent2D _swapchainExtent;

  std::vector<VkFramebuffer> _swapchainFramebuffers;
  VkFormat _swapchainImageFormat;
  std::vector<VkImage> _swapchainImages;
  std::vector<VkImageView> _swapchainImageViews;

  VkRenderPass _renderPass;

  std::vector<VkDeviceMemory> _depthImageMemorys;
  VkFormat _swapchainDepthFormat;
  std::vector<VkImage> _depthImages;
  std::vector<VkImageView> _depthImageViews;

  std::vector<VkSemaphore> _imageAvailableSemaphores;
  std::vector<VkSemaphore> _renderFinishedSemaphores;
  std::vector<VkFence> _inFlightFences;
  u32 _currentFrameIndex = 0;

private:
  void Init();
  void CreateSwapchain();
  void CreateImageViews();
  void CreateRenderPass();
  void CreateDepthResources();
  void CreateFramebuffers();
  void CreateSyncObjects();

  // Helper functions
  VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
  VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
  VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};
}  // namespace VK
