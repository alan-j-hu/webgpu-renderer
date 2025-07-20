#include "TilemapEditor.h"
#include "../Commands/CreateLayerCommand.h"
#include "../Commands/PlaceTileCommand.h"
#include "noworry/grid.h"

#include "imgui.h"

TilemapEditor::TilemapEditor(AppState& app_state)
    : m_camera_selection(0),
      m_app_state(app_state),
      m_selected_layer {-1},
      m_selected_tile {-1},
      m_subwindow_2d(app_state.renderer().device(), 500, 500),
      m_subwindow_3d(app_state.renderer().device(), 500, 500),
      m_spritesheet(app_state.renderer().device(),
                    app_state.sprite_renderer().pipeline(),
                    m_subwindow_3d.texture(),
                    app_state.renderer().default_sampler()),
      m_scene(app_state.renderer(), m_camera),
      m_cursor_overlay(app_state, *this),
      m_grid_mesh(
          create_grid(app_state.renderer().device(),
                      glm::vec3(0, 0, 0),
                      16,
                      16,
                      glm::vec3(16, 0, 0),
                      glm::vec3(0, 16, 0))),
      m_tile_picker(app_state)
{
    //m_subwindow_2d.set_clear_color(app_state.background_color());
    m_subwindow_3d.set_clear_color(app_state.background_color());

    m_camera.set_position(glm::vec3(8.0f, -1.0f, 10.0f));
    m_camera.set_target(glm::vec3(8.0f, 8.0f, 0.0f));

    m_ortho_camera.set_position(glm::vec3(0.0f, 0.0f, 10.0f));
    m_ortho_camera.set_target(glm::vec3(0.0f, 0.0f, 0.0f));
}

glm::vec3 TilemapEditor::map_2d_to_3d(const glm::vec2& vec2d) const
{
    glm::vec2 v = vec2d;
    v.x /= m_subwindow_2d.width() / 2;
    v.y /= m_subwindow_2d.height() / 2;
    v.x -= 1;
    v.y -= 1;
    v.y *= -1;
    return m_ortho_camera.unproject(glm::vec3(v, 0));
}

glm::vec2 TilemapEditor::map_3d_to_2d(const glm::vec3& vec3d) const
{
    glm::vec2 v = m_ortho_camera.project(vec3d);
    v.y *= -1;
    v.x += 1;
    v.y += 1;
    v.x *= m_subwindow_2d.width() / 2;
    v.y *= m_subwindow_2d.height() / 2;
    return v;
}

std::optional<std::pair<int, int>> TilemapEditor::mouseover_cell()
{
    glm::vec2 pos = mouse_pos();
    glm::vec3 pos_3d = map_2d_to_3d(pos);

    if (pos_3d.x < 0 || pos_3d.x >= 16) {
        return std::nullopt;
    }
    if (pos_3d.y < 0 || pos_3d.y >= 16) {
        return std::nullopt;
    }

    int x = (int) pos_3d.x;
    int y = (int) pos_3d.y;

    return std::optional(std::pair<int, int>(x, y));
}

glm::vec2 TilemapEditor::mouse_pos() const
{
    return glm::vec2(m_mouse_rel_x, m_mouse_rel_y);
}

void TilemapEditor::render()
{
    render_preview();

    {
        m_app_state.sprite_renderer().begin(m_subwindow_2d);

        Region src;
        src.x = 0;
        src.y = 0;
        src.w = 1;
        src.h = 1;

        Region dest;
        dest.x = 0;
        dest.y = 0;
        dest.w = m_subwindow_2d.width();
        dest.h = m_subwindow_2d.height();

        m_app_state.sprite_renderer().draw(m_spritesheet, dest, src);
        m_cursor_overlay.draw(m_subwindow_2d, m_app_state.sprite_renderer());

        m_app_state.sprite_renderer().end();
    }

    auto project = m_app_state.project();

    if (ImGui::BeginChild("Map", ImVec2(500, 700))) {
        const char* items[] = {
            "Perspective",
            "Orthographic"
        };

        if (ImGui::BeginCombo("##camera", items[m_camera_selection])) {
            for (int i = 0; i < IM_ARRAYSIZE(items); ++i) {
                bool is_selected = i == m_camera_selection;
                if (ImGui::Selectable(items[i], is_selected)) {
                    m_camera_selection = i;
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (m_camera_selection == 0) {
            m_scene.set_camera(m_camera);
        } else {
            m_scene.set_camera(m_ortho_camera);
        }

        ImVec2 screen_pos = ImGui::GetCursorScreenPos();
        ImVec2 mouse_pos = ImGui::GetMousePos();
        m_mouse_rel_x = mouse_pos.x - screen_pos.x;
        m_mouse_rel_y = mouse_pos.y - screen_pos.y;
        ImGui::Image((ImTextureID)(intptr_t)m_subwindow_2d.texture().view(),
                     ImVec2(m_subwindow_2d.width(), m_subwindow_2d.height()));
    }
    ImGui::EndChild();

    ImGui::SameLine();

    if (ImGui::BeginChild("Side Pane", ImVec2(200, 200))) {
        m_tile_picker.render(m_selected_tile);

        if (ImGui::Button("Add Layer")) {
            m_app_state.push_command(std::make_unique<CreateLayerCommand>());
        }
        if (ImGui::BeginListBox("##Meshes", ImVec2(-FLT_MIN, 0))) {
            for (int i = 0; i < project.layer_count(); ++i) {
                const Layer& layer = project.layer_at(i);
                bool selected = i == m_selected_layer;
                if (ImGui::Selectable(std::to_string(i).c_str(), selected)) {
                    m_selected_layer = i;
                }
            }
          ImGui::EndListBox();
        }

        unproject();
    }
    ImGui::EndChild();
}

void TilemapEditor::render_preview()
{
    Frame frame(m_app_state.renderer(), m_subwindow_3d, m_scene);
    frame.add(m_transform, m_grid_mesh, m_app_state.wireframe_material());

    auto& project = m_app_state.project();
    for (int i = 0; i < project.layer_count(); ++i) {
        auto& layer = project.layer_at(i);
        for (int y = 0; y < 16; ++y) {
            for (int x = 0; x < 16; ++x) {
                auto& opt = layer.at(x, y);
                if (!opt) {
                    continue;
                }

                auto& inst = opt.value();
                ResolvedTile resolved = m_app_state.resolve_tile(inst.def());
                if (resolved.mesh == nullptr) {
                    continue;
                }
                Material& material = resolved.material == nullptr
                  ? m_app_state.default_material()
                  : *resolved.material;

                Transform transform;
                transform.set_translation(glm::vec3(x, y, inst.z()));
                frame.add(transform, resolved.mesh->mesh(), material);
            }
        }
    }
}

void TilemapEditor::unproject()
{
    auto& project = m_app_state.project();

    if (ImGui::IsMouseDown(0)) {
        auto cell_opt = mouseover_cell();
        if (cell_opt) {
            int x = cell_opt->first;
            int y = cell_opt->second;

            if (m_selected_layer != -1 && m_selected_tile != -1) {
                m_app_state.push_command(std::make_unique<PlaceTileCommand>(
                    m_selected_layer, x, y, 0,
                    project.tiledef_at(m_selected_tile)
                ));
            }
        }
    }
}
