#ifndef APPSTATE_H
#define APPSTATE_H

#include "Project.h"
#include "TileThumbnail.h"
#include "Commands/Command.h"
#include "Editor/Modal.h"

#include "noworry/renderer.h"
#include "noworry/Draw2D/SpriteRenderer.h"
#include "noworry/rendertarget.h"
#include "noworry/resourcetable.h"
#include "noworry/camera/orthographiccamera.h"
#include "noworry/scene/scene.h"

#include <vector>
#include <webgpu/webgpu.h>

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

    void undo();

    void redo();

    ThumbnailCapture& thumbnail_capture() { return m_capture; }

    TileThumbnail& tile_thumbnail(int i)
    {
        return m_thumbnail_cache.at(i);
    }

    void refresh_thumbnails();

private:
    std::vector<std::unique_ptr<Command>> m_history;
    int m_history_cursor;

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
