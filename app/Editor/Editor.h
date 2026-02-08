#ifndef EDITOR_H
#define EDITOR_H

#include "FileDialog.h"
#include "ErrorModal.h"
#include "ExportDialog.h"
#include "Tilemap/HeightMode.h"
#include "Tilemap/LayerNode.h"
#include "Tilemap/LevelNode.h"
#include "Tilemap/Mode.h"
#include "Tilemap/TileMode.h"
#include "Tilemap/View3DMode.h"
#include "Tilemap/ZPalette.h"
#include "Tileset/TileList.h"
#include "Tileset/TilesetThumbnails.h"
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
#include <unordered_map>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

/// Level map editor.
class Editor
{
public:
    Editor(AppState&);

    ~Editor();

    const ZPalette& z_palette() const;

    void draw();

    glm::vec3 map_2d_to_3d(const glm::vec2&) const;
    glm::vec2 map_3d_to_2d(const glm::vec3&) const;

    bool viewer_mouse_down() const { return m_viewer_mouse_down; }

    bool viewer_mouse_over() const { return m_viewer_mouse_over; }

    std::optional<std::pair<int, int>> mouseover_cell();

    glm::vec2 mouse_pos() const;

    const LayerLocation& selected_layer() const { return m_selected_layer; }

    TilesetThumbnails& tileset_thumbnails(int idx)
    {
        return *m_tileset_thumbnails.at(idx);
    }

    void open_error_modal(const std::string& message);

private:
    enum class IOState
    {
        NONE,
        EXPORTING,
        OPENING,
        SAVING
    } m_io_state;

    int m_camera_selection;

    AppState* m_app_state;
    ExportDialog m_export_dialog;
    FileDialog m_main_file_dialog;
    ErrorModal m_error_modal;

    TileList m_tile_list;

    // Must initialize RenderTarget before spritesheet
    RenderTarget m_subwindow_2d;
    RenderTarget m_subwindow_3d;
    Spritesheet m_spritesheet;
    bool m_viewer_mouse_down;
    bool m_viewer_mouse_over;

    LayerLocation m_selected_layer;

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

    std::vector<std::unique_ptr<TilesetThumbnails>> m_tileset_thumbnails;
    std::unordered_map<const Level*, std::unique_ptr<LevelNode>> m_level_nodes;

    void render_preview();

    void draw_menubar();

    void draw_main_pane();

    void draw_tileset_editor();

    void draw_tilemap_editor();

    void draw_toolbar();

    void draw_layer_list();

    void draw_layer_item(int i);

    void draw_error_modal();

    void load_project(const std::filesystem::path&);

    void save_project(const std::filesystem::path&);

    void destroy_scene();

    void setup_scene();
};

#endif
