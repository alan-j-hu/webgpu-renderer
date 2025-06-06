#ifndef TILESET_MODELINSPECTOR_H
#define TILESET_MODELINSPECTOR_H

#include "tilerotations.h"
#include "tileset.h"
#include "../appstate.h"
#include "../modal.h"
#include "../pane.h"
#include "noworry/mesh.h"
#include "noworry/renderer.h"
#include "noworry/rendertarget.h"
#include "noworry/resourcetable.h"
#include "noworry/transform.h"
#include "noworry/camera/perspectivecamera.h"
#include "noworry/scene/scene.h"

#include <filesystem>
#include <vector>

/// Allows the loading and viewing of tile shapes from a 3D model
/// library file.
class ModelInspector : public Pane
{
public:
    ModelInspector(std::string name, int flex, AppState&);

    Mesh& grid_mesh() { return m_grid_mesh; }

protected:
    virtual void content() override;

private:
    AppState* m_app_state;

    TileRotations* m_selected_tile = nullptr;
    RotationTag m_rotation;

    RenderTarget m_tile_preview;
    Transform m_transform;
    Mesh m_grid_mesh;

    Scene m_scene;
    PerspectiveCamera m_camera;

    std::vector<std::filesystem::path> m_sink;

    bool render_preview();
};

#endif
