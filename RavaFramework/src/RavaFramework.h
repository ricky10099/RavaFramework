#pragma once

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

////////////////////////////////////////////////////////////////////////
// std
////////////////////////////////////////////////////////////////////////
#include <shobjidl.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <print>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
using namespace std::chrono_literals;

////////////////////////////////////////////////////////////////////////
// GLFW
////////////////////////////////////////////////////////////////////////
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

////////////////////////////////////////////////////////////////////////
// stb_image
////////////////////////////////////////////////////////////////////////
#define STB_IMAGE_IMPLEMENTATION

////////////////////////////////////////////////////////////////////////
// glm
////////////////////////////////////////////////////////////////////////
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

////////////////////////////////////////////////////////////////////////
// Macros
////////////////////////////////////////////////////////////////////////
#define NO_COPY(type)                    \
  type(const type&)            = delete; \
  type& operator=(const type&) = delete;
#define NO_MOVE(type)               \
  type(type&&)            = delete; \
  type& operator=(type&&) = delete;

////////////////////////////////////////////////////////////////////////
// Typedef
////////////////////////////////////////////////////////////////////////
using i8  = char;
using i16 = short;
using i32 = int;
using i64 = long long;

using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using f32 = float;
using f64 = double;

using Vec2  = glm::vec2;
using Vec3  = glm::vec3;
using Vec4  = glm::vec4;
using Mat3  = glm::mat3;
using Mat4  = glm::mat4;
using Quat  = glm::quat;
using Color = glm::vec4;  // r,g,b,a

template <typename T>
using Unique = std::unique_ptr<T>;

template <typename T>
using Shared = std::shared_ptr<T>;

template <typename T>
using Weak = std::weak_ptr<T>;

enum class RendererAPI {
  Vulkan,
  DirectX11,
  DirectX12,
  OpenGL,
};

namespace Rava {
// Initialization / Shutdown
extern void SetClearColor(f32 r, f32 g, f32 b, f32 a);
extern void SetClearColor(Color color);
extern void SetFullscreen(bool isFullscreen);
extern void SetResizeable(bool isResizable);
extern void SetRendererAPI(RendererAPI api);
extern bool InitFramework(u32 width, u32 height);
extern bool InitFramework(u32 width, u32 height, std::string_view title);
extern void ShutdownFramework();

// Main loop helpers
extern bool ProcessMessage();  // handle window events
extern void BeginFrame();
extern void EndFrame();

//// Simple draw API (expand later)
// void DrawTriangle();
// void DrawQuad(float x, float y, float w, float h);

}  // namespace Rava

namespace Input {
enum class KeyCode {
  // From glfw3.h
  Space      = 32,
  Apostrophe = 39, /* ' */
  Comma      = 44, /* , */
  Minus      = 45, /* - */
  Period     = 46, /* . */
  Slash      = 47, /* / */

  D0 = 48, /* 0 */
  D1 = 49, /* 1 */
  D2 = 50, /* 2 */
  D3 = 51, /* 3 */
  D4 = 52, /* 4 */
  D5 = 53, /* 5 */
  D6 = 54, /* 6 */
  D7 = 55, /* 7 */
  D8 = 56, /* 8 */
  D9 = 57, /* 9 */

  Semicolon = 59, /* ; */
  Equal     = 61, /* = */

  A = 65,
  B = 66,
  C = 67,
  D = 68,
  E = 69,
  F = 70,
  G = 71,
  H = 72,
  I = 73,
  J = 74,
  K = 75,
  L = 76,
  M = 77,
  N = 78,
  O = 79,
  P = 80,
  Q = 81,
  R = 82,
  S = 83,
  T = 84,
  U = 85,
  V = 86,
  W = 87,
  X = 88,
  Y = 89,
  Z = 90,

  LeftBracket  = 91, /* [ */
  Backslash    = 92, /* \ */
  RightBracket = 93, /* ] */
  GraveAccent  = 96, /* ` */

  /* Function keys */
  Escape      = 256,
  Enter       = 257,
  Tab         = 258,
  Backspace   = 259,
  Insert      = 260,
  Delete      = 261,
  Right       = 262,
  Left        = 263,
  Down        = 264,
  Up          = 265,
  PageUp      = 266,
  PageDown    = 267,
  Home        = 268,
  End         = 269,
  CapsLock    = 280,
  ScrollLock  = 281,
  NumLock     = 282,
  PrintScreen = 283,
  Pause       = 284,
  F1          = 290,
  F2          = 291,
  F3          = 292,
  F4          = 293,
  F5          = 294,
  F6          = 295,
  F7          = 296,
  F8          = 297,
  F9          = 298,
  F10         = 299,
  F11         = 300,
  F12         = 301,
  F13         = 302,
  F14         = 303,
  F15         = 304,
  F16         = 305,
  F17         = 306,
  F18         = 307,
  F19         = 308,
  F20         = 309,
  F21         = 310,
  F22         = 311,
  F23         = 312,
  F24         = 313,
  F25         = 314,

  /* Keypad */
  KP0        = 320,
  KP1        = 321,
  KP2        = 322,
  KP3        = 323,
  KP4        = 324,
  KP5        = 325,
  KP6        = 326,
  KP7        = 327,
  KP8        = 328,
  KP9        = 329,
  KPDecimal  = 330,
  KPDivide   = 331,
  KPMultiply = 332,
  KPSubtract = 333,
  KPAdd      = 334,
  KPEnter    = 335,
  KPEqual    = 336,

  LeftShift    = 340,
  LeftControl  = 341,
  LeftAlt      = 342,
  LeftSuper    = 343,
  RightShift   = 344,
  RightControl = 345,
  RightAlt     = 346,
  RightSuper   = 347,
  Menu         = 348,
};

enum class Mouse {
  // From glfw3.h
  Button0 = 0,
  Button1 = 1,
  Button2 = 2,
  Button3 = 3,
  Button4 = 4,
  Button5 = 5,
  Button6 = 6,
  Button7 = 7,

  ButtonLast   = Button7,
  ButtonLeft   = Button0,
  ButtonRight  = Button1,
  ButtonMiddle = Button2
};

enum class Joystick {
  // From glfw3.h
  Joystick1    = 0,
  Joystick2    = 1,
  Joystick3    = 2,
  Joystick4    = 3,
  Joystick5    = 4,
  Joystick6    = 5,
  Joystick7    = 6,
  Joystick8    = 7,
  Joystick9    = 8,
  Joystick10   = 9,
  Joystick11   = 10,
  Joystick12   = 11,
  Joystick13   = 12,
  Joystick14   = 13,
  Joystick15   = 14,
  Joystick16   = 15,
  JoystickLast = Joystick16
};

enum class JoystickHat {
  // From glfw3.h
  Centered  = 0,
  Up        = 1,
  Right     = 2,
  Down      = 4,
  Left      = 8,
  RightUp   = (Right | Up),
  RightDown = (Right | Down),
  LeftUp    = (Left | Up),
  LeftDown  = (Left | Down)
};

enum class GamepadButton {
  // From glfw3.h
  Button1           = 0,
  Button2           = 1,
  Button3           = 2,
  Button4           = 3,
  ButtonLeftBumper  = 4,
  ButtonRightBumper = 5,
  ButtonBack        = 6,
  ButtonStart       = 7,
  ButtonGuide       = 8,
  ButtonLeftStick   = 9,
  ButtonRightStick  = 10,
  ButtonDpadUp      = 11,
  ButtonDpadRight   = 12,
  ButtonDpadDown    = 13,
  ButtonDpadLeft    = 14,
  ButtonLast        = ButtonDpadLeft,

  ButtonA = Button1,
  ButtonB = Button2,
  ButtonX = Button3,
  ButtonY = Button4,

  ButtonCross    = Button1,
  ButtonCircle   = Button2,
  ButtonSquare   = Button3,
  ButtonTriangle = Button4,
};

enum class GamepadAxis {
  // From glfw3.h
  AxisLeftX        = 0,
  AxisLeftY        = 1,
  AxisRightX       = 2,
  AxisRightY       = 3,
  AxisLeftTrigger  = 4,
  AxisRightTrigger = 5,
  AxisLast         = AxisRightTrigger
};

extern bool IsKeyPressed(KeyCode key);
extern bool IsKeyDown(KeyCode key);
extern bool IsKeyRepeat(KeyCode key, u32 frameCount = 2);
extern bool IsKeyReleased(KeyCode key);
extern bool IsMousePressed(Mouse button = Mouse::Button0);
extern bool IsMouseDown(Mouse button = Mouse::Button0);
extern bool IsMouseRepeat(Mouse button = Mouse::Button0, u32 frameCount = 2);
extern bool IsMouseReleased(Mouse button = Mouse::Button0);
extern Vec2 GetMousePosition();
extern float GetMouseX();
extern float GetMouseY();
}  // namespace Input