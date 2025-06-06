#ifndef TILEDEFINITIONEDITOR_H
#define TILEDEFINITIONEDITOR_H

#include "tiledefinition.h"
#include "../appstate.h"
#include "noworry/rendertarget.h"
#include "noworry/transform.h"
#include "noworry/camera/perspectivecamera.h"
#include "noworry/material/texturematerial.h"
#include "noworry/scene/scene.h"
#include <filesystem>
#include <memory>
#include <vector>

/// Editor for tile definition. Also used in AddNewTile.
class TileDefinitionEditor
{
public:
    TileDefinitionEditor(AppState&);

    void render(TileDefinition& definition);

private:
    AppState* m_app_state;
    std::vector<std::filesystem::path> m_sink;

    RenderTarget m_preview;
    PerspectiveCamera m_camera;
    Scene m_scene;
    Transform m_transform;

    void draw_frame(TileDefinition& definition);

    Material& get_material(TileDefinition& definition);
};

#endif
