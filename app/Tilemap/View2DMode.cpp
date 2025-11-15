#include "View2DMode.h"
#include "TilemapEditor.h"
#include "../AppState.h"

#include "imgui.h"

View2DMode::View2DMode(AppState& app_state, TilemapEditor& editor)
    : m_app_state(&app_state),
      m_editor(&editor)
{
}

void View2DMode::handle_input()
{
    auto& project = app_state().project();

    if (ImGui::IsMouseDown(0)) {
        auto cell_opt = editor().mouseover_cell();
        if (cell_opt) {
            int x = cell_opt->first;
            int y = cell_opt->second;

            handle_click(x, y);
        }
    }
}

AppState& View2DMode::app_state()
{
    return *m_app_state;
}

TilemapEditor& View2DMode::editor()
{
    return *m_editor;
}

void View2DMode::handle_click(int tile_x, int tile_y)
{
}
