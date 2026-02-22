#include "TileList.h"
#include "../Editor.h"
#include "../../Commands/DeleteTileCommand.h"
#include "../../Commands/UpdateTileCommand.h"
#include <utility>
#include "imgui.h"

TileList::TileList(AppState& state, Editor& editor)
    : m_app_state(&state),
      m_tile_picker(state, editor),
      m_editor(&editor),
      m_new_tile(state),
      m_tile_editor(state)
{
}

void TileList::draw()
{
    std::shared_ptr<const Tileset> tileset =
        m_app_state->project().tileset_at(0);
    std::optional<int> selected = m_app_state->selected_tiledef_idx();

    if (ImGui::Button("+")) {
        int next_idx = selected ? *selected + 1 : 0;
        m_new_tile.open(next_idx);
    }
    m_new_tile.update();

    ImGui::SameLine();
    if (ImGui::Button("-") && selected) {
        m_app_state->push_command(
            std::make_unique<DeleteTileCommand>(*selected));
        selected = std::nullopt;
    }

    auto& project = m_app_state->project();
    m_tile_picker.render(selected);
    m_app_state->select_tiledef(selected);

    if (selected) {
        if (auto optional = m_tile_editor.render(*tileset->at(*selected))) {
            m_app_state->push_command(std::make_unique<UpdateTileCommand>(
                *selected,
                std::move(*optional.value())));
        }
    }
}
