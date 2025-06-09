#include "tilemapeditor.h"
#include "noworry/grid.h"

#include "imgui.h"

TilemapEditor::TilemapEditor(AppState& app_state)
    : m_camera_selection(0),
      m_app_state(app_state),
      m_selected_layer {nullptr},
      m_selected_tile {nullptr},
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
        Tileset& tileset = m_app_state.tileset();
        if (ImGui::BeginListBox("Tiles ##Tiles", ImVec2(-FLT_MIN, 0))) {
            for (int i = 0; i < tileset.tile_count(); ++i) {
                TileDefinition& defn = tileset.tile_at(i);
                if (ImGui::Selectable(std::to_string(i).c_str(),
                                      &defn == m_selected_tile)) {
                    m_selected_tile = &defn;
                }
            }
            ImGui::EndListBox();
        }

        if (ImGui::Button("Add Layer")) {
            m_layers.push_back(std::make_unique<TileLayer>());
        }
        if (ImGui::BeginListBox("##Meshes", ImVec2(-FLT_MIN, 0))) {
            for (int i = 0; i < m_layers.size(); ++i) {
                TileLayer* layer = m_layers[i].get();
                bool selected = layer == m_selected_layer;
                if (ImGui::Selectable(std::to_string(i).c_str(), selected)) {
                    m_selected_layer = layer;
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
    for (auto& layer : m_layers) {
        layer->render(frame);
    }
}

void TilemapEditor::unproject()
{
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

            if (m_selected_layer != nullptr && m_selected_tile != nullptr) {
                m_selected_layer->at(x, y) =
                    TileInstance(m_app_state, *m_selected_tile, 0);
            }
        }
    }
}
