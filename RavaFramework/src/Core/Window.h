#pragma once

namespace Rava {
class Window {
public:
  static Unique<Window> Instance;

public:
  Window();
  ~Window();

  NO_COPY(Window)

  static bool Create();

  void ShowWindow();

  bool ProcessMessage() const;

  inline void ResetResizedFlag() { _resized = false; }

  inline HWND GetWin32Window() const { return glfwGetWin32Window(_glfwWindow); }
  inline GLFWwindow* GetGLFWWindow() const { return _glfwWindow; }
  inline bool IsInitialized() const { return _initialized; }
  inline bool IsResized() const { return _resized; }

private:
  GLFWwindow* _glfwWindow;
  bool _initialized = false;
  bool _resized     = false;

private:
  static void ResizeCallback(GLFWwindow* window, int width, int height);
};
}  // namespace Rava