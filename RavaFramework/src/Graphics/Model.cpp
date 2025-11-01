#include "RavaFramework.h"

#include "Core/Config.h "

#include "Graphics/Model.h"
#include "Graphics/ModelLoader/ufbxLoader.h"
#include "Graphics/Vulkan/VKModel.h"

namespace Rava {
//Model Model::Create(std::string_view filepath) {
//  ufbxLoader loader{filepath.data()};
//  if (!loader.LoadModel()) {
//    std::print("Failed to load Model file {0}", filepath.data());
//  }
//
//  switch (Config::SelectedAPI) {
//    case RendererAPI::Vulkan:
//      return VK::Model(loader);
//    default:
//      return Model();
//  }
//}
}  // namespace Rava