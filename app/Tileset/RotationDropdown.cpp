#include "RotationDropdown.h"
#include "imgui.h"

static int to_int(RotationTag rotation)
{
    switch (rotation) {
    case RotationTag::Rotate0: return 0;
    case RotationTag::Rotate90: return 1;
    case RotationTag::Rotate180: return 2;
    default: return 3;
    }
}

void rotation_dropdown(RotationTag& rotation)
{
    const char* names[] = {
      "0 degrees",
      "90 degrees",
      "180 degrees",
      "270 degrees"
    };

    RotationTag enums[] = {
        RotationTag::Rotate0,
        RotationTag::Rotate90,
        RotationTag::Rotate180,
        RotationTag::Rotate270
    };

    if (ImGui::BeginCombo("##rotation", names[to_int(rotation)])) {
        for (int i = 0; i < IM_ARRAYSIZE(names); ++i) {
            bool is_selected = i == to_int(rotation);
            if (ImGui::Selectable(names[i], is_selected)) {
                rotation = enums[i];
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}
