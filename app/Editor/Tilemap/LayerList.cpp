#include "LayerList.h"
#include "../Editor.h"
#include "../../AppState.h"
#include "../../Project.h"
#include "../../Commands/CreateLayerCommand.h"
#include "../../Commands/DeleteLayerCommand.h"
#include "../../Commands/ReorderLayerCommand.h"
#include "imgui.h"

LayerList::LayerList(AppState& app_state, Editor& editor)
    : m_app_state(&app_state), m_editor(&editor)
{
}

int LayerList::count()
{
    auto* level = m_app_state->selected_level();
    if (level == nullptr) {
        return 0;
    }
    return level->layer_count();
}

ListBox::ItemState LayerList::item(int idx)
{
    auto* level = m_app_state->selected_level();
    if (level == nullptr) {
        return ListBox::NONE;
    }

    const Layer& layer = level->layer_at(idx);
    const bool selected = m_app_state->selected_layer() == &layer;
    const ImVec4 bg_color =
        selected ? ImVec4(1, 1, 1, 1) : ImVec4(0.5, 0.5, 0.5, 1);
    const ImVec4 tint_color =
        selected ? ImVec4(0.8, 0.8, 0.8, 1) : ImVec4(1, 1, 1, 1);

    auto& level_node = *m_editor->level_node(*level);
    ImTextureID tex_id =
        (ImTextureID)(intptr_t)level_node.layer_at(idx).thumbnail().view();

    auto state = ListBox::NONE;
    if (ImGui::ImageButton(
        std::to_string(idx).c_str(),
        tex_id,
        ImVec2(100, 100),
        ImVec2(0, 0),
        ImVec2(1, 1),
        bg_color,
        tint_color)) {

        state = ListBox::SELECTED;
    } else if (ImGui::IsItemHovered(
            ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)) {

        state = ListBox::HOVERED;
    }

    context_menu(*level, idx);

    return state;
}

void LayerList::select(int idx)
{
    return m_app_state->select_layer(idx);
}

void LayerList::request_reorder(int idx, int delta)
{
    auto* level = m_app_state->selected_level();
    if (level == nullptr) {
        return;
    }
    m_app_state->push_command(
        std::make_unique<ReorderLayerCommand>(*level, idx, delta));
}

void LayerList::draw()
{
    auto* level = m_app_state->selected_level();
    if (level == nullptr) {
        return;
    }

    if (ImGui::Button("Add Layer")) {
        m_app_state->push_command(
            std::make_unique<CreateLayerCommand>(*level));
    }

    update();
}

void LayerList::context_menu(Level& level, int idx)
{
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Delete")) {
            m_app_state->push_command(
                std::make_unique<DeleteLayerCommand>(level, idx));
        }
        ImGui::EndPopup();
    }
}
