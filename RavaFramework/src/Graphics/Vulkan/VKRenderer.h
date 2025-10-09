#pragma once

#include "Graphics/Renderer.h"

namespace VK {
class Context;
class Swapchain;
class Renderer : public Rava::Renderer {
public:
  // static Unique<DescriptorPool> GlobalDescriptorPool;
  //static Unique<Context> VKContext;

public:
  Renderer();
  virtual ~Renderer() override;

  NO_COPY(Renderer)

  virtual void BeginFrame() override;
  virtual void EndFrame() override;
  virtual void BeginSwapChainRenderPass() override;
  virtual void EndSwapChainRenderPass() override;

  virtual void WaitDeviceIdle() override;

  const Shared<Context> GetContext() const { return _context; }
  VkCommandBuffer GetCurrentCommandBuffer() const;

private:
  Shared<Context> _context;
  Unique<Swapchain> _swapchain;
  std::vector<VkCommandBuffer> _commandBuffers;
  VkCommandBuffer _currentCommandBuffer = VK_NULL_HANDLE;

  u32 _currentImageIndex;
  u32 _currentFrameIndex;
  //bool _isFrameStarted = false;

private:
  void RecreateSwapChain();
  //void RecreateRenderpass();
  void CreateCommandBuffers();
  void FreeCommandBuffers();
  //void Recreate();
};
}  // namespace VK