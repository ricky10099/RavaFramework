#include "RavaFramework.h"

#include "Core/Config.h"
#include "Graphics/Vulkan/VKValidation.h"

#include "Graphics/Context.h"
#include "Graphics/Vulkan/VKContext.h"
#include "Graphics/Vulkan/VKRenderer.h"

namespace VK {
Renderer::Renderer() {
  _initialized = _context.IsInitialized();
  RecreateSwapChain();
  // RecreateRenderpass();
  CreateCommandBuffers();
}

void Renderer::RecreateSwapChain() {
  VkExtent2D extent = {Rava::Config::WindowWidth, Rava::Config::WindowHeight};
  while (extent.width == 0 || extent.height == 0) {
    extent = {Rava::Config::WindowWidth, Rava::Config::WindowHeight};
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(_context->GetLogicalDevice());

  if (_swapChain == nullptr) {
    _swapChain = std::make_unique<SwapChain>(extent);
  } else {
    // std::print("recreating swapchain at frame {0}", _frameCounter);
    std::shared_ptr<SwapChain> oldSwapChain = std::move(_swapChain);
    _swapChain                              = std::make_unique<SwapChain>(extent, oldSwapChain);
    if (!oldSwapChain->CompareSwapFormats(*_swapChain.get())) {
      std::print("swap chain image or depth format has changed");
    }
  }
}

void Renderer::CreateCommandBuffers() {
  _commandBuffers.resize(MAX_FRAMES_SYNC);
  VkCommandBufferAllocateInfo allocateInfo{};
  allocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocateInfo.commandPool        = _context->GetCommandPool();
  allocateInfo.commandBufferCount = static_cast<u32>(_commandBuffers.size());

  VkResult result = vkAllocateCommandBuffers(
      _context->GetLogicalDevice(), &allocateInfo, _commandBuffers.data()
  );

  _initialized = IsResultValid(result, "Failed to Allocate Command Buffers!");
}

void Renderer::FreeCommandBuffers() {
  vkFreeCommandBuffers(
      _context->GetLogicalDevice(), _context->GetCommandPool(),
      static_cast<u32>(_commandBuffers.size()), _commandBuffers.data()
  );
  _commandBuffers.clear();
}

void Renderer::BeginFrame() {
  //assert(!m_frameInProgress,  "Can't Call BeginFrame while already in progress!");

  auto result = _swapChain->AcquireNextImage(&_currentImageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    RecreateSwapChain();
    // return nullptr;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    std::print("Failed to Acquire Swap Chain Image!");
  }

  //_frameInProgress = true;
  //_frameCounter++;

  _currentCommandBuffer = GetCurrentCommandBuffer();
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
  VK_CHECK(result, "Failed to Begin Recording Command Buffer!")

  m_currentCommandBuffer = commandBuffer;
  // return commandBuffer;
  if (m_currentCommandBuffer) {
    m_frameInfo
        = {m_currentFrameIndex, 0.0f, /* m_FrameTime */
           m_currentCommandBuffer,

           m_globalDescriptorSets[m_currentFrameIndex]};
  }

  m_editor->NewFrame();
}
}  // namespace VK