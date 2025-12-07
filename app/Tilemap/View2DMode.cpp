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

Region View2DMode::region(int tile_x, int tile_y, float w, float h)
{
    glm::vec3 pos_3d(tile_x, tile_y, 0);

    // Bottom-left in 2D space
    glm::vec2 pos_bl = editor().map_3d_to_2d(pos_3d);
    // Top-right in 2D space
    glm::vec2 pos_tr = editor().map_3d_to_2d(pos_3d + glm::vec3(w, h, 0));

    // Beware, +1 goes towards the top of the screen in 3D, but towards
    // the bottom in 2D
    Region r;
    r.x = pos_bl.x;
    r.y = pos_bl.y;
    r.w = pos_tr.x - pos_bl.x;
    r.h = pos_bl.y - pos_tr.y;

    return r;
}

void View2DMode::handle_click(int tile_x, int tile_y)
{
}
