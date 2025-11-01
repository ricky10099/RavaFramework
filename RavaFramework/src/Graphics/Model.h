#pragma once

namespace Rava {
struct Vertex {
  glm::vec3 Position;
  glm::vec3 Color;
  glm::vec3 Normal;
  glm::vec2 UV;
  bool operator==(const Vertex& other) const {
    return Position == other.Position && Color == other.Color && Normal == other.Normal
        && UV == other.UV;
  }
};

struct Mesh {
  u32 FirstVertex;
  u32 VertexCount;  
  u32 FirstIndex;
  u32 IndexCount;
};

//class Model {
//public:
//  static Model Create(std::string_view file);
//
//  virtual void Draw() {}
//};
}  // namespace Rava