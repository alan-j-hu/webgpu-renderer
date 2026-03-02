#include "TileDefinitionEditor.h"
#include "../FileDialog.h"
#include "noworry/Resource/ResourceLoader.h"
#include "noworry/resourcetable.h"
#include "noworry/Gfx3D/ModelInstance.h"
#include "imgui.h"

namespace fs = std::filesystem;

TileDefinitionEditor::TileDefinitionEditor(AppState& app_state)
    : m_app_state(&app_state),
      m_file_dialog("Choose Model", fs::current_path()),
      m_preview(app_state.renderer().device(), 200, 200),
      m_render_ctx(app_state.renderer())
{
    m_preview.set_clear_color(app_state.background_color());

    m_camera.set_position(glm::vec3(2.0f, -0.5f, 2.0f));
    m_camera.set_target(glm::vec3(2.0f, 0.5f, 1.0f));
}

std::optional<std::unique_ptr<TileDef>>
TileDefinitionEditor::render(const TileDef& definition)
{
    m_app_state->renderer().render(
        m_render_ctx,
        *this,
        m_preview,
        m_camera);

    auto new_definition = std::make_unique<TileDef>(definition);
    bool changed = false;

    if (ImGui::Button("Choose File")) {
        m_file_dialog.open();
    }

    auto model_opt = definition.model();
    auto model_data_opt = definition.model_data();

    if (auto selection = m_file_dialog.update()) {
        std::filesystem::path& path = *selection;
        model_opt = m_app_state->resources().load<Model>(path);
        model_data_opt = m_app_state->resources().load<ModelData>(path);

        new_definition->set_model_path(std::move(path));
        new_definition->set_model(model_opt);
        new_definition->set_model_data(model_data_opt);
        changed = true;
    }

    if (model_opt && model_data_opt) {
        m_model = model_opt;
        m_instance.set_model(model_opt->get());
    } else {
        m_instance.set_model(nullptr);
    }

    int width = definition.width();
    ImGui::InputInt("Width", &width, 1);
    new_definition->set_width(width);
    changed = changed || (new_definition->width() != definition.width());

    int depth = definition.depth();
    ImGui::InputInt("Depth", &depth, 1);
    new_definition->set_depth(depth);
    changed = changed || (new_definition->depth() != definition.depth());

    ImGui::Image((ImTextureID)(intptr_t)
                 m_preview.texture().view(),
                 ImVec2(150, 150));

    if (changed) {
        return std::make_optional(std::move(new_definition));
    }
    return std::nullopt;
}

void TileDefinitionEditor::render(RenderContext& ctx)
{
    ctx.add(m_app_state->small_grid_mesh(),
            m_app_state->wireframe_material(),
            glm::mat4(1));
    m_instance.render(ctx);
}
