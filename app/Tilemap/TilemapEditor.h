#ifndef TILEMAPEDITOR_H
#define TILEMAPEDITOR_H

#include "CursorOverlay.h"
#include "../AppState.h"
#include "../TilePicker.h"

#include "noworry/mesh.h"
#include "noworry/renderer.h"
#include "noworry/rendertarget.h"
#include "noworry/Draw2D/SpriteBatch.h"
#include "noworry/Draw2D/Spritesheet.h"
#include "noworry/transform.h"
#include "noworry/camera/orthographiccamera.h"
#include "noworry/camera/perspectivecamera.h"
#include "noworry/scene/scene.h"

#include <memory>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

/// Level map editor.
class TilemapEditor
{
public:
    TilemapEditor(AppState&);

    void render();

    glm::vec3 map_2d_to_3d(const glm::vec2&) const;
    glm::vec2 map_3d_to_2d(const glm::vec3&) const;

    glm::vec2 mouse_pos() const;

    int selected_tile() const { return m_selected_tile; }

private:
    int m_camera_selection;

    AppState& m_app_state;
    // Must initialize RenderTarget before spritesheet
    RenderTarget m_subwindow_2d;
    RenderTarget m_subwindow_3d;
    Spritesheet m_spritesheet;
    TilePicker m_tile_picker;

    int m_selected_layer;
    int m_selected_tile;

    float m_mouse_rel_x;
    float m_mouse_rel_y;

    Scene m_scene;
    Transform m_transform;
    OrthographicCamera m_ortho_camera;
    PerspectiveCamera m_camera;

    CursorOverlay m_cursor_overlay;

    Mesh m_grid_mesh;

    void render_preview();

    void unproject();
};

#endif
