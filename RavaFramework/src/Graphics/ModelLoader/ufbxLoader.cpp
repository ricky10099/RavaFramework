#include "RavaFramework.h"

//#include <ufbx/ufbx.c>
#include "Core/Utils.h"

#include "Graphics/Model.h"
#include "Graphics/ModelLoader/ufbxLoader.h"

namespace Rava {
ufbxLoader::ufbxLoader(const std::string& filepath) : _filepath(filepath) {
  _path = GetPathWithoutFileName(filepath);
}

bool ufbxLoader::LoadModel(const u32 instanceCount) {
  ufbx_load_opts loadOptions{};
  loadOptions.ignore_animation              = true;
  loadOptions.load_external_files           = true;
  loadOptions.ignore_missing_external_files = true;
  loadOptions.generate_missing_normals      = true;
  loadOptions.target_axes                   = ufbx_axes_left_handed_y_up;
  loadOptions.target_unit_meters            = 1.0f;

  ufbx_error ufbxError;

  _ufbxScene = ufbx_load_file(_filepath.data(), &loadOptions, &ufbxError);

  if (_ufbxScene == nullptr) {
    char errorBuffer[512];
    ufbx_format_error(errorBuffer, sizeof(errorBuffer), &ufbxError);
    std::print("ufbxLoader::Load error: file: {0}, error: {1}", _filepath, errorBuffer);
    return false;
  }

  if (!_ufbxScene->meshes.count) {
    std::print("ufbxBuilder::Load: no meshes found in {0}", _filepath);
    return false;
  }

  LoadNode(_ufbxScene->root_node);

  ufbx_free_scene(_ufbxScene);
  return true;
}

void ufbxLoader::LoadNode(const ufbx_node* fbxNode) {
  ufbx_mesh* fbxMesh = fbxNode->mesh;
  if (fbxMesh) {
    u32 meshCount = static_cast<u32>(fbxMesh->material_parts.count);
    if (meshCount > 0) {
      Vertices.clear();
      Indices.clear();
      Meshes.clear();

      Meshes.resize(meshCount);
      for (u32 meshIndex = 0; meshIndex < meshCount; ++meshIndex) {
        LoadMesh(fbxNode, meshIndex);
      }
    }
  }

  u32 childCount = static_cast<u32>(fbxNode->children.count);
  for (u32 childIndex = 0; childIndex < childCount; ++childIndex) {
    LoadNode(fbxNode->children[childIndex]);
  }
}

void ufbxLoader::LoadMesh(const ufbx_node* fbxNode, const u32 meshIndex) {
  ufbx_mesh* fbxMesh                = fbxNode->mesh;
  const ufbx_mesh_part& fbxMeshPart = fbxNode->mesh->material_parts[meshIndex];
  size_t faceCount                  = fbxMeshPart.num_faces;

  if (!fbxMeshPart.num_triangles) {
    std::print("ufbxLoader::LoadMesh: only triangle meshes are supported!");
    return;
  }

  size_t numVerticesBefore = Vertices.size();
  size_t numIndicesBefore  = Indices.size();

  Mesh& mesh       = Meshes[meshIndex];
  mesh.FirstVertex = static_cast<u32>(numVerticesBefore);
  mesh.FirstIndex  = static_cast<u32>(numIndicesBefore);
  mesh.IndexCount  = 0;

  ufbx_material_map& baseColorMap = fbxNode->materials[meshIndex]->pbr.base_color;
  glm::vec4 diffuseColor          = baseColorMap.has_value
                                      ? glm::vec4(
                                   baseColorMap.value_vec4.x, baseColorMap.value_vec4.y,
                                   baseColorMap.value_vec4.z, baseColorMap.value_vec4.w
                               )
                                      : glm::vec4(1.0f);

#pragma region Vertices
  bool hasUVs                 = fbxMesh->uv_sets.count;
  bool hasVertexColors        = fbxMesh->vertex_color.exists;

  for (size_t fbxFaceIndex = 0; fbxFaceIndex < faceCount; ++fbxFaceIndex) {
    ufbx_face& fbxFace        = fbxMesh->faces[fbxMeshPart.face_indices.data[fbxFaceIndex]];
    size_t triangleIndexCount = fbxMesh->max_face_triangles * 3;
    std::vector<u32> verticesPerFaceIndexBuffer(triangleIndexCount);
    size_t triangleCount = ufbx_triangulate_face(
        verticesPerFaceIndexBuffer.data(), triangleIndexCount, fbxMesh, fbxFace
    );
    size_t vertexCountPerFace = triangleCount * 3;

    for (u32 vertexPerFace = 0; vertexPerFace < vertexCountPerFace; ++vertexPerFace) {
      u32 vertexPerFaceIndex = verticesPerFaceIndexBuffer[vertexPerFace];

      Vertex vertex{};

      u32 fbxVertexIndex     = fbxMesh->vertex_indices[vertexPerFaceIndex];
      ufbx_vec3& positionFbx = fbxMesh->vertices[fbxVertexIndex];
      vertex.Position        = glm::vec3(positionFbx.x, positionFbx.y, positionFbx.z);

      u32 fbxNormalIndex = fbxMesh->vertex_normal.indices[vertexPerFaceIndex];
      assert(
          fbxNormalIndex < fbxMesh->vertex_normal.values.count, "LoadMesh: memory violation normals"
      );
      ufbx_vec3& normalFbx = fbxMesh->vertex_normal.values.data[fbxNormalIndex];
      vertex.Normal        = glm::vec3(normalFbx.x, normalFbx.y, normalFbx.z);

      if (hasUVs) {
        u32 fbxUVIndex = fbxMesh->vertex_uv.indices[vertexPerFaceIndex];
        assert(
            fbxUVIndex < fbxMesh->vertex_uv.values.count,
            "LoadMesh: memory violation uv coordinates"
        );
        ufbx_vec2& uvFbx = fbxMesh->vertex_uv.values.data[fbxUVIndex];
        vertex.UV        = glm::vec2(uvFbx.x, uvFbx.y);
      }

      if (hasVertexColors) {
        u32 fbxColorIndex   = fbxMesh->vertex_color.indices[vertexPerFaceIndex];
        ufbx_vec4& colorFbx = fbxMesh->vertex_color.values.data[fbxColorIndex];

        glm::vec3 linearColor
            = glm::pow(glm::vec3(colorFbx.x, colorFbx.y, colorFbx.z), glm::vec3(2.2f));
        glm::vec4 vertexColor(linearColor.x, linearColor.y, linearColor.z, colorFbx.w);
        vertex.Color = vertexColor * diffuseColor;
      } else {
        vertex.Color = diffuseColor;
      }

      Vertices.push_back(vertex);
    }
  }
#pragma endregion

#pragma region Indices
  u32 meshAllVertices = static_cast<u32>(Vertices.size() - numVerticesBefore);

  ufbx_vertex_stream stream{};
  stream.data         = &Vertices[numVerticesBefore];
  stream.vertex_count = meshAllVertices;
  stream.vertex_size  = sizeof(Vertex);

  Indices.resize(numIndicesBefore + meshAllVertices);

  ufbx_error ufbxError;
  size_t vertexCount = ufbx_generate_indices(
      &stream, 1, &Indices[numIndicesBefore], meshAllVertices, nullptr, &ufbxError
  );

  if (ufbxError.type != UFBX_ERROR_NONE) {
    char errorBuffer[512];
    ufbx_format_error(errorBuffer, sizeof(errorBuffer), &ufbxError);
    std::print(
        "ufbxBuilder: creation of index buffer failed, file: {0}, error: {1},  node: {2}",
        _filepath, errorBuffer, fbxNode->name.data
    );
  }

  Vertices.resize(numVerticesBefore + vertexCount);
  mesh.VertexCount = static_cast<u32>(vertexCount);
  mesh.IndexCount  = meshAllVertices;
#pragma endregion
}
}  // namespace Rava