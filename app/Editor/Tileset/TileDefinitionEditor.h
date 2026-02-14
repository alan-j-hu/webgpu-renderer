#ifndef TILEDEFINITIONEDITOR_H
#define TILEDEFINITIONEDITOR_H

#include "../FileDialog.h"
#include "../../AppState.h"
#include "noworry/rendertarget.h"
#include "noworry/transform.h"
#include "noworry/camera/perspectivecamera.h"
#include "noworry/Gfx3D/ModelInstance.h"
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

    std::optional<std::unique_ptr<TileDef>> render(const TileDef& definition);

private:
    AppState* m_app_state;
    FileDialog m_file_dialog;
    std::optional<std::shared_ptr<Model>> m_model;

    RenderTarget m_preview;
    PerspectiveCamera m_camera;
    Scene m_scene;
    ModelInstance* m_instance;

    void render_preview();
};

#endif
