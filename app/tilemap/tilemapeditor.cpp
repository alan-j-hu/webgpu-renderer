#include "tilemapeditor.h"
#include "noworry/grid.h"

#include "imgui.h"

TilemapEditor::TilemapEditor(AppState& app_state)
    : m_camera_selection(0),
      m_app_state(app_state),
      m_selected_layer {-1},
      m_selected_tile {-1},
      m_subwindow(app_state.renderer().device(), 500, 500),
      m_scene(app_state.renderer(), m_camera),
      m_grid_mesh(
          create_grid(app_state.renderer().device(),
                      glm::vec3(0, 0, 0),
                      16,
                      16,
                      glm::vec3(16, 0, 0),
                      glm::vec3(0, 16, 0)))
{
    m_subwindow.set_clear_color(app_state.background_color());

    m_camera.set_position(glm::vec3(8.0f, -1.0f, 10.0f));
    m_camera.set_target(glm::vec3(8.0f, 8.0f, 0.0f));

    m_ortho_camera.set_position(glm::vec3(0.0f, 0.0f, 10.0f));
    m_ortho_camera.set_target(glm::vec3(0.0f, 0.0f, 0.0f));
}

void TilemapEditor::render()
{
    render_preview();
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
        ImGui::Image((ImTextureID)(intptr_t)m_subwindow.texture().view(),
                     ImVec2(m_subwindow.width(), m_subwindow.height()));
    }
    ImGui::EndChild();

    ImGui::SameLine();

    if (ImGui::BeginChild("Side Pane", ImVec2(200, 200))) {
        if (ImGui::BeginListBox("Tiles ##Tiles", ImVec2(-FLT_MIN, 0))) {
            for (int i = 0; i < project.tile_defs().size(); ++i) {
                const TileDef& defn = project.tile_defs().at(i);
                if (ImGui::Selectable(std::to_string(i).c_str(),
                                      i == m_selected_tile)) {
                    m_selected_tile = i;
                }
            }
            ImGui::EndListBox();
        }

        if (ImGui::Button("Add Layer")) {
            m_app_state.set_project(project.map_layers([](auto& layers) {
                Layer layer;
                return layers.push_back(layer);
            }));
        }
        if (ImGui::BeginListBox("##Meshes", ImVec2(-FLT_MIN, 0))) {
            for (int i = 0; i < project.layers().size(); ++i) {
                const Layer& layer = project.layers().at(i);
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
    Frame frame(m_app_state.renderer(), m_subwindow, m_scene);
    frame.add(m_transform, m_grid_mesh, m_app_state.wireframe_material());

    auto& project = m_app_state.project();
    for (auto& layer : project.layers()) {
        for (int i = 0; i < 16; ++i) {
            for (int j = 0; j < 16; ++j) {
                auto& opt = layer.at(i, j);
                if (opt) {
                  auto& inst = opt.value();
                  auto& def = project.tile_defs().at(inst.def);

                  if (def.mesh) {
                      auto it = m_app_state.mesh_map().find(*def.mesh.value());
                      if (it == m_app_state.mesh_map().end()) {
                          continue;
                      }

                      Transform transform;
                      transform.set_translation(glm::vec3(i, j, inst.z));
                      frame.add(transform,
                                it->second->rotated(def.rotation).mesh(),
                                *def.texture.value().material);
                  }
                }
            }
        }
    }
}

void TilemapEditor::unproject()
{
    auto& project = m_app_state.project();

    glm::vec2 pos =
        glm::vec2(m_mouse_rel_x / m_subwindow.width(),
                  m_mouse_rel_y / m_subwindow.height());
    pos.x = 2 * pos.x - 1;
    pos.y = 2 * pos.y - 1;
    auto world = m_ortho_camera.unproject(glm::vec3(pos, 1));
    ImGui::Text("%f, %f\n%f, %f", pos.x, pos.y, world.x, world.y);

    if (ImGui::IsMouseDown(0)) {
        if (world.x >= 0 && world.x < 16 && world.y >= 0 && world.y < 16) {
            int x = (int) world.x;
            int y = (int) world.y;

            if (m_selected_layer != -1 && m_selected_tile != -1) {
                m_app_state.set_project(project.map_layers([&](auto layers) {
                    auto& layer = project.layers().at(m_selected_layer);
                    TileInst inst;
                    inst.def = m_selected_tile;
                    inst.z = 0;
                    return layers.set(m_selected_layer, layer.set(x, y, inst));
                }));
            }
        }
    }
}
