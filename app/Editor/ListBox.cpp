#include "ListBox.h"
#include "imgui.h"

#include <iostream>

void ListBox::update()
{
    ImGuiIO& io = ImGui::GetIO();
    const bool mouse_down = ImGui::IsMouseDown(ImGuiMouseButton_Left);

    for (int i = 0; i < count(); ++i) {
        ImGui::PushID(i);

        if (mouse_down && m_drag_drop && m_drag_drop->cursor == i) {
            ImGui::Separator();
        }

        ImVec2 cursor_before = ImGui::GetCursorScreenPos();
        switch (item(i)) {
        case ListBox::SELECTED:
            select(i);
            break;
        case ListBox::HOVERED:
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                m_drag_drop = { i, i };
            }

            if (m_drag_drop) {
                ImVec2 cursor_after = ImGui::GetCursorScreenPos();
                float middle = cursor_before.y
                  + (cursor_after.y - cursor_before.y) / 2.0;
                if (io.MousePos.y < middle) {
                    m_drag_drop = { m_drag_drop->idx, i };
                } else {
                    m_drag_drop = { m_drag_drop->idx, i + 1 };
                }
            }
            break;
        }

        ImGui::PopID();
    }

    if (m_drag_drop) {
        int idx = m_drag_drop->idx;
        int cursor = m_drag_drop->cursor;

        if (mouse_down) {
            if (cursor == count()) {
                ImGui::Separator();
            }
        } else {
            int starting_cursor = cursor > idx
                ? idx + 1
                : idx;
            int delta = cursor - starting_cursor;
            if (delta != 0) {
                request_reorder(idx, delta);
            }

            m_drag_drop = std::nullopt;
        }
    }
}
