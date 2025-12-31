#include "AddTile.h"
#include "../FileDialog.h"
#include "../Commands/CreateTileCommand.h"
#include "noworry/resourcetable.h"
#include "imgui.h"
#include <utility>

AddTile::AddTile(AppState& app_state)
    : Modal("Add Tile"),
      m_app_state(&app_state),
      m_edit(app_state),
      m_definition(std::make_shared<TileDef>())
{
}

ModalResponse AddTile::render()
{
    ModalResponse response = ModalResponse::KeepOpen;

    auto optional = m_edit.render(*m_definition);
    if (optional) {
        m_definition = optional.value();
    }

    if (ImGui::Button("Add Tile")) {
        response = ModalResponse::Close;
        m_app_state->push_command(
            std::make_unique<CreateTileCommand>(*m_definition)
        );
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        response = ModalResponse::Close;
    }
    return response;
}
