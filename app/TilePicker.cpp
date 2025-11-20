#include "TilePicker.h"
#include "imgui.h"
#include <webgpu/webgpu.h>

TilePicker::TilePicker(AppState& app_state)
    : m_app_state(&app_state)
{
}

void TilePicker::render(int& selected_tile)
{
    auto& project = m_app_state->project();
    if (ImGui::BeginChild("Tiles ##Tiles", ImVec2(100, 250))) {
        for (int i = 0; i < project.tiledef_count(); ++i) {
            auto def = project.tiledef_at(i);

            WGPUTextureView view =
                m_app_state->tile_thumbnail(i).texture_view();
            ImGui::PushID(i);

            ImVec2 uv_tl = ImVec2(0, 1 - def->depth() / 5.0);
            ImVec2 uv_br = ImVec2(def->width() / 5.0, 1);

            if (ImGui::ImageButton("Tile",
                                   (ImTextureID)view,
                                   ImVec2(32 * def->width(),
                                          32 * def->depth()),
                                   uv_tl,
                                   uv_br)) {
                selected_tile = i;
            }
            ImGui::PopID();
        }
    }
    ImGui::EndChild();
}
