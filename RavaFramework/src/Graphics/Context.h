#pragma once

namespace Rava {
class Context {
public:
  static Unique<Context> Instance;

public:
  virtual ~Context() = default;


  static bool Create();

  //virtual void BeginFrame() = 0;
  //virtual void EndFrame()   = 0;

  //virtual void DrawTriangle()                               = 0;
  //virtual void DrawQuad(float x, float y, float w, float h) = 0;

  virtual bool IsInitialized() const { return _initialized; }

protected:
  bool _initialized = false;

};
}  // namespace RVF