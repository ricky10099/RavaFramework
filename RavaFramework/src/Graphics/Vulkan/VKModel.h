//#pragma once
//
//#include "Graphics/Model.h"
//
//namespace Rava {
//class ufbxLoader;
//}
//
//namespace VK {
//struct Mesh;
//class Buffer;
//struct Vertex : public Rava::Vertex {
//  static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
//  static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
//};
//
//class Model/* : public Rava::Model */{
//public:
//  Model(std::string filepath);
//  ~Model();
//
//  NO_COPY(Model)
//
//  //void Draw() override;
//
//  const std::vector<Vertex> GetVertices() { return _vertices; }
//  const std::vector<u32> GetIndices() { return _indices; }
//
//private:
//  std::vector<Mesh> _meshes{};
//  std::vector<Vertex> _vertices;
//  std::vector<u32> _indices;
//
//  Unique<Buffer> _vertexBuffer;
//  u32 _vertexCount;
//
//  bool _hasIndexBuffer = false;
//  Unique<Buffer> _indexBuffer;
//  u32 _indexCount;
//
//private:
//  void CopyMeshes(const std::vector<Mesh>& meshes);
//
//  void CreateVertexBuffers(const std::vector<Rava::Vertex>& vertices);
//  void CreateIndexBuffers(const std::vector<u32>& indices);
//
//  void BindDescriptors(
//      /*const FrameInfo& frameInfo,*/ const VkPipelineLayout& pipelineLayout, Mesh& mesh
//  );
//};
//}  // namespace VK