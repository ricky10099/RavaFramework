#include "RavaFramework.h"
#include <print>

int main() {
  Rava::InitFramework(1280, 720, "test");

  Vec2 prevMousePos = Input::GetMousePosition();

  while (Rava::ProcessMessage()) {
    if (Input::IsKeyPressed(Input::KeyCode::Escape)) {
      break;
    }

    Vec2 currMousePos = Input::GetMousePosition();
    if (currMousePos != prevMousePos) {
      std::print("MousePosition: x:{}, y{}\n", currMousePos.x, currMousePos.y);
      prevMousePos = currMousePos;
    }
  }

  Rava::ShutdownFramework();
}