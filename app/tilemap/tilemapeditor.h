#ifndef TILEMAPEDITOR_H
#define TILEMAPEDITOR_H

#include "../tileset/tileseteditor.h"

#include "noworry/mesh.h"
#include "noworry/renderer.h"
#include "noworry/rendertarget.h"
#include "noworry/camera/orthographiccamera.h"
#include "noworry/camera/perspectivecamera.h"
#include "noworry/scene/scene.h"

#include <memory>

class TilemapEditor
{
public:
    TilemapEditor(Renderer& renderer, TilesetEditor&);

    void render();

private:
    int m_camera_selection;

    Renderer& m_renderer;
    RenderTarget m_subwindow;
    TilesetEditor* m_tileset_editor;

    Scene m_scene;
    OrthographicCamera m_ortho_camera;
    PerspectiveCamera m_camera;

    Mesh m_grid_mesh;
    std::unique_ptr<RenderObject> m_grid;

    void render_preview();
};

#endif
