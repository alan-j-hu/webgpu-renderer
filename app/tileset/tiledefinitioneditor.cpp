#include "tiledefinitioneditor.h"
#include "rotationdropdown.h"
#include "../filedialog.h"
#include "noworry/resourcetable.h"
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
    TileDef new_definition = definition;
    bool changed = false;

    const char* label = "Choose Mesh";
    TileRotations* rotations = nullptr;
    if (definition.mesh) {
        label = definition.mesh.value()->c_str();
        auto it = m_app_state->mesh_map().find(*definition.mesh.value());
        if (it != m_app_state->mesh_map().end()) {
            rotations = it->second;
        }
    }

    if (ImGui::BeginCombo("Shape", label)) {
        for (auto& pair : m_app_state->mesh_map()) {
            if (ImGui::Selectable(pair.second->name().c_str(), false)) {
                changed = true;
                rotations = pair.second;
                new_definition.mesh =
                    std::make_shared<std::string>(rotations->name());
            }
        }
        ImGui::EndCombo();
    }

    rotation_dropdown(new_definition.rotation);
    if (new_definition.rotation != definition.rotation) {
        changed = true;
    }

    if (ImGui::Button("Choose Texture")) {
        m_app_state->modals().push(
            std::make_unique<FileDialog>(fs::current_path(), m_sink));
    }

    TextureMaterial* material = nullptr;
    if (definition.texture) {
        material = definition.texture.value().material.get();
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

                material = texture_ref.material.get();
            }
        }
        m_sink.clear();
    }

    if (material != nullptr) {
        ImGui::Image((ImTextureID)(intptr_t)
                     material->texture().view(),
                     ImVec2(50, 50));
    }

    draw_frame(definition, rotations, material);

    ImGui::Image((ImTextureID)(intptr_t)m_preview.texture().view(),
                 ImVec2(m_preview.width(), m_preview.height()));

    if (changed) {
        return std::make_optional<TileDef>(new_definition);
    }
    return std::nullopt;
}

void TileDefinitionEditor::draw_frame(
    const TileDef& definition,
    TileRotations* rotations,
    Material* material)
{
    Frame frame(m_app_state->renderer(), m_preview, m_scene);

    Transform transform;
    frame.add(transform,
              m_app_state->small_grid_mesh(),
              m_app_state->wireframe_material());

    if (material == nullptr) {
        material = &m_app_state->default_material();
    }

    if (rotations != nullptr) {
        frame.add(transform,
                  rotations->rotated(definition.rotation).mesh(),
                  *material);
    }
}
