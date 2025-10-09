#include "RavaFramework.h"

#include "Core/Config.h"
#include "Graphics/Vulkan/VKValidation.h"

#include "Core/Window.h"
#include "Graphics/Context.h"
#include "Graphics/Vulkan/VKContext.h"
#include "Graphics/Vulkan/VKRenderer.h"
#include "Graphics/Vulkan/VKSwapchain.h"
#include "Graphics/Vulkan/VKUtils.h"

namespace VK {
// Unique<Context> Renderer::VKContext = nullptr;

Renderer::Renderer() {
  if (_context == nullptr) {
    _context = std::make_shared<Context>();
  }
  _initialized = _context->IsInitialized();
  RecreateSwapChain();
  // RecreateRenderpass();
  CreateCommandBuffers();
}

Renderer::~Renderer() {
  _swapchain.reset();
  std::print("~Renderer");
  FreeCommandBuffers();
  // _context = nullptr;
  _context.reset();
}

void Renderer::RecreateSwapChain() {
  std::print("WindowWidth: {}", Config::WindowWidth);
  VkExtent2D extent = {Config::WindowWidth, Config::WindowHeight};
  while (extent.width == 0 || extent.height == 0) {
    extent = {Config::WindowWidth, Config::WindowHeight};
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(_context->GetLogicalDevice());

  if (_swapchain == nullptr) {
    _swapchain = std::make_unique<Swapchain>(_context);
  } else {
    // std::print("recreating swapchain at frame {0}", _frameCounter);
    Shared<Swapchain> oldSwapChain = std::move(_swapchain);
    _swapchain                     = std::make_unique<Swapchain>(_context, oldSwapChain);
    if (!oldSwapChain->CompareSwapFormats(*_swapchain.get())) {
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
  // assert(!m_frameInProgress,  "Can't Call BeginFrame while already in progress!");

  auto result = _swapchain->AcquireNextImage(&_currentImageIndex);
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

  result = vkBeginCommandBuffer(_currentCommandBuffer, &beginInfo);
  IsResultValid(result, "Failed to Begin Recording Command Buffer!");

  //_currentCommandBuffer = commandBuffer;
  //// return commandBuffer;
  // if (_currentCommandBuffer) {
  //   _frameInfo
  //       = {m_currentFrameIndex, 0.0f, /* m_FrameTime */
  //          m_currentCommandBuffer,

  //         m_globalDescriptorSets[m_currentFrameIndex]};
  //}
}

void Renderer::EndFrame() {
  //_currentCommandBuffer = GetCurrentCommandBuffer();
  if (vkEndCommandBuffer(_currentCommandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }

  auto result = _swapchain->SubmitCommandBuffers(&_currentCommandBuffer, &_currentImageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR
      || Rava::Window::Instance->IsResized()) {
    Rava::Window::Instance->ResetResizedFlag();
    RecreateSwapChain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }

  // isFrameStarted    = false;
  _currentFrameIndex = (_currentFrameIndex + 1) % MAX_FRAMES_SYNC;
}

void Renderer::BeginSwapChainRenderPass() {
  // assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
  // assert(
  //     commandBuffer == getCurrentCommandBuffer()
  //     && "Can't begin render pass on command buffer from a different frame"
  //);

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass  = _swapchain->GetRenderPass();
  renderPassInfo.framebuffer = _swapchain->GetFrameBuffer(_currentImageIndex);

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = _swapchain->GetSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color
      = {Config::ClearColor.r, Config::ClearColor.g, Config::ClearColor.b, Config::ClearColor.a};
  clearValues[1].depthStencil    = {1.0f, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues    = clearValues.data();

  vkCmdBeginRenderPass(_currentCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  viewport.x        = 0.0f;
  viewport.y        = 0.0f;
  viewport.width    = static_cast<float>(_swapchain->GetSwapChainExtent().width);
  viewport.height   = static_cast<float>(_swapchain->GetSwapChainExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  VkRect2D scissor{
      {0, 0},
      _swapchain->GetSwapChainExtent()
  };
  vkCmdSetViewport(_currentCommandBuffer, 0, 1, &viewport);
  vkCmdSetScissor(_currentCommandBuffer, 0, 1, &scissor);
}

void Renderer::EndSwapChainRenderPass() {
  // assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
  // assert(
  //     commandBuffer == getCurrentCommandBuffer()
  //     && "Can't end render pass on command buffer from a different frame"
  //);
  vkCmdEndRenderPass(_currentCommandBuffer);
}

void Renderer::WaitDeviceIdle() {
  vkDeviceWaitIdle(_context->GetLogicalDevice());
}

VkCommandBuffer Renderer::GetCurrentCommandBuffer() const {
  return _commandBuffers[_swapchain->GetCurrentFrameIndex()];
}
}  // namespace VK