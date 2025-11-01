#pragma once

#include <ufbx/ufbx.h>

namespace Rava {
struct Vertex;
struct Mesh;
class ufbxLoader {
public:
  std::vector<u32> Indices{};
  std::vector<Vertex> Vertices{};
  std::vector<Mesh> Meshes{};

public:
  ufbxLoader() = delete;
  ufbxLoader(const std::string& filepath);

  bool LoadModel(const u32 instanceCount = 1);

private:
  std::string _filepath;
  std::string _path;

  ufbx_scene* _ufbxScene = nullptr;

private:
  void LoadNode(const ufbx_node* fbxNode);
  void LoadMesh(const ufbx_node* fbxNode, const u32 meshIndex);
};
}  // namespace Rava