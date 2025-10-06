#include "RavaFramework.h"

#include "Core/Config.h"

#include "Graphics/Renderer.h"
#include "Graphics/Vulkan/VKRenderer.h"

namespace Rava {
Unique<Renderer> Renderer::Instance = nullptr;

bool Renderer::Create() {
  Unique<Renderer> renderer;

  switch (Config::SelectedAPI) {
    case RendererAPI::Vulkan:
      Instance = std::make_unique<VK::Renderer>();
      return Instance->IsInitialized();
    default:
      return false;
  }
}
}