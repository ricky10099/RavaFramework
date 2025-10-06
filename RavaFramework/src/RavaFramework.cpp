#include "RavaFramework.h"

#include "Core/Config.h"
#include "Core/Input.h"
#include "Core/Window.h"

#include "Graphics/Context.h"
#include "Graphics/Renderer.h"

namespace Rava {
bool InitFramework(u32 width, u32 height, std::string_view title) {
  Config::WindowWidth  = width;
  Config::WindowHeight = height;
  Config::WindowTitle  = title;

  return Window::Create()/* && Context::Create()*/ && Renderer::Create();
}

void ShutdownFramework() {
  std::print("Shutdown");
}

void SetClearColor(f32 r, f32 g, f32 b, f32 a) {
  Config::ClearColor = {r, g, b, a};
}

void SetClearColor(Color color) {
  SetClearColor(color.r, color.g, color.b, color.a);
}

void SetFullscreen(bool isFullscreen) {
  Config::IsFullscreen = isFullscreen;
}

void SetResizeable(bool isResizable) {
  Config::IsResizeable = isResizable;
}

void SetRendererAPI(RendererAPI api) {
  Config::SelectedAPI = api;
}

bool ProcessMessage() {
  return Window::Instance->ProcessMessage();
}
}  // namespace Rava

namespace Input {
bool IsKeyPressed(KeyCode key) {
  auto* window = Rava::Window::Instance->GetGLFWWindow();
  auto state   = glfwGetKey(window, static_cast<int>(key));
  return state == GLFW_PRESS;
}

bool IsKeyDown(KeyCode key) {
  auto* window = Rava::Window::Instance->GetGLFWWindow();
  auto keyCode = static_cast<int>(key);
  auto state   = glfwGetKey(window, keyCode);

  if (state == GLFW_PRESS && !KeyProcessed[keyCode]) {
    KeyProcessed[keyCode] = true;
    return true;
  }

  if (state == GLFW_RELEASE) {
    KeyProcessed[keyCode] = false;
  }

  return false;
}

bool IsKeyRepeat(KeyCode key, u32 frameCount) {
  auto* window = Rava::Window::Instance->GetGLFWWindow();
  auto keyInt  = static_cast<int>(key);
  auto state   = glfwGetKey(window, keyInt);

  if (state == GLFW_PRESS) {
    KeyHoldFrames[keyInt]++;
    if (KeyHoldFrames[keyInt] >= frameCount) {
      return true;
    }
  } else {
    KeyHoldFrames[keyInt] = 0;
  }

  return false;
}

bool IsKeyReleased(KeyCode key) {
  auto* window = Rava::Window::Instance->GetGLFWWindow();
  auto state   = glfwGetKey(window, static_cast<int>(key));
  return state == GLFW_RELEASE;
}

bool IsMousePressed(Mouse button) {
  auto* window = Rava::Window::Instance->GetGLFWWindow();
  auto state   = glfwGetMouseButton(window, static_cast<int>(button));
  return state == GLFW_PRESS;
}

bool IsMouseDown(Mouse button) {
  auto* window     = Rava::Window::Instance->GetGLFWWindow();
  auto mouseButton = static_cast<int>(button);
  auto state       = glfwGetMouseButton(window, mouseButton);

  if (state == GLFW_PRESS && !MouseProcessed[mouseButton]) {
    MouseProcessed[mouseButton] = true;
    return true;
  }

  if (state == GLFW_RELEASE) {
    MouseProcessed[mouseButton] = false;
  }

  return false;
}

bool IsMouseRepeat(Mouse button, u32 frameCount) {
  auto* window     = Rava::Window::Instance->GetGLFWWindow();
  auto mouseButton = static_cast<int>(button);
  auto state       = glfwGetMouseButton(window, mouseButton);

  if (state == GLFW_PRESS) {
    MouseHoldFrames[mouseButton]++;
    if (MouseHoldFrames[mouseButton] >= frameCount) {
      return true;
    }
  } else {
    MouseHoldFrames[mouseButton] = 0;
  }

  return false;
}

bool IsMouseReleased(Mouse button) {
  auto* window = Rava::Window::Instance->GetGLFWWindow();
  auto state   = glfwGetMouseButton(window, static_cast<int>(button));
  return state == GLFW_RELEASE;
}

Vec2 GetMousePosition() {
  auto* window = Rava::Window::Instance->GetGLFWWindow();
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  return {(float)xpos, (float)ypos};
}

float GetMouseX() {
  return GetMousePosition().x;
}

float GetMouseY() {
  return GetMousePosition().y;
}
}  // namespace Input