#pragma once

namespace Rava {
class Window;
//class Renderer;
class Application {
public:
  static Unique<Application> Instance;

public:
  Application(u32 width, u32 height, std::string_view title);
  ~Application();

  static void Create(u32 width, u32 height, std::string_view title);

  bool ProcessMessage();
  void BeginFrame();
  void EndFrame();

  Window* GetWindow() { return _window.get(); }
  //Renderer* GetRenderer() { return _renderer.get(); }

private:
  std::unique_ptr<Window> _window;
  //std::unique_ptr<Renderer> _renderer;

  u32 _width, _height;
  std::string_view _title;
  bool isFullscreen = false;
};
}  // namespace Rava