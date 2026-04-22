#include "TileList.h"
#include "../Editor.h"
#include "../../Commands/DeleteTiledefCommand.h"
#include "../../Commands/ReorderTiledefCommand.h"
#include "../../Commands/UpdateTiledefCommand.h"
#include <utility>
#include "imgui.h"

TileList::TileList(AppState& state, Editor& editor)
    : m_app_state(&state),
      m_editor(&editor),
      m_new_tile(state),
      m_tile_editor(state)
{
}

int TileList::count()
{
    return m_app_state->project().tileset_at(0)->count();
}

ListBox::ItemState TileList::item(int i)
{
    const int PADDING = 5;
    std::optional<int> selected = m_app_state->selected_tiledef_idx();
    auto& tileset = *m_app_state->project().tileset_at(0);

    auto def = tileset.at(i);
    WGPUTextureView view =
        m_editor->tileset_thumbnails(0).at(i).texture_view();

    ImVec2 inner_dim = ImVec2(32 * def->width(), 32 * def->depth());
    ImVec2 outer_dim = ImVec2(
        inner_dim.x + 2 * PADDING,
        inner_dim.y + 2 * PADDING);
    ImVec2 uv_tl = ImVec2(0, 1 - def->depth() / 5.0);
    ImVec2 uv_br = ImVec2(def->width() / 5.0, 1);

    ImVec2 cursor_before = ImGui::GetCursorScreenPos();

    bool is_selected = ImGui::Selectable(
        "##Tile",
        selected == i,
        ImGuiSelectableFlags_AllowOverlap,
        outer_dim);
    bool is_hovered = ImGui::IsItemHovered(
        ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

    context_menu(i);

    ImVec2 cursor_after = ImGui::GetCursorScreenPos();
    ImGui::SetCursorScreenPos(ImVec2(
        cursor_before.x + PADDING,
        cursor_before.y + PADDING));

    ImGui::Image((ImTextureID)view, inner_dim, uv_tl, uv_br);

    ImGui::SetCursorScreenPos(cursor_after);

    ImGui::Dummy(ImVec2(0, 0));

    return is_selected ? ListBox::SELECTED
        : is_hovered ? ListBox::HOVERED
        : ListBox::NONE;
}

void TileList::select(int idx)
{
    m_app_state->select_tiledef(idx);
}

void TileList::request_reorder(int idx, int delta)
{
    auto& tileset = *m_app_state->project().tileset_at(0);
    m_app_state->push_command(
        std::make_unique<ReorderTiledefCommand>(tileset, idx, delta));
}

void TileList::draw()
{
    auto& tileset = *m_app_state->project().tileset_at(0);
    if (ImGui::Button("Add Tile")) {
        m_new_tile.open(tileset.count());
    }
    m_new_tile.update();

    update();
}

void TileList::draw_tile_editor()
{
    auto& tileset = *m_app_state->project().tileset_at(0);
    if (auto selected = m_app_state->selected_tiledef_idx()) {
        if (auto optional = m_tile_editor.render(*tileset.at(*selected))) {
            m_app_state->push_command(std::make_unique<UpdateTiledefCommand>(
                *selected,
                std::move(*optional.value())));
        }
    } else {
        ImGui::Text("(No tile selected.)");
    }
}

void TileList::context_menu(int i)
{
    auto& tileset = *m_app_state->project().tileset_at(0);

    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Delete")) {
            m_app_state->push_command(
                std::make_unique<DeleteTiledefCommand>(tileset, i));
        }
        ImGui::EndPopup();
    }
}
