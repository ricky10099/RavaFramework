#include "RavaFramework.h"

#include "Core/Config.h "

#include "Graphics/Model.h"
#include "Graphics/ModelLoader/ufbxLoader.h"
#include "Graphics/Vulkan/VKModel.h"

namespace Rava {
Unique<Model> Model::Create(std::string_view filepath) {
  ufbxLoader loader{filepath.data()};
  if (!loader.LoadModel()) {
    std::print("Failed to load Model file {0}", filepath.data());
    return nullptr;
  }

  Unique<Model> model;
  switch (Config::SelectedAPI) {
    case RendererAPI::Vulkan:
      // model = std::make_shared<VK::Renderer>();
       return std::make_unique<VK::Model>(loader);
    default:
      model = nullptr;
  }
}
}  // namespace Rava