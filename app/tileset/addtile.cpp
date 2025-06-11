#include "addtile.h"
#include "../filedialog.h"
#include "noworry/resourcetable.h"
#include "imgui.h"
#include <iostream>

AddTile::AddTile(AppState& app_state)
    : Modal("Add Tile"),
      m_app_state(&app_state),
      m_edit(app_state)
{
}

ModalResponse AddTile::render()
{
    ModalResponse response = ModalResponse::KeepOpen;

    auto optional = m_edit.render(m_definition);
    if (optional) {
        m_definition = optional.value();
    }

    if (ImGui::Button("Add Tile")) {
        response = ModalResponse::Close;
        auto& project = m_app_state->project();
        m_app_state->set_project(project.map_tile_defs([&](auto tile_defs) {
            return tile_defs.push_back(m_definition);
        }));
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        response = ModalResponse::Close;
    }
    return response;
}
