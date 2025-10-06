#include "RavaFramework.h"

#include "Graphics/Context.h"
#include "Core/Config.h"

#include "Graphics/Vulkan/VKContext.h"

namespace Rava {
Unique<Context> Context::Instance = nullptr;

bool Context::Create() {
  switch (Config::SelectedAPI) {
    case RendererAPI::Vulkan:
      //Instance = std::make_unique<VK::Context>();
      //return Instance->IsInitialized();
      return false;
    case RendererAPI::DirectX11:
      // Instance = std::make_unique<DX11::Context>();
      // return Instance->IsInitialized();
      return false;
    case RendererAPI::DirectX12:
      // Instance = std::make_unique<DX12::Context>();
      // return Instance->IsInitialized();
      return false;
    case RendererAPI::OpenGL:
      // Instance = std::make_unique<GL::Context>();
      // return Instance->IsInitialized();
      return false;
    default:
      return false;
  }
}
}  // namespace Rava