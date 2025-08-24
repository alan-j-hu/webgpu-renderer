#include "TileDefinitionEditor.h"
#include "RotationDropdown.h"
#include "../FileDialog.h"
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

    ResolvedTile resolved = m_app_state->resolve_tile(definition);

    const char* label = "Choose Mesh";
    if (definition.mesh) {
        label = definition.mesh.value()->name().c_str();
    }

    if (ImGui::BeginCombo("Shape", label)) {
        for (auto& pair : m_app_state->mesh_map()) {
            if (ImGui::Selectable(pair.second->name().c_str(), false)) {
                changed = true;
                new_definition.mesh = pair.second;
            }
        }
        ImGui::EndCombo();
    }

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
        auto opt = m_app_state->resources().load_model(path);

        if (opt) {
            m_model = opt;

            m_scene.children().clear();

            m_scene.children().push_back(std::make_unique<RenderObject>(
                m_app_state->renderer(),
                Transform(),
                m_app_state->small_grid_mesh(),
                m_app_state->wireframe_material()));

            auto inst = std::make_unique<ModelInstance>(**opt);
            m_scene.children().push_back(std::move(inst));
        }

        m_sink.clear();
    }

    if (ImGui::Button("Choose Texture")) {
        m_app_state->modals().push(
            std::make_unique<FileDialog>(fs::current_path(), m_sink));
    }

    if (m_sink.size() > 0) {
        std::filesystem::path& path = m_sink[0];
        if (!definition.texture || path != definition.texture.value().path) {
            changed = true;
            auto optional =
                m_app_state->resources().load_texture_material(path);
            if (optional) {
                TextureRef texture_ref;
                texture_ref.path = path;
                texture_ref.material = optional.value();
                new_definition.texture =
                    std::make_optional<TextureRef>(texture_ref);
            }
        }
        m_sink.clear();
    }

    if (resolved.material != nullptr) {
        ImGui::Image((ImTextureID)(intptr_t)
                     resolved.material->texture().view(),
                     ImVec2(50, 50));
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
