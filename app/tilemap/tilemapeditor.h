#ifndef TILEMAPEDITOR_H
#define TILEMAPEDITOR_H

#include "../appstate.h"

#include "noworry/mesh.h"
#include "noworry/renderer.h"
#include "noworry/rendertarget.h"
#include "noworry/camera/orthographiccamera.h"
#include "noworry/camera/perspectivecamera.h"
#include "noworry/scene/scene.h"

#include <memory>

/// Level map editor.
class TilemapEditor
{
public:
    TilemapEditor(AppState&);

    void render();

private:
    int m_camera_selection;

    AppState& m_app_state;
    RenderTarget m_subwindow;

    Scene m_scene;
    OrthographicCamera m_ortho_camera;
    PerspectiveCamera m_camera;

    Mesh m_grid_mesh;
    std::unique_ptr<RenderObject> m_grid;

    void render_preview();
};

#endif
