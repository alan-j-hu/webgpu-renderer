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
    auto& project = m_app_state->project();
    std::shared_ptr<const Tileset> tileset = project.tileset_at(0);

    if (ImGui::BeginChild("Tiles ##Tiles", ImVec2(100, 250))) {
        for (int i = 0; i < tileset->count(); ++i) {
            auto def = tileset->at(i);

            WGPUTextureView view =
                m_editor->tileset_thumbnails(0).at(i).texture_view();
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
