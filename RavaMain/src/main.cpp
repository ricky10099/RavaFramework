#include "RavaFramework.h"
#include <print>

int main() {
  Rava::SetClearColor(0.3f, 0.4f, 0.7f, 1.0f);
  Rava::InitFramework(1440, 720, "test");

  Vec2 prevMousePos = Input::GetMousePosition();

  while (Rava::ProcessMessage()) {
    Rava::BeginFrame();

    if (Input::IsKeyPressed(Input::KeyCode::Escape)) {
      break;
    }

    Vec2 currMousePos = Input::GetMousePosition();
    if (currMousePos != prevMousePos) {
      //std::print("MousePosition: x:{}, y{}\n", currMousePos.x, currMousePos.y);
      prevMousePos = currMousePos;
    }

    Rava::EndFrame();
  }

  Rava::ShutdownFramework();
}