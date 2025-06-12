#include "appstate.h"
#include "noworry/grid.h"

#include <utility>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

AppState::AppState(WGPUDevice device)
    : m_renderer(device),
      m_resources(m_renderer),
      m_small_grid_mesh(
          create_grid(device,
                      glm::vec3(0, 0, 0),
                      5,
                      5,
                      glm::vec3(5, 0, 0),
                      glm::vec3(0, 5, 0))),
      m_thumbnail_camera(),
      m_thumbnail_scene(m_renderer, m_thumbnail_camera)
{
    m_background_color = {0.0f, 0.5f, 0.5f, 1.0f};
    m_default_material = &m_resources.add_flat_material(0.5, 0.5, 0.5);
    m_wireframe_material = &m_resources.add_wireframe_material(1, 1, 1);

    m_thumbnail_camera.set_top(1);
    m_thumbnail_camera.set_bottom(0);
    m_thumbnail_camera.set_left(0);
    m_thumbnail_camera.set_right(1);
    m_thumbnail_camera.set_position(glm::vec3(0, 0, 1));
    m_thumbnail_camera.set_target(glm::vec3(0, 0, 0));
}

ResolvedTile AppState::resolve_tile(const TileDef& def)
{
    ResolvedTile resolved;

    if (def.mesh) {
        resolved.mesh = &def.mesh.value()->rotated(def.rotation);
    }

    if (def.texture) {
        resolved.material = def.texture.value().material.get();
    }

    return resolved;
}

void AppState::push_command(std::unique_ptr<Command> command)
{
    command->redo(m_project);
    m_history.push_back(std::move(command));
}

void AppState::refresh_thumbnails()
{
    while (m_thumbnail_cache.size() < m_project.tiledef_count()) {
        m_thumbnail_cache.emplace_back(m_renderer.device(), 64, 64);
    }
    for (int i = 0; i < m_project.tiledef_count(); ++i) {
        RenderTarget& target = m_thumbnail_cache.at(i);
        {
            Frame frame(m_renderer, target, m_thumbnail_scene);
            auto def = m_project.tiledef_at(i);
            ResolvedTile resolved = resolve_tile(*def);

            if (resolved.mesh == nullptr) {
                continue;
            }
            Material& material = resolved.material == nullptr
                ? default_material()
                : *resolved.material;

            Transform transform;
            frame.add(transform, resolved.mesh->mesh(), material);
        }
    }
}

void AppState::load_meshes(std::filesystem::path& path)
{
    m_mesh_map.clear();
    m_meshes.clear();
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path.string(),
        aiProcess_CalcTangentSpace      |
        aiProcess_Triangulate           |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);
    if (scene == nullptr) {
        return;
    }

    visit_node(scene, scene->mRootNode);
}

void AppState::visit_node(const aiScene* scene, const aiNode* node)
{
    if (node->mNumMeshes != 0) {
        TileRotations& mesh = load_mesh(
            node->mName.C_Str(),
            scene->mMeshes[node->mMeshes[0]]);
        m_mesh_map.emplace(mesh.name(), &mesh);
    }

    const unsigned int count = node->mNumChildren;
    aiNode** const children = node->mChildren;
    for (int i = 0; i < count; ++i) {
        visit_node(scene, children[i]);
    }
}

TileRotations& AppState::load_mesh(const char* name, aiMesh* mesh)
{
    std::vector<Vertex> vertices;
    const int vc = mesh->mNumVertices;
    vertices.reserve(vc);
    for (int i = 0; i < vc; ++i) {
        Vertex v;
        v.x = mesh->mVertices[i].x;
        v.y = mesh->mVertices[i].y;
        v.z = mesh->mVertices[i].z;
        v.u = mesh->mTextureCoords[0][i].x;
        v.v = mesh->mTextureCoords[0][i].y;
        vertices.push_back(v);
    }

    std::vector<std::uint16_t> indices;
    const int fc = mesh->mNumFaces;
    indices.reserve(3 * fc);
    for (int i = 0; i < fc; ++i) {
        indices.push_back(mesh->mFaces[i].mIndices[0]);
        indices.push_back(mesh->mFaces[i].mIndices[1]);
        indices.push_back(mesh->mFaces[i].mIndices[2]);
    }

    // padding
    if (indices.size() % 2 == 1) {
        indices.push_back(0);
    }

    m_meshes.push_back(
        std::make_unique<TileRotations>(*this, name, vertices, indices));
    return *m_meshes[m_meshes.size() - 1];
}
