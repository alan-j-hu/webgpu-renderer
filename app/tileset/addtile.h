#ifndef ADDTILE_H
#define ADDTILE_H

#include "rotationdropdown.h"
#include "../appstate.h"
#include "../modal.h"
#include "noworry/rendertarget.h"
#include "noworry/texture.h"
#include "noworry/camera/perspectivecamera.h"
#include "noworry/scene/scene.h"
#include <filesystem>
#include <memory>
#include <vector>

/// Modal for adding a new tile.
class AddTile : public Modal
{
public:
    AddTile(AppState&);

    virtual ModalResponse render() override;

private:
    AppState* m_app_state;
    TileRotations* m_selected_mesh;
    std::shared_ptr<Texture> m_texture;
    std::vector<std::filesystem::path> m_sink;

    RotationTag m_rotation;
    RotationDropdown m_rotation_dropdown;

    RenderTarget m_preview;
    PerspectiveCamera m_camera;
    Scene m_scene;
};

#endif
