#include "tilemapeditor.h"
#include "noworry/grid.h"

#include "imgui.h"

TilemapEditor::TilemapEditor(Renderer& renderer, TilesetEditor& editor)
    : m_camera_selection(0),
      m_renderer(renderer),
      m_subwindow(renderer.device(), 500, 500),
      m_scene(renderer, m_camera),
      m_tileset_editor(&editor),
      m_grid_mesh(
          create_grid(renderer.device(),
                      glm::vec3(0, 0, 0),
                      16,
                      16,
                      glm::vec3(16, 0, 0),
                      glm::vec3(0, 16, 0)))
{
    m_grid = std::make_unique<RenderObject>(
        m_renderer.device(),
        m_grid_mesh,
        m_tileset_editor->wireframe_material());

    m_camera.set_position(glm::vec3(8.0f, 8.0f, 10.0f));
    m_camera.set_target(glm::vec3(8.0f, 8.0f, 0.0f));

    m_ortho_camera.set_position(glm::vec3(0.0f, 1.0f, 10.0f));
    m_ortho_camera.set_target(glm::vec3(0.0f, 0.0f, 0.0f));
}

void TilemapEditor::render()
{
    render_preview();

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

    ImGui::Image((ImTextureID)(intptr_t)m_subwindow.texture().view(),
                 ImVec2(m_subwindow.width(), m_subwindow.height()));
}

void TilemapEditor::render_preview()
{
    Frame frame(m_renderer, m_subwindow, m_scene);
    frame.add_renderobject(*m_grid);
}
