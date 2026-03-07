#include "ListBox.h"
#include "imgui.h"

void ListBox::update()
{
    ImGuiIO& io = ImGui::GetIO();
    const bool mouse_down = ImGui::IsMouseDown(ImGuiMouseButton_Left);

    for (int i = 0; i < count(); ++i) {
        ImGui::PushID(i);

        if (m_drag_drop && m_drag_drop->target == i) {
            if (mouse_down) {
                ImGui::Separator();
            } else {
                request_reorder(m_drag_drop->source, m_drag_drop->target);
                m_drag_drop = std::nullopt;
            }
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
                    m_drag_drop = { m_drag_drop->source, i };
                } else {
                    m_drag_drop = { m_drag_drop->source, i + 1 };
                }
            }
            break;
        }

        ImGui::PopID();
    }
}
