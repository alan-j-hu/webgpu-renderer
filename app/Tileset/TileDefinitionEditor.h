#ifndef TILEDEFINITIONEDITOR_H
#define TILEDEFINITIONEDITOR_H

#include "../AppState.h"
#include "noworry/rendertarget.h"
#include "noworry/transform.h"
#include "noworry/camera/perspectivecamera.h"
#include "noworry/Material/TextureMaterial.h"
#include "noworry/scene/scene.h"
#include <filesystem>
#include <memory>
#include <vector>

/// Editor for tile definition. Also used in AddNewTile.
class TileDefinitionEditor
{
public:
    TileDefinitionEditor(AppState&);

    std::optional<TileDef> render(const TileDef& definition);

private:
    AppState* m_app_state;
    std::vector<std::filesystem::path> m_sink;

    RenderTarget m_preview;
    PerspectiveCamera m_camera;
    Scene m_scene;

    void draw_frame(const ResolvedTile& resolved);
};

#endif
