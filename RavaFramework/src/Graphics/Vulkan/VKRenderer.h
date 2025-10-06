#pragma once

#include "Graphics/Renderer.h"

namespace VK {
class Context;
class SwapChain;
class Renderer : public Rava::Renderer {
public:
  // static Unique<DescriptorPool> GlobalDescriptorPool;

public:
  Renderer();
  virtual ~Renderer() override;

  NO_COPY(Renderer)

  virtual void BeginFrame() override;
  virtual void EndFrame() override;
  void BeginSwapChainRenderPass();
  void EndSwapChainRenderPass();


  const Context* GetContext() { return _context; }
  VkCommandBuffer GetCurrentCommandBuffer() const;

private:
  Context* _context;
  Unique<SwapChain> _swapChain;
  std::vector<VkCommandBuffer> _commandBuffers;
  VkCommandBuffer _currentCommandBuffer = VK_NULL_HANDLE;

  u32 _currentImageIndex;
  u32 _currentFrameIndex;
  bool _isFrameStarted = false;

private:
  void RecreateSwapChain();
  //void RecreateRenderpass();
  void CreateCommandBuffers();
  void FreeCommandBuffers();
  //void Recreate();
};
}  // namespace VK