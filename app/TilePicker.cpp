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
    if (ImGui::BeginChild("Tiles ##Tiles", ImVec2(100, 200))) {
        for (int i = 0; i < project.tiledef_count(); ++i) {
            WGPUTextureView view =
                m_app_state->tile_thumbnail(i).texture_view();
            ImGui::PushID(i);
            if (ImGui::ImageButton("Tile",
                                   (ImTextureID)view,
                                   ImVec2(32, 32))) {
                selected_tile = i;
            }
            ImGui::PopID();
        }
    }
    ImGui::EndChild();
}
