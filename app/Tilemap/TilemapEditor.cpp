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
                    app_state.renderer_2d().pipeline(),
                    m_subwindow_3d.texture(),
                    app_state.renderer().default_sampler()),
      m_scene(app_state.renderer(), m_camera),
      m_sprite_batch(app_state.renderer().device(), 100),
      m_grid_mesh(
          create_grid(app_state.renderer().device(),
                      glm::vec3(0, 0, 0),
                      16,
                      16,
                      glm::vec3(16, 0, 0),
                      glm::vec3(0, 16, 0))),
      m_tile_picker(app_state)
{
    m_subwindow_2d.set_clear_color(app_state.background_color());
    m_subwindow_3d.set_clear_color(app_state.background_color());

    m_camera.set_position(glm::vec3(8.0f, -1.0f, 10.0f));
    m_camera.set_target(glm::vec3(8.0f, 8.0f, 0.0f));

    m_ortho_camera.set_position(glm::vec3(0.0f, 0.0f, 10.0f));
    m_ortho_camera.set_target(glm::vec3(0.0f, 0.0f, 0.0f));
}

void TilemapEditor::render()
{
    render_preview();

    {
        Frame2D frame = m_app_state.renderer_2d().begin(m_subwindow_2d);
        m_sprite_batch.begin(frame.pass());

        Region src;
        src.x = 0;
        src.y = 0;
        src.w = 1;
        src.h = 1;

        Region dest;
        dest.x = -1;
        dest.y = -1;
        dest.w = 1;
        dest.h = 1;

        m_sprite_batch.draw(m_spritesheet, dest, src);

        m_sprite_batch.end();
        frame.finish();
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

    glm::vec2 pos =
        glm::vec2(m_mouse_rel_x / m_subwindow_3d.width(),
                  m_mouse_rel_y / m_subwindow_3d.height());
    pos.x = 2 * pos.x - 1;
    pos.y = 2 * pos.y - 1;
    auto world = m_ortho_camera.unproject(glm::vec3(pos, 1));
    ImGui::Text("%f, %f\n%f, %f", pos.x, pos.y, world.x, world.y);

    if (ImGui::IsMouseDown(0)) {
        if (world.x >= 0 && world.x < 16 && world.y >= 0 && world.y < 16) {
            int x = (int) world.x;
            int y = (int) world.y;

            if (m_selected_layer != -1 && m_selected_tile != -1) {
                m_app_state.push_command(std::make_unique<PlaceTileCommand>(
                    m_selected_layer, x, y, 0,
                    project.tiledef_at(m_selected_tile)
                ));
            }
        }
    }
}
