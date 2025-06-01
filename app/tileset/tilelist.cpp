#include "tilelist.h"
#include "addtile.h"
#include <utility>
#include "imgui.h"

TileList::TileList(std::string name, int flex, AppState& state)
    : Pane(std::move(name), flex), m_app_state(&state)
{
}

void TileList::content()
{
    if (ImGui::Button("Add New Tile")) {
        m_app_state->modals().push(std::make_unique<AddTile>());
    }
}
