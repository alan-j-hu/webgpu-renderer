#include "addtile.h"
#include "../filedialog.h"
#include "noworry/resourcetable.h"
#include "imgui.h"

AddTile::AddTile(AppState& app_state)
    : Modal("Add Tile"),
      m_app_state(&app_state),
      m_edit(app_state)
{
}

ModalResponse AddTile::render()
{
    ModalResponse response = ModalResponse::KeepOpen;

    m_edit.render(m_definition);

    if (ImGui::Button("Add Tile")) {
        response = ModalResponse::Close;
        m_app_state->tileset().add_tile(m_definition);
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        response = ModalResponse::Close;
    }
    return response;
}
