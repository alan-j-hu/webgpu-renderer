#include "TilePicker.h"
#include "Editor/Editor.h"
#include "imgui.h"
#include <webgpu/webgpu.h>

TilePicker::TilePicker(AppState& app_state, Editor& editor)
    : m_app_state(&app_state), m_editor(&editor)
{
}

void TilePicker::render(int& selected_tile)
{
    const int PADDING = 5;

    auto& project = m_app_state->project();
    std::shared_ptr<const Tileset> tileset = project.tileset_at(0);

    if (ImGui::BeginChild("Tiles ##Tiles", ImVec2(100, 250))) {
        for (int i = 0; i < tileset->count(); ++i) {
            ImGui::PushID(i);

            auto def = tileset->at(i);
            WGPUTextureView view =
                m_editor->tileset_thumbnails(0).at(i).texture_view();

            ImVec2 inner_dim = ImVec2(32 * def->width(), 32 * def->depth());
            ImVec2 outer_dim = ImVec2(
                inner_dim.x + 2 * PADDING,
                inner_dim.y + 2 * PADDING);
            ImVec2 uv_tl = ImVec2(0, 1 - def->depth() / 5.0);
            ImVec2 uv_br = ImVec2(def->width() / 5.0, 1);

            ImVec2 cursor_before = ImGui::GetCursorPos();
            if (ImGui::Selectable(
                    "##Tile",
                    selected_tile == i,
                    ImGuiSelectableFlags_AllowOverlap,
                    outer_dim)) {
                selected_tile = i;
            }
            ImVec2 cursor_after = ImGui::GetCursorPos();

            ImGui::SetCursorPos(ImVec2(
                cursor_before.x + PADDING,
                cursor_before.y + PADDING));

            ImGui::Image((ImTextureID)view, inner_dim, uv_tl, uv_br);

            ImGui::SetCursorPos(cursor_after);

            ImGui::PopID();
        }
    }
    ImGui::EndChild();
}
