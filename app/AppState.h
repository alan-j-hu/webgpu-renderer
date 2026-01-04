#ifndef APPSTATE_H
#define APPSTATE_H

#include "Deserialize.h"
#include "Project.h"
#include "ThumbnailUtil.h"
#include "Commands/Command.h"

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
    ResourceTable& resources() { return m_resources; }

    Material& default_material() { return *m_default_material; }
    Material& wireframe_material() { return *m_wireframe_material; }
    BasicMesh& small_grid_mesh() { return m_small_grid_mesh; }

    JsonDeserializer& deserializer() { return m_deserializer; }

    const Project& project() const { return m_project; }

    void set_project(Project);

    void push_command(std::unique_ptr<Command>);

    void undo();

    void redo();

    ThumbnailUtil& thumbnail_util() { return m_thumbnail_util; }

private:
    std::vector<std::unique_ptr<Command>> m_history;
    int m_history_cursor;

    WGPUColor m_background_color;
    Renderer m_renderer;
    SpriteRenderer m_sprite_renderer;
    ResourceTable m_resources;
    JsonDeserializer m_deserializer;

    Material* m_default_material;
    Material* m_wireframe_material;
    BasicMesh m_small_grid_mesh;

    Project m_project;

    ThumbnailUtil m_thumbnail_util;
};

#endif
