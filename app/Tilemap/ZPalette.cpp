#include "ZPalette.h"
#include "../AppState.h"
#include "noworry/Resource/ResourceLoader.h"

#include <stdexcept>
#include "imgui.h"

ZPalette::ZPalette(AppState& app_state)
    : m_app_state(&app_state),
      m_selected(16)
{
    load(0, "Resources/height-16.png");
    load(1, "Resources/height-15.png");
    load(2, "Resources/height-14.png");
    load(3, "Resources/height-13.png");
    load(4, "Resources/height-12.png");
    load(5, "Resources/height-11.png");
    load(6, "Resources/height-10.png");
    load(7, "Resources/height-9.png");
    load(8, "Resources/height-8.png");
    load(9, "Resources/height-7.png");
    load(10, "Resources/height-6.png");
    load(11, "Resources/height-5.png");
    load(12, "Resources/height-4.png");
    load(13, "Resources/height-3.png");
    load(14, "Resources/height-2.png");
    load(15, "Resources/height-1.png");
    load(16, "Resources/height0.png");
    load(17, "Resources/height1.png");
    load(18, "Resources/height2.png");
    load(19, "Resources/height3.png");
    load(20, "Resources/height4.png");
    load(21, "Resources/height5.png");
    load(22, "Resources/height6.png");
    load(23, "Resources/height7.png");
    load(24, "Resources/height8.png");
    load(25, "Resources/height9.png");
    load(26, "Resources/height10.png");
    load(27, "Resources/height11.png");
    load(28, "Resources/height12.png");
    load(29, "Resources/height13.png");
    load(30, "Resources/height14.png");
    load(31, "Resources/height15.png");
}

short ZPalette::selected_z() const
{
    return m_selected - 16;
}

std::shared_ptr<Texture> ZPalette::texture(short z) const
{
    if (z < -16 || z >= 16) {
        return {};
    }

    return m_textures[z + 16];
}

const Spritesheet* ZPalette::spritesheet(short z) const
{
    if (z < -16 || z >= 16) {
        return {};
    }

    return &m_spritesheets[z + 16];
}

void ZPalette::render()
{
    if (ImGui::BeginChild("ZPal", ImVec2(40, 0))) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 0);

        for (int i = 31; i >= 0; --i) {
            ImGui::PushID(i);
            if (render_button(i, m_selected == i)) {
                m_selected = i;
            }
            ImGui::PopID();
        }

        ImGui::PopStyleVar(2);
    }
    ImGui::EndChild();
}

void ZPalette::load(int index, const std::filesystem::path& path)
{
    auto& resources = m_app_state->resources();
    auto opt = resources.load<Texture>("./app" / path);
    if (opt) {
        m_textures[index] = *opt;
        m_spritesheets[index] = Spritesheet(
            m_app_state->renderer().device(),
            m_app_state->sprite_renderer().pipeline(),
            **opt,
            m_app_state->renderer().default_sampler());
    } else {
        throw std::runtime_error("Could not open " + path.string());
    }
}

bool ZPalette::render_button(int i, bool selected)
{
    auto& texture = *m_textures[i];

    ImVec4 tint_col = selected
        ? ImVec4(0.5, 1, 0.5, 1)
        : ImVec4(1, 1, 1, 1);

    return ImGui::ImageButton(
        "Z",
        (ImTextureID)(intptr_t)texture.view(),
        ImVec2(16, 16),
        ImVec2(0, 0),
        ImVec2(1, 1),
        ImVec4(0, 0, 0, 0),
        tint_col);
}
