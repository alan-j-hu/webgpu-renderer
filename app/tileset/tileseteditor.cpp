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
      m_tile_preview(renderer.device(), 100, 100),
      m_scene(m_renderer)
{
    m_tile_preview.set_clear_color({1, 1, 1, 1});
}

bool TilesetEditor::render_preview()
{
    m_renderer.render(m_tile_preview, m_scene);
    return true;
}

void TilesetEditor::render()
{
    namespace fs = std::filesystem;

    render_preview();

    if (ImGui::Button("Choose Mesh File", ImVec2(0, 0))) {
        m_modals.push(
            std::make_unique<FileDialog>(fs::current_path(), m_sink));
    }

    if (ImGui::Button("Add Tile", ImVec2(0, 0))) {
        m_modals.push(std::make_unique<AddNewTile>(*this, m_modals));
    }

    if (ImGui::BeginListBox("##Meshes", ImVec2(-FLT_MIN, 0))) {
        for (auto& pair : m_mesh_map) {
            if (ImGui::Selectable(pair.first.c_str(), false, 0)) {
            }
        }
        ImGui::EndListBox();
    }

    ImGui::Image((ImTextureID)(intptr_t)m_tile_preview.texture().view(),
                 ImVec2(m_tile_preview.width(), m_tile_preview.height()));

    if (m_sink.size() != 0) {
        load_meshes(m_sink[0]);
        m_sink.clear();
    }
}

void TilesetEditor::load_meshes(std::filesystem::path& path)
{
    m_names.clear();
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

    for (int i = 0; i < scene->mNumMeshes; ++i) {
        load_mesh(scene->mMeshes[i]);
    }

    visit_node(scene->mRootNode);
}

void TilesetEditor::visit_node(aiNode* node)
{
    m_names.push_back(node->mName.C_Str());
    if (node->mNumMeshes != 0) {
        m_mesh_map[node->mName.C_Str()] = m_meshes[0].get();
    }

    const unsigned int count = node->mNumChildren;
    aiNode** const children = node->mChildren;
    for (int i = 0; i < count; ++i) {
        visit_node(children[i]);
    }
}

void TilesetEditor::load_mesh(aiMesh* mesh)
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
        indices.push_back(mesh->mFaces->mIndices[0]);
        indices.push_back(mesh->mFaces->mIndices[1]);
        indices.push_back(mesh->mFaces->mIndices[2]);
    }

    // padding
    if (indices.size() % 2 == 1) {
        indices.push_back(0);
    }

    m_meshes.push_back(
        std::make_unique<Mesh>(
            m_renderer.device(),
            vertices.data(), vc,
            indices.data(), indices.size()));
}
