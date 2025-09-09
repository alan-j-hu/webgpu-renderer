#ifndef APPSTATE_H
#define APPSTATE_H

#include "Modal.h"
#include "Reducer.h"
#include "TileThumbnail.h"
#include "Commands/Command.h"
#include "Tileset/RotationDropdown.h"
#include "Tileset/TileMesh.h"
#include "Tileset/TileRotations.h"

#include "noworry/renderer.h"
#include "noworry/Draw2D/SpriteRenderer.h"
#include "noworry/rendertarget.h"
#include "noworry/resourcetable.h"
#include "noworry/camera/orthographiccamera.h"
#include "noworry/scene/scene.h"

#include <vector>
#include <webgpu/webgpu.h>

class TilemapEditor;

/// Helper class containing data used by multiple sub-editors.
class AppState
{
public:
    AppState(WGPUDevice);

    WGPUColor background_color() { return m_background_color; }

    Renderer& renderer() { return m_renderer; }
    SpriteRenderer& sprite_renderer() { return m_sprite_renderer; }
    ModalStack& modals() { return m_modals; }
    ResourceTable& resources() { return m_resources; }

    Material& default_material() { return *m_default_material; }
    Material& wireframe_material() { return *m_wireframe_material; }
    BasicMesh& small_grid_mesh() { return m_small_grid_mesh; }

    const Project& project() const { return m_project; }

    void push_command(std::unique_ptr<Command>);

    TileThumbnail& tile_thumbnail(int i)
    {
        return m_thumbnail_cache.at(i);
    }
    void refresh_thumbnails();

    void connect_tilemap_editor(TilemapEditor&);

    void disconnect_tilemap_editor(TilemapEditor&);

private:
    std::vector<std::unique_ptr<Command>> m_history;

    WGPUColor m_background_color;
    Renderer m_renderer;
    SpriteRenderer m_sprite_renderer;
    ModalStack m_modals;
    ResourceTable m_resources;

    Material* m_default_material;
    Material* m_wireframe_material;
    BasicMesh m_small_grid_mesh;

    Project m_project;

    ThumbnailCapture m_capture;
    std::vector<TileThumbnail> m_thumbnail_cache;
};

#endif
