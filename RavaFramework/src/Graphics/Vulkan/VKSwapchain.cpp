#include "RavaFramework.h"

#include "Graphics/Vulkan/VKSwapchain.h"

#include "Core/Config.h"
#include "Graphics/Vulkan/VKContext.h"
#include "Graphics/Vulkan/VKUtils.h"
#include "Graphics/Vulkan/VkRenderer.h"

namespace VK {
Swapchain::Swapchain(Shared<Context> context) : Swapchain(context, nullptr) {}

Swapchain::Swapchain(Shared<Context> context, Shared<Swapchain> oldSwapchain)
    : _context(context), _oldSwapchain(oldSwapchain) {
  Init();
  _oldSwapchain = nullptr;
}

Swapchain::~Swapchain() {
  std::print("~Swapchain");
  auto device = _context->GetLogicalDevice();
  for (size_t i = 0; i < _swapchainImages.size(); ++i) {
    vkDestroySemaphore(device, _renderFinishedSemaphores[i], nullptr);
  }
  for (int i = 0; i < MAX_FRAMES_SYNC; ++i) {
    vkDestroySemaphore(device, _imageAvailableSemaphores[i], nullptr);
    vkDestroyFence(device, _inFlightFences[i], nullptr);
  }

  for (auto framebuffer : _swapchainFramebuffers) {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
  }

  vkDestroyRenderPass(device, _renderPass, nullptr);

  for (size_t i = 0; i < _depthImages.size(); ++i) {
    vkDestroyImageView(device, _depthImageViews[i], nullptr);
    vkDestroyImage(device, _depthImages[i], nullptr);
    vkFreeMemory(device, _depthImageMemorys[i], nullptr);
  }

  for (auto imageView : _swapchainImageViews) {
    vkDestroyImageView(device, imageView, nullptr);
  }
  _swapchainImageViews.clear();

  vkDestroySwapchainKHR(device, _swapchain, nullptr);
  _swapchain = VK_NULL_HANDLE;
}

void Swapchain::Init() {
  CreateSwapchain();
  CreateImageViews();
  CreateRenderPass();
  CreateDepthResources();
  CreateFramebuffers();
  CreateSyncObjects();
}

void Swapchain::CreateSwapchain() {
  SwapchainDetails swapchainDetail = _context->GetSwapchainDetails();

  VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(swapchainDetail.Formats);
  VkPresentModeKHR presentMode     = ChoosePresentMode(swapchainDetail.PresentModes);
  VkExtent2D extent                = ChooseSwapExtent(swapchainDetail.SurfaceCapabilities);

  u32 imageCount = swapchainDetail.SurfaceCapabilities.minImageCount + 1;
  if (swapchainDetail.SurfaceCapabilities.maxImageCount > 0
      && imageCount > swapchainDetail.SurfaceCapabilities.maxImageCount) {
    imageCount = swapchainDetail.SurfaceCapabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface                  = _context->GetSurface();
  createInfo.minImageCount            = imageCount;
  createInfo.imageFormat              = surfaceFormat.format;
  createInfo.imageColorSpace          = surfaceFormat.colorSpace;
  createInfo.imageExtent              = extent;
  createInfo.imageArrayLayers         = 1;
  createInfo.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  createInfo.preTransform             = swapchainDetail.SurfaceCapabilities.currentTransform;
  createInfo.compositeAlpha           = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode              = presentMode;
  createInfo.clipped                  = VK_TRUE;

  QueueFamilyIndices indices = _context->GetPhysicalQueueFamilies();
  u32 queueFamilyIndices[]   = {indices.GraphicsFamily, indices.PresentFamily};

  if (indices.GraphicsFamily != indices.PresentFamily) {
    createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices   = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices   = nullptr;
  }

  createInfo.oldSwapchain = _oldSwapchain == nullptr ? VK_NULL_HANDLE : _oldSwapchain->_swapchain;

  if (vkCreateSwapchainKHR(_context->GetLogicalDevice(), &createInfo, nullptr, &_swapchain
      )
      != VK_SUCCESS) {
    throw std::runtime_error("Failed to Create Swapchain!");
  }

  vkGetSwapchainImagesKHR(_context->GetLogicalDevice(), _swapchain, &imageCount, nullptr
  );
  _swapchainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(
      _context->GetLogicalDevice(), _swapchain, &imageCount, _swapchainImages.data()
  );

  _swapchainImageFormat = surfaceFormat.format;
  _swapchainExtent      = extent;
  _swapchainDepthFormat = FindDepthFormat();
}

void Swapchain::CreateImageViews() {
  _swapchainImageViews.resize(_swapchainImages.size());
  for (size_t i = 0; i < _swapchainImages.size(); i++) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image                           = _swapchainImages[i];
    viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format                          = _swapchainImageFormat;
    viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel   = 0;
    viewInfo.subresourceRange.levelCount     = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount     = 1;

    if (vkCreateImageView(
            _context->GetLogicalDevice(), &viewInfo, nullptr, &_swapchainImageViews[i]
        )
        != VK_SUCCESS) {
      throw std::runtime_error("Failed to Create Texture Image View!");
    }
  }
}

void Swapchain::CreateRenderPass() {
  VkAttachmentDescription colorAttachment  = {};
  colorAttachment.format                   = _swapchainImageFormat;
  colorAttachment.samples                  = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp                   = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp                  = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp            = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp           = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout            = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout              = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment            = 0;
  colorAttachmentRef.layout                = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment  = {};
  depthAttachment.format                   = _swapchainDepthFormat;
  depthAttachment.samples                  = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp                   = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp                  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp            = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp           = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout            = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout              = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  VkAttachmentReference depthAttachmentRef = {};
  depthAttachmentRef.attachment            = 1;
  depthAttachmentRef.layout                = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass    = {};
  subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount    = 1;
  subpass.pColorAttachments       = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;

  VkSubpassDependency dependency = {};
  dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
  dependency.srcAccessMask       = 0;
  dependency.srcStageMask
      = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.dstSubpass = 0;
  dependency.dstAccessMask
      = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  dependency.dstStageMask
      = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

  std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
  VkRenderPassCreateInfo renderPassInfo              = {};
  renderPassInfo.sType                               = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount                     = static_cast<u32>(attachments.size());
  renderPassInfo.pAttachments                        = attachments.data();
  renderPassInfo.subpassCount                        = 1;
  renderPassInfo.pSubpasses                          = &subpass;
  renderPassInfo.dependencyCount                     = 1;
  renderPassInfo.pDependencies                       = &dependency;

  if (vkCreateRenderPass(
          _context->GetLogicalDevice(), &renderPassInfo, nullptr, &_renderPass
      )
      != VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass!");
  }
}

void Swapchain::CreateDepthResources() {
  VkExtent2D swapChainExtent = GetSwapChainExtent();

  _depthImages.resize(ImageCount());
  _depthImageMemorys.resize(ImageCount());
  _depthImageViews.resize(ImageCount());
  for (size_t i = 0; i < _depthImages.size(); i++) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width  = swapChainExtent.width;
    imageInfo.extent.height = swapChainExtent.height;
    imageInfo.extent.depth  = 1;
    imageInfo.mipLevels     = 1;
    imageInfo.arrayLayers   = 1;
    imageInfo.format        = _swapchainDepthFormat;
    imageInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags         = 0;

    _context->CreateImageWithInfo(
        imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _depthImages[i], _depthImageMemorys[i]
    );

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image                           = _depthImages[i];
    viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format                          = _swapchainDepthFormat;
    viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.baseMipLevel   = 0;
    viewInfo.subresourceRange.levelCount     = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount     = 1;

    if (vkCreateImageView(
            _context->GetLogicalDevice(), &viewInfo, nullptr, &_depthImageViews[i]
        )
        != VK_SUCCESS) {
      throw std::runtime_error("failed to create texture image view!");
    }
  }
}

void Swapchain::CreateFramebuffers() {
  _swapchainFramebuffers.resize(ImageCount());
  for (size_t i = 0; i < ImageCount(); i++) {
    std::array<VkImageView, 2> attachments = {_swapchainImageViews[i], _depthImageViews[i]};

    VkExtent2D swapChainExtent              = GetSwapChainExtent();
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass              = _renderPass;
    framebufferInfo.attachmentCount         = static_cast<u32>(attachments.size());
    framebufferInfo.pAttachments            = attachments.data();
    framebufferInfo.width                   = swapChainExtent.width;
    framebufferInfo.height                  = swapChainExtent.height;
    framebufferInfo.layers                  = 1;

    if (vkCreateFramebuffer(
            _context->GetLogicalDevice(), &framebufferInfo, nullptr,
            &_swapchainFramebuffers[i]
        )
        != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

void Swapchain::CreateSyncObjects() {
  _imageAvailableSemaphores.resize(MAX_FRAMES_SYNC);
  _renderFinishedSemaphores.resize(ImageCount());
  _inFlightFences.resize(MAX_FRAMES_SYNC);

  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags             = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_SYNC; ++i) {
    if (vkCreateSemaphore(
            _context->GetLogicalDevice(), &semaphoreInfo, nullptr,
            &_imageAvailableSemaphores[i]
        ) != VK_SUCCESS
        || vkCreateFence(
               _context->GetLogicalDevice(), &fenceInfo, nullptr, &_inFlightFences[i]
           ) != VK_SUCCESS) {
      throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
  }

  for (size_t i = 0; i < ImageCount(); ++i) {
    if (vkCreateSemaphore(
            _context->GetLogicalDevice(), &semaphoreInfo, nullptr,
            &_renderFinishedSemaphores[i]
        )
        != VK_SUCCESS) {
      throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
  }
}

VkResult Swapchain::AcquireNextImage(u32* imageIndex) {
  vkWaitForFences(
      _context->GetLogicalDevice(), 1, &_inFlightFences[_currentFrameIndex], VK_TRUE,
      std::numeric_limits<uint64_t>::max()
  );

  VkResult result = vkAcquireNextImageKHR(
      _context->GetLogicalDevice(), _swapchain, std::numeric_limits<uint64_t>::max(),
      _imageAvailableSemaphores[_currentFrameIndex], VK_NULL_HANDLE, imageIndex
  );

  return result;
}

VkResult Swapchain::SubmitCommandBuffers(const VkCommandBuffer* buffers, u32* imageIndex) {
  VkSubmitInfo submitInfo = {};
  submitInfo.sType        = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[]      = {_imageAvailableSemaphores[_currentFrameIndex]};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount     = 1;
  submitInfo.pWaitSemaphores        = waitSemaphores;
  submitInfo.pWaitDstStageMask      = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers    = buffers;

  VkSemaphore signalSemaphores[]  = {_renderFinishedSemaphores[*imageIndex]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores    = signalSemaphores;

  vkResetFences(_context->GetLogicalDevice(), 1, &_inFlightFences[_currentFrameIndex]);
  if (vkQueueSubmit(
          _context->GetGraphicsQueue(), 1, &submitInfo,
          _inFlightFences[_currentFrameIndex]
      )
      != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType            = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores    = signalSemaphores;

  VkSwapchainKHR swapChains[] = {_swapchain};
  presentInfo.swapchainCount  = 1;
  presentInfo.pSwapchains     = swapChains;

  presentInfo.pImageIndices = imageIndex;

  auto result = vkQueuePresentKHR(_context->GetPresentQueue(), &presentInfo);

  _currentFrameIndex = (_currentFrameIndex + 1) % MAX_FRAMES_SYNC;

  return result;
}

VkFormat Swapchain::FindDepthFormat() {
  return _context->FindSupportedFormat(
      {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
      VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
  );
}

VkSurfaceFormatKHR Swapchain::ChooseSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats
) {
  for (const auto& availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
        && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats[0];
}

VkPresentModeKHR Swapchain::ChoosePresentMode(
    const std::vector<VkPresentModeKHR>& availablePresentModes
) {
  for (const auto& availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      std::cout << "Present mode: Mailbox" << std::endl;
      return availablePresentMode;
    }
  }

  // for (const auto &availablePresentMode : availablePresentModes) {
  //   if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
  //     std::cout << "Present mode: Immediate" << std::endl;
  //     return availablePresentMode;
  //   }
  // }

  std::cout << "Present mode: V-Sync" << std::endl;
  return VK_PRESENT_MODE_FIFO_KHR;
}  // namespace VK

VkExtent2D Swapchain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
  /*if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {*/
    VkExtent2D actualExtent = {Config::WindowWidth, Config::WindowHeight};
    actualExtent.width      = std::max(
        capabilities.minImageExtent.width,
        std::min(capabilities.maxImageExtent.width, actualExtent.width)
    );
    actualExtent.height = std::max(
        capabilities.minImageExtent.height,
        std::min(capabilities.maxImageExtent.height, actualExtent.height)
    );

    return actualExtent;
  //}
}
}  // namespace VK