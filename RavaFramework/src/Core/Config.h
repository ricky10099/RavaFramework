#pragma once

namespace Rava {
namespace Config {
static RendererAPI SelectedAPI      = RendererAPI::Vulkan;
static u32 WindowWidth              = 1280;
static u32 WindowHeight             = 720;
static std::string_view WindowTitle = "RavaFramework";
static bool IsFullscreen            = false;
static bool IsResizeable            = true;
static Color ClearColor             = {0.3f, 0.3f, 0.3f, 1.0f};
}  // namespace Config
}  // namespace Rava