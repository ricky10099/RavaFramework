#include "RavaFramework.h"

#include "Core/Config.h"
#include "Core/Window.h"

namespace Rava {
Unique<Window> Window::Instance = nullptr;

Window::Window() : _glfwWindow(nullptr) {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, Config::IsResizeable);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

  GLFWmonitor* primaryMonitor  = glfwGetPrimaryMonitor();
  const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);

  if (Config::IsFullscreen) {
    _glfwWindow = glfwCreateWindow(
        videoMode->width, videoMode->height, Config::WindowTitle.data(), primaryMonitor, nullptr
    );

  } else {
    int monitorX, monitorY;
    glfwGetMonitorPos(primaryMonitor, &monitorX, &monitorY);
    _glfwWindow
        = glfwCreateWindow(Config::WindowWidth, Config::WindowHeight, "test", nullptr, nullptr);
    glfwSetWindowPos(
        _glfwWindow, monitorX + (videoMode->width - Config::WindowWidth) / 2,
        monitorY + (videoMode->height - Config::WindowHeight) / 2
    );
    glfwShowWindow(_glfwWindow);
  }

  if (!glfwVulkanSupported()) {
    std::print("GLFW: Vulkan not supported!");
  }

  glfwSetWindowUserPointer(_glfwWindow, this);
  glfwSetFramebufferSizeCallback(_glfwWindow, ResizeCallback);

  _initialized = true;
}

Window::~Window() {
  glfwDestroyWindow(_glfwWindow);
  glfwTerminate();
  Instance.release();
}

bool Window::Create() {
  if (Instance == nullptr) {
    Instance = std::make_unique<Window>();
    return Instance->IsInitialized();
  }

  return false;
}

void Window::ShowWindow() {
  glfwShowWindow(_glfwWindow);
}

bool Window::ProcessMessage() const {
  glfwPollEvents();
  return !glfwWindowShouldClose(_glfwWindow);
}

void Window::ResizeCallback(GLFWwindow* glfwWindow, int width, int height) {
  //auto window          = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
  //window->_resized     = true;
  Config::WindowWidth  = width;
  Config::WindowHeight = height;
}
}  // namespace Rava