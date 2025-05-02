#include "addnewtile.h"
#include "tiledefinition.h"
#include "tileset.h"
#include "tileseteditor.h"
#include "../filedialog.h"
#include <algorithm>
#include <utility>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "imgui.h"

namespace fs = std::filesystem;

AddNewTile::AddNewTile(TilesetEditor& tileset_editor, ModalStack& modals)
    : Modal("Add New Tile"),
      m_tileset_editor(tileset_editor),
      m_modals(modals),
      m_width(1),
      m_height(1),
      m_error(Error::None)
{
}

bool AddNewTile::render()
{
    bool close = false;

    if (ImGui::Button("Choose Mesh File", ImVec2(0, 0))) {
        m_modals.push(
            std::make_unique<FileDialog>(fs::current_path(), m_sink));
    }

    if (ImGui::BeginListBox("##Meshes", ImVec2(-FLT_MIN, 0))) {
        for (auto& pair : m_tileset_editor.mesh_map()) {
            if (ImGui::Selectable(pair.first.c_str(), false, 0)) {
            }
        }
        ImGui::EndListBox();
    }

    ImGui::InputInt("Width", &m_width);
    m_width = std::min(1, m_width);
    ImGui::InputInt("Height", &m_height);
    m_height = std::min(1, m_height);

    render_error();
    if (ImGui::Button("Add", ImVec2(0, 0))) {
        close = try_add_tile();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(0, 0))) {
        close = true;
    }

    return close;
}

void AddNewTile::render_error()
{
    switch (m_error) {
    case Error::BadMesh:
        ImGui::Text("Bad mesh.");
        break;
    default:
    }
}

bool AddNewTile::try_add_tile()
{
    Assimp::Importer importer;

    if (m_sink.size() == 0) {
        return false;
    }

    fs::path& path = m_sink[0];
    const aiScene* scene = importer.ReadFile(
        path.string(),
        aiProcess_CalcTangentSpace      |
        aiProcess_Triangulate           |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);
    if (scene == nullptr) {
        m_error = Error::BadMesh;
        return false;
    }
    m_tileset_editor.tileset().add_tile(TileDefinition(m_width, m_height));

    return true;
}
