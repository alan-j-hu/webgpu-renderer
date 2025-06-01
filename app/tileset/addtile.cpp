#include "addtile.h"
#include "../filedialog.h"
#include "noworry/resourcetable.h"
#include "imgui.h"

namespace fs = std::filesystem;

AddTile::AddTile(AppState& app_state)
    : Modal("Add Tile"), m_app_state(&app_state)
{
}

ModalResponse AddTile::render()
{
    ModalResponse response = ModalResponse::KeepOpen;

    if (ImGui::Button("Choose Texture")) {
        m_app_state->modals().push(
            std::make_unique<FileDialog>(fs::current_path(), m_sink));
    }

    if (m_sink.size() > 0) {
        m_texture = m_app_state->resources().load_texture(m_sink[0]);
        m_sink.clear();
    }

    if (m_texture.get() != nullptr) {
        ImGui::Image((ImTextureID)(intptr_t)m_texture->view(),
                     ImVec2(50, 50));
    }

    if (ImGui::Button("Add Tile")) {
        response = ModalResponse::Close;
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        response = ModalResponse::Close;
    }
    return response;
}
