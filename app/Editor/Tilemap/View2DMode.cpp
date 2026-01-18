#include "View2DMode.h"
#include "../Editor.h"
#include "../../AppState.h"

#include "imgui.h"

View2DMode::View2DMode(AppState& app_state, Editor& editor)
    : m_app_state(&app_state),
      m_editor(&editor)
{
}

void View2DMode::handle_input()
{
    auto& project = app_state().project();

    auto cell_opt = editor().mouseover_cell();
    if (cell_opt) {
        int x = cell_opt->first;
        int y = cell_opt->second;

        if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            handle_left_mouse_down(x, y);
        } else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            handle_left_mouse_up(x, y);
        } else if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            handle_right_mouse_down(x, y);
        }
    }
}

AppState& View2DMode::app_state()
{
    return *m_app_state;
}

Editor& View2DMode::editor()
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

void View2DMode::handle_left_mouse_down(int tile_x, int tile_y)
{
}

void View2DMode::handle_left_mouse_up(int tile_x, int tile_y)
{
}

void View2DMode::handle_right_mouse_down(int tile_x, int tile_y)
{
}
