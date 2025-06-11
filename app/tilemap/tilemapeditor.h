#ifndef TILEMAPEDITOR_H
#define TILEMAPEDITOR_H

#include "tilelayer.h"
#include "../appstate.h"

#include "noworry/mesh.h"
#include "noworry/renderer.h"
#include "noworry/rendertarget.h"
#include "noworry/transform.h"
#include "noworry/camera/orthographiccamera.h"
#include "noworry/camera/perspectivecamera.h"
#include "noworry/scene/scene.h"

#include <memory>
#include <vector>

/// Level map editor.
class TilemapEditor
{
public:
    TilemapEditor(AppState&);

    void render();

private:
    int m_camera_selection;

    AppState& m_app_state;
    std::vector<std::unique_ptr<TileLayer>> m_layers;
    int m_selected_layer;
    int m_selected_tile;

    RenderTarget m_subwindow;
    float m_mouse_rel_x;
    float m_mouse_rel_y;

    Scene m_scene;
    Transform m_transform;
    OrthographicCamera m_ortho_camera;
    PerspectiveCamera m_camera;

    Mesh m_grid_mesh;

    void render_preview();

    void unproject();
};

#endif
