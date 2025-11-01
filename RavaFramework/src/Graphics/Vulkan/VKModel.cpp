//#include "RavaFramework.h"
//
//#include "Graphics/ModelLoader/ufbxLoader.h"
//#include "Graphics/Vulkan/VKModel.h"
//
//namespace VK {
//Model::Model(std::string filepath) {
//  Rava::ufbxLoader loader{filepath.data()};
//  loader.LoadModel();
//
//  CreateVertexBuffers(loader.Vertices);
//  CreateIndexBuffers(loader.Indices);
//}
//
//void Model::CreateVertexBuffers(const std::vector<Rava::Vertex>& vertices) {
//  _vertexCount            = static_cast<u32>(vertices.size());
//  VkDeviceSize bufferSize = sizeof(vertices[0]) * _vertexCount;
//  u32 vertexSize          = sizeof(vertices[0]);
//
//  Buffer stagingBuffer{
//      vertexSize, _vertexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
//  };
//
//  stagingBuffer.Map();
//  stagingBuffer.WriteToBuffer((void*)vertices.data());
//
//  _vertexBuffer = std::make_unique<Buffer>(
//      vertexSize, _vertexCount,
//      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
//      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
//  );
//
//  CopyBuffer();
//}
//}  // namespace VK