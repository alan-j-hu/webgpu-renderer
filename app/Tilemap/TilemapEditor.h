#ifndef TILEMAPEDITOR_H
#define TILEMAPEDITOR_H

#include "LayerNode.h"
#include "Mode.h"
#include "TileMode.h"
#include "View3DMode.h"
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

    void render();

    glm::vec3 map_2d_to_3d(const glm::vec2&) const;
    glm::vec2 map_3d_to_2d(const glm::vec3&) const;

    std::optional<std::pair<int, int>> mouseover_cell();

    glm::vec2 mouse_pos() const;

    int selected_layer() const { return m_selected_layer; }

    Project::Listener& add_layer_listener() { return m_layer_listener; }

private:
    class LayerListener : public Project::Listener
    {
    public:
        LayerListener(TilemapEditor&);

        virtual void add_layer(Layer&) override;
        virtual void remove_layer(const int& index) override;

    private:
        TilemapEditor* m_editor;
    };

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
    PerspectiveCamera m_camera;

    TileMode m_tile_mode;
    View3DMode m_view_3d_mode;
    Mode* m_current_mode;

    BasicMesh m_grid_mesh;

    std::vector<LayerNode*> m_layer_nodes;
    LayerListener m_layer_listener;

    void render_preview();

    void draw_toolbar();

    void add_layer(Layer&);

    void remove_layer(int index);
};

#endif
