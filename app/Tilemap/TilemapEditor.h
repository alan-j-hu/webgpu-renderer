#ifndef TILEMAPEDITOR_H
#define TILEMAPEDITOR_H

#include "HeightMode.h"
#include "LayerNode.h"
#include "LevelNode.h"
#include "Mode.h"
#include "TileMode.h"
#include "View3DMode.h"
#include "ZPalette.h"
#include "../AppState.h"
#include "../TilePicker.h"

#include "noworry/renderer.h"
#include "noworry/rendertarget.h"
#include "noworry/Draw2D/Spritesheet.h"
#include "noworry/Gfx3D/BasicMesh.h"
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

    ~TilemapEditor();

    const ZPalette& z_palette() const;

    void render();

    glm::vec3 map_2d_to_3d(const glm::vec2&) const;
    glm::vec2 map_3d_to_2d(const glm::vec3&) const;

    std::optional<std::pair<int, int>> mouseover_cell();

    glm::vec2 mouse_pos() const;

    int selected_layer() const { return m_selected_layer; }

    LevelNode& level_node() { return m_level_node; }

private:
    int m_camera_selection;

    AppState& m_app_state;
    // Must initialize RenderTarget before spritesheet
    RenderTarget m_subwindow_2d;
    RenderTarget m_subwindow_3d;
    Spritesheet m_spritesheet;

    int m_selected_layer;

    float m_mouse_rel_x;
    float m_mouse_rel_y;

    Scene m_scene;
    Transform m_transform;
    OrthographicCamera m_ortho_camera;
    PerspectiveCamera m_persp_camera;
    Camera* m_current_camera;

    HeightMode m_height_mode;
    TileMode m_tile_mode;
    View3DMode m_view_3d_mode;
    Mode* m_current_mode;

    BasicMesh m_grid_mesh;

    ZPalette m_z_palette;

    LevelNode m_level_node;

    void render_preview();

    void draw_toolbar();

    void draw_layer_list();

    void draw_layer_item(int i);
};

#endif
