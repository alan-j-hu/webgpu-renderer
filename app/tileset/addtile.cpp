#include "addtile.h"
#include "imgui.h"

AddTile::AddTile()
    : Modal("Add Tile")
{
}

ModalResponse AddTile::render()
{
    ModalResponse response = ModalResponse::KeepOpen;
    if (ImGui::Button("Add Tile")) {
        response = ModalResponse::Close;
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        response = ModalResponse::Close;
    }
    return response;
}
