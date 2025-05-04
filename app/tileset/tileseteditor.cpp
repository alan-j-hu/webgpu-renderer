#include "tileseteditor.h"
#include "addnewtile.h"
#include "../filedialog.h"

#include <utility>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "imgui.h"

TilesetEditor::TilesetEditor(ModalStack& modals, Renderer& renderer)
    : m_modals(modals),
      m_renderer(renderer),
      m_tile_preview(renderer.device(), 200, 200),
      m_resources(m_renderer),
      m_scene(m_renderer)
{
    m_tile_preview.set_clear_color({1, 1, 1, 1});

    m_default_material = &m_resources.add_flat_material(0.5, 0.5, 0.5);
}

bool TilesetEditor::render_preview()
{
    m_renderer.render(m_tile_preview, m_selected_tile->scene());
    return true;
}

void TilesetEditor::render()
{
    namespace fs = std::filesystem;

    if (ImGui::Button("Choose Mesh File", ImVec2(0, 0))) {
        m_modals.push(
            std::make_unique<FileDialog>(fs::current_path(), m_sink));
    }

    if (ImGui::Button("Add Tile", ImVec2(0, 0))) {
        m_modals.push(std::make_unique<AddNewTile>(*this, m_modals));
    }

    if (ImGui::BeginListBox("##Meshes", ImVec2(-FLT_MIN, 0))) {
        for (auto& pair : m_mesh_map) {
            bool selected = pair.second == m_selected_tile;
            if (ImGui::Selectable(pair.second->name().c_str(), selected, 0)) {
                m_selected_tile = pair.second;
            }
        }
        ImGui::EndListBox();
    }

    if (m_selected_tile != nullptr) {
        render_preview();
        ImGui::Image((ImTextureID)(intptr_t)m_tile_preview.texture().view(),
                     ImVec2(m_tile_preview.width(), m_tile_preview.height()));

        if (ImGui::BeginListBox("##Vertices", ImVec2(-FLT_MIN, 0))) {
            for (auto index : m_selected_tile->indices()) {
                ImGui::Value("x", index);
            }
            ImGui::EndListBox();
        }
    }

    if (m_sink.size() != 0) {
        load_meshes(m_sink[0]);
        m_sink.clear();
    }
}

void TilesetEditor::load_meshes(std::filesystem::path& path)
{
    m_selected_tile = nullptr;
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

void TilesetEditor::visit_node(const aiScene* scene, const aiNode* node)
{
    if (node->mNumMeshes != 0) {
        TileMesh& mesh = load_mesh(
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

TileMesh& TilesetEditor::load_mesh(const char* name, aiMesh* mesh)
{
    std::vector<Vertex> vertices;
    const int vc = mesh->mNumVertices;
    vertices.reserve(vc);
    for (int i = 0; i < vc; ++i) {
        Vertex v;
        v.x = mesh->mVertices[i].x;
        v.y = mesh->mVertices[i].z;
        v.z = mesh->mVertices[i].y;
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

    Mesh& gpu_mesh = m_resources.add_mesh(
        vertices.data(), vc, indices.data(), indices.size());
    m_meshes.push_back(
        std::make_unique<TileMesh>(
            *this,
            name,
            gpu_mesh,
            vertices,
            indices));
    return *m_meshes[m_meshes.size() - 1];
}
