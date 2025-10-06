#pragma once

namespace Input {
static std::unordered_map<int, bool> KeyProcessed;
static std::unordered_map<int, u32> KeyHoldFrames;
static std::unordered_map<int, bool> MouseProcessed;
static std::unordered_map<int, u32> MouseHoldFrames;
}  // namespace Input