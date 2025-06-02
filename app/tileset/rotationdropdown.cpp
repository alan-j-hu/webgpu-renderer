#include "rotationdropdown.h"
#include "imgui.h"

RotationDropdown::RotationDropdown(RotationTag& rotation)
    : m_rotation(&rotation)
{
}

void RotationDropdown::render()
{
    const char* names[] = {
      "90",
      "180",
      "270",
      "360"
    };

    RotationTag enums[] = {
        RotationTag::Rotate0,
        RotationTag::Rotate90,
        RotationTag::Rotate180,
        RotationTag::Rotate270
    };

    if (ImGui::BeginCombo("##rotation", names[to_int(*m_rotation)])) {
        for (int i = 0; i < IM_ARRAYSIZE(names); ++i) {
            bool is_selected = i == to_int(*m_rotation);
            if (ImGui::Selectable(names[i], is_selected)) {
                *m_rotation = enums[i];
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

int RotationDropdown::to_int(RotationTag rotation)
{
    switch (rotation) {
    case RotationTag::Rotate0: return 0;
    case RotationTag::Rotate90: return 1;
    case RotationTag::Rotate180: return 2;
    default: return 3;
    }
}
