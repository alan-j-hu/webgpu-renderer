#include "TileDefinitionEditor.h"
#include "RotationDropdown.h"
#include "../FileDialog.h"
#include "noworry/Resource/ResourceLoader.h"
#include "noworry/resourcetable.h"
#include "noworry/Gfx3D/ModelInstance.h"
#include "imgui.h"

namespace fs = std::filesystem;

TileDefinitionEditor::TileDefinitionEditor(AppState& app_state)
    : m_app_state(&app_state),
      m_preview(app_state.renderer().device(), 200, 200),
      m_scene(app_state.renderer(), m_camera)
{
    m_preview.set_clear_color(app_state.background_color());

    m_camera.set_position(glm::vec3(2.0f, -0.5f, 2.0f));
    m_camera.set_target(glm::vec3(2.0f, 0.5f, 1.0f));
}

std::optional<TileDef>
TileDefinitionEditor::render(const TileDef& definition)
{
    render_preview();

    TileDef new_definition = definition;
    bool changed = false;

    rotation_dropdown(new_definition.rotation);
    if (new_definition.rotation != definition.rotation) {
        changed = true;
    }

    if (ImGui::Button("Choose File")) {
        m_app_state->modals().push(
            std::make_unique<FileDialog>(fs::current_path(), m_sink));
    }

    if (m_sink.size() > 0) {
        std::filesystem::path& path = m_sink[0];
        auto model_opt = m_app_state->resources().load<Model>(path);
        auto model_data_opt = m_app_state->resources().load<ModelData>(path);

        if (model_opt && model_data_opt) {
            m_model = model_opt;

            m_scene.children().clear();

            m_scene.children().push_back(std::make_unique<RenderObject>(
                m_app_state->renderer(),
                Transform(),
                m_app_state->small_grid_mesh(),
                m_app_state->wireframe_material()));

            auto inst = std::make_unique<ModelInstance>(**model_opt);
            m_scene.children().push_back(std::move(inst));

            new_definition.model = model_opt;
            new_definition.model_data = model_data_opt;
            changed = true;
        }

        m_sink.clear();
    }

    ImGui::Image((ImTextureID)(intptr_t)
                 m_preview.texture().view(),
                 ImVec2(150, 150));

    if (changed) {
        return std::make_optional<TileDef>(new_definition);
    }
    return std::nullopt;
}

void TileDefinitionEditor::render_preview()
{
    m_app_state->renderer().render(m_preview, m_scene);
}
