#include "tileseteditor.h"
#include "addnewtile.h"
#include "../filedialog.h"

#include "noworry/mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "imgui.h"

TilesetEditor::TilesetEditor(ModalStack& modals, Renderer& renderer)
    : m_modals(modals), m_renderer(renderer)
{
}

void TilesetEditor::render()
{
    namespace fs = std::filesystem;

    if (ImGui::Button("Choose Mesh File", ImVec2(0, 0))) {
        m_modals.push(
            std::make_unique<FileDialog>(fs::current_path(), m_sink));
    }

    if (ImGui::Button("Add Tile", ImVec2(0, 0))) {
        m_modals.push(std::make_unique<AddNewTile>(m_tileset, m_modals));
    }

    if (ImGui::BeginListBox("##Meshes", ImVec2(-FLT_MIN, 0))) {
        for (auto& name : m_names) {
            if (ImGui::Selectable(name.c_str(), false, 0)) {
            }
        }
        ImGui::EndListBox();
    }

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

    Mesh(m_renderer.device(),
         vertices.data(), vc,
         indices.data(), indices.size());
}
