#include "TileList.h"
#include "../../Commands/UpdateTileCommand.h"
#include <utility>
#include "imgui.h"

TileList::TileList(AppState& state, Editor& editor)
    : m_app_state(&state),
      m_tile_picker(state, editor),
      m_editor(&editor),
      m_new_tile(state),
      m_tile_editor(state),
      m_selected{-1}
{
}

void TileList::draw()
{
    std::shared_ptr<const Tileset> tileset =
        m_app_state->project().tileset_at(0);

    if (ImGui::Button("Add New Tile")) {
        m_new_tile.open();
    }
    m_new_tile.update();

    auto& project = m_app_state->project();
    m_tile_picker.render(m_selected);

    if (m_selected != -1) {
        auto optional = m_tile_editor.render(*tileset->at(m_selected));
        if (optional) {
            m_app_state->push_command(std::make_unique<UpdateTileCommand>(
                m_selected,
                optional.value())
            );
        }
    }
}
