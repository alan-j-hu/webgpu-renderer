#include "TilemapEditor.h"
#include "LayerNode.h"
#include "../Commands/CreateLayerCommand.h"
#include "../Commands/DeleteLayerCommand.h"
#include "../Commands/PlaceTileCommand.h"
#include "noworry/grid.h"

#include "imgui.h"

TilemapEditor::TilemapEditor(AppState& app_state)
    : m_camera_selection(0),
      m_app_state(app_state),
      m_subwindow_2d(app_state.renderer().device(), 500, 500),
      m_subwindow_3d(app_state.renderer().device(), 500, 500),
      m_spritesheet(app_state.renderer().device(),
                    app_state.sprite_renderer().pipeline(),
                    m_subwindow_3d.texture(),
                    app_state.renderer().default_sampler()),
      m_scene(app_state.renderer()),
      m_height_mode(app_state, *this),
      m_tile_mode(app_state, *this),
      m_view_3d_mode(app_state, *this),
      m_current_mode(&m_tile_mode),
      m_grid_mesh(
          create_grid(app_state.renderer().device(),
                      glm::vec3(0, 0, 0),
                      16,
                      16,
                      glm::vec3(16, 0, 0),
                      glm::vec3(0, 16, 0))),
      m_z_palette(app_state),
      m_level_node(app_state, m_selected_layer)
{
    app_state.connect_tilemap_editor(*this);

    //m_subwindow_2d.set_clear_color(app_state.background_color());
    m_subwindow_3d.set_clear_color(app_state.background_color());

    m_persp_camera.set_position(glm::vec3(8.0f, -1.0f, 10.0f));
    m_persp_camera.set_target(glm::vec3(8.0f, 8.0f, 0.0f));

    m_ortho_camera.set_position(glm::vec3(0.0f, 0.0f, 10.0f));
    m_ortho_camera.set_target(glm::vec3(0.0f, 0.0f, 0.0f));

    m_current_camera = &m_persp_camera;

    Transform transform;

    m_scene.children().push_back(std::make_unique<RenderObject>(
        app_state.renderer(),
        transform,
        m_grid_mesh,
        app_state.wireframe_material()
    ));
    m_scene.children().push_back(std::make_unique<RenderableRef>(
        m_level_node
    ));
}

TilemapEditor::~TilemapEditor()
{
    m_app_state.disconnect_tilemap_editor(*this);
}

const ZPalette& TilemapEditor::z_palette() const
{
    return m_z_palette;
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
    draw_toolbar();

    m_current_mode->handle_input();
    render_preview();

    {
        m_app_state.sprite_renderer().begin(m_subwindow_2d);

        m_app_state.sprite_renderer().draw(
            m_spritesheet,
            glm::vec2(0, 0),
            m_spritesheet.texture().width(),
            m_spritesheet.texture().height());

        m_current_mode->draw_overlay(
            m_subwindow_2d,
            m_app_state.sprite_renderer());

        m_app_state.sprite_renderer().end();
    }

    if (ImGui::BeginChild("Map", ImVec2(500, 700))) {
        ImVec2 screen_pos = ImGui::GetCursorScreenPos();
        ImVec2 mouse_pos = ImGui::GetMousePos();
        m_mouse_rel_x = mouse_pos.x - screen_pos.x;
        m_mouse_rel_y = mouse_pos.y - screen_pos.y;
        ImGui::Image((ImTextureID)(intptr_t)m_subwindow_2d.texture().view(),
                     ImVec2(m_subwindow_2d.width(), m_subwindow_2d.height()));
    }
    ImGui::EndChild();

    ImGui::SameLine();

    m_z_palette.render();

    ImGui::SameLine();

    if (ImGui::BeginChild("Side Pane", ImVec2(200, 700))) {
        m_current_mode->draw_controls();

        draw_layer_list();
    }
    ImGui::EndChild();
}

void TilemapEditor::render_preview()
{
    m_app_state.renderer().render(m_subwindow_3d, m_scene, *m_current_camera);
}

void TilemapEditor::draw_toolbar()
{
    if (ImGui::Button("View 3D")) {
        m_current_mode = &m_view_3d_mode;
        m_current_camera = &m_persp_camera;
    }
    ImGui::SameLine();
    if (ImGui::Button("Edit Tiles")) {
        m_current_mode = &m_tile_mode;
        m_current_camera = &m_ortho_camera;
    }
    ImGui::SameLine();
    if (ImGui::Button("Edit Z")) {
        m_current_mode = &m_height_mode;
        m_current_camera = &m_ortho_camera;
    }
    ImGui::SameLine();
    if (ImGui::Button("Undo")) {
        m_app_state.undo();
    }
    ImGui::SameLine();
    if (ImGui::Button("Redo")) {
        m_app_state.redo();
    }
}

void TilemapEditor::draw_layer_list()
{
    auto& project = m_app_state.project();
    auto& level = project.level_at(m_selected_layer);

    if (ImGui::Button("-")) {
        m_app_state.push_command(
            std::make_unique<DeleteLayerCommand>(m_selected_layer));
        m_selected_layer.layer = std::max(-1, m_selected_layer.layer - 1);
    }
    ImGui::SameLine();
    if (ImGui::Button("+")) {
        m_app_state.push_command(
            std::make_unique<CreateLayerCommand>(m_selected_layer));
    }

    for (int i = 0; i < level.layer_count(); ++i) {
        draw_layer_item(i);
    }
}

void TilemapEditor::draw_layer_item(int i)
{
    auto& project = m_app_state.project();
    const Layer& layer = project.level_at(m_selected_layer).layer_at(i);

    const bool selected = i == m_selected_layer.layer;
    const ImVec4 bg_color =
        selected ? ImVec4(1, 1, 1, 1) : ImVec4(0.5, 0.5, 0.5, 1);
    const ImVec4 tint_color =
        selected ? ImVec4(0.8, 0.8, 0.8, 1) : ImVec4(1, 1, 1, 1);

    ImTextureID tex_id =
        (ImTextureID)(intptr_t)m_level_node.layer_at(i).thumbnail().view();

    if (ImGui::ImageButton(
            std::to_string(i).c_str(),
            tex_id,
            ImVec2(100, 100),
            ImVec2(0, 0),
            ImVec2(1, 1),
            bg_color,
            tint_color)) {

        m_selected_layer.layer = i;
    }
}
