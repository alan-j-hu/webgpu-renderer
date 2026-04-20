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

    ImGui::Image((ImTextureID)(intptr_t)
                 m_preview.texture().view(),
                 ImVec2(150, 150));

    int width = definition.width();
    ImGui::InputInt("Width", &width, 1);
    new_definition->set_width(width);
    changed = changed || (new_definition->width() != definition.width());

    int depth = definition.depth();
    ImGui::InputInt("Depth", &depth, 1);
    new_definition->set_depth(depth);
    changed = changed || (new_definition->depth() != definition.depth());

    if (collision_matrix(*new_definition)) {
        changed = true;
    }

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

bool TileDefinitionEditor::collision_matrix(TileDef& tiledef)
{
    const char* labels[] = { "0", "1", "2", "3", "4" };

    bool changed = false;
    int width = tiledef.width();
    int depth = tiledef.depth();

    ImGuiTableFlags flags =
        ImGuiTableFlags_SizingFixedFit
      | ImGuiTableFlags_Borders
      | ImGuiTableFlags_NoHostExtendX;
    ImVec2 outer_size(0, 0);
    if (ImGui::BeginTable("Collision Matrix", width + 1, flags, outer_size)) {
        auto col_flags =
            ImGuiTableColumnFlags_WidthFixed
          | ImGuiTableColumnFlags_NoResize
          | ImGuiTableColumnFlags_NoReorder;
        ImGui::TableSetupColumn("", col_flags);
        for (int col = 0; col < width; ++col) {
            ImGui::TableSetupColumn(labels[col], col_flags);
        }

        for (int row = depth - 1; row >= 0; --row) {
            ImGui::PushID(row);

            ImGui::TableNextRow(0, 16);

            ImGui::TableNextColumn();
            ImGui::TableHeader(labels[row]);

            for (int col = 0; col < width; ++col) {
                ImGui::PushID(col);
                ImGui::TableNextColumn();

                int collision_val = tiledef.collision_at(col, row);
                ImGui::SetNextItemWidth(32);
                if (ImGui::InputInt("##Collision", &collision_val, 0)) {
                    tiledef.set_collision(col, row, collision_val);
                    changed = true;
                }

                ImGui::PopID();
            }

            ImGui::PopID();
        }

        ImGui::TableHeadersRow();

        ImGui::EndTable();
    }

    return changed;
}
