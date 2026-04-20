#ifndef TILEDEFINITIONEDITOR_H
#define TILEDEFINITIONEDITOR_H

#include "../FileDialog.h"
#include "../../AppState.h"
#include "noworry/rendertarget.h"
#include "noworry/transform.h"
#include "noworry/camera/perspectivecamera.h"
#include "noworry/Gfx3D/ModelInstance.h"
#include "noworry/Gfx3D/RenderContext.h"
#include "noworry/Material/TextureMaterial.h"
#include <filesystem>
#include <memory>
#include <vector>
#include <glm/ext/vector_int2.hpp>

/// Editor for tile definition. Also used in AddNewTile.
class TileDefinitionEditor : public Renderable
{
public:
    TileDefinitionEditor(AppState&);

    std::optional<std::unique_ptr<TileDef>> render(const TileDef& definition);

    virtual void render(RenderContext&) override;

private:
    AppState* m_app_state;
    FileDialog m_file_dialog;
    std::optional<std::shared_ptr<Model>> m_model;

    RenderTarget m_preview;
    PerspectiveCamera m_camera;
    RenderContext m_render_ctx;
    ModelInstance m_instance;

    std::optional<glm::ivec2> m_selected_collision;

    void render_preview();

    bool collision_matrix(TileDef&);
};

#endif
