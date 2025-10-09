#pragma once

namespace Config {
extern RendererAPI SelectedAPI;
extern u32 WindowWidth;
extern u32 WindowHeight;
extern std::string_view WindowTitle;
extern bool IsFullscreen;
extern bool IsResizeable;
extern Color ClearColor;
}  // namespace Config