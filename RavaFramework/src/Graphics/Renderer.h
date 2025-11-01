#pragma once

namespace Rava {
class Renderer {
public:
  static Unique<Renderer> Instance;
  //static Unique<Context> Context;

public:
  virtual ~Renderer() = default;

  static bool Create();

  virtual void BeginFrame() = 0;
  virtual void EndFrame()   = 0;
  virtual void BeginSwapChainRenderPass() = 0;
  virtual void EndSwapChainRenderPass()   = 0;

  virtual void WaitDeviceIdle() = 0;
  //virtual int CreateModel(std::string filepath) = 0;

  virtual void* GetContext() const = 0;

  virtual bool IsInitialized() const { return _initialized; }

protected:
  bool _initialized = false;
};
}  // namespace Rava