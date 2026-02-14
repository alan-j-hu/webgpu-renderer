#include "NewTile.h"
#include "../FileDialog.h"
#include "../../Commands/CreateTileCommand.h"
#include "noworry/resourcetable.h"
#include "imgui.h"
#include <utility>

NewTile::NewTile(AppState& app_state)
    : m_app_state(&app_state),
      m_edit(app_state),
      m_definition(std::make_unique<TileDef>())
{
}

void NewTile::open(int index)
{
    m_index = index;
    ImGui::OpenPopup("New Tile");
}

void NewTile::update()
{
    if (ImGui::BeginPopupModal("New Tile")) {
        if (auto optional = m_edit.render(*m_definition)) {
            m_definition = std::move(optional.value());
        }

        if (ImGui::Button("Add Tile")) {
            auto holder = std::make_unique<TileDef>();
            m_definition.swap(holder);
            m_app_state->push_command(std::make_unique<CreateTileCommand>(
                std::move(*holder),
                m_index));
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            m_definition = std::make_unique<TileDef>();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
