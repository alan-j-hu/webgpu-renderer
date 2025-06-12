#include "tilelist.h"
#include "addtile.h"
#include <utility>
#include "imgui.h"

TileList::TileList(std::string name, int flex, AppState& state)
    : Pane(std::move(name), flex),
      m_app_state(&state),
      m_tile_picker(state),
      m_editor(state),
      m_selected{-1}
{
}

void TileList::content()
{
    if (ImGui::Button("Add New Tile")) {
        m_app_state->modals().push(std::make_unique<AddTile>(*m_app_state));
    }

    auto& project = m_app_state->project();
    m_tile_picker.render(m_selected);

    if (m_selected != -1) {
        auto optional = m_editor.render(project.tile_defs().at(m_selected));
        if (optional) {
            m_app_state->set_project(project.map_tile_defs([&](auto tiles) {
                return tiles.set(m_selected, optional.value());
            }));
        }
    }
}
