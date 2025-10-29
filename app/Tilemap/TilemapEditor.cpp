#include "TilemapEditor.h"
#include "LayerNode.h"
#include "../Commands/CreateLayerCommand.h"
#include "../Commands/DeleteLayerCommand.h"
#include "../Commands/PlaceTileCommand.h"
#include "noworry/grid.h"

#include "imgui.h"

TilemapEditor::LayerListener::LayerListener(TilemapEditor& editor)
    : m_editor(&editor)
{
}

void TilemapEditor::LayerListener::add_layer(Layer& layer, int index)
{
    m_editor->add_layer(layer, index);
}

void TilemapEditor::LayerListener::remove_layer(int index)
{
    m_editor->remove_layer(index);
}

TilemapEditor::TilemapEditor(AppState& app_state)
    : m_camera_selection(0),
      m_app_state(app_state),
      m_selected_layer {-1},
      m_subwindow_2d(app_state.renderer().device(), 500, 500),
      m_subwindow_3d(app_state.renderer().device(), 500, 500),
      m_spritesheet(app_state.renderer().device(),
                    app_state.sprite_renderer().pipeline(),
                    m_subwindow_3d.texture(),
                    app_state.renderer().default_sampler()),
      m_scene(app_state.renderer(), m_camera),
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
      m_layer_listener(*this),
      m_z_palette(app_state)
{
    app_state.connect_tilemap_editor(*this);

    //m_subwindow_2d.set_clear_color(app_state.background_color());
    m_subwindow_3d.set_clear_color(app_state.background_color());

    m_camera.set_position(glm::vec3(8.0f, -1.0f, 10.0f));
    m_camera.set_target(glm::vec3(8.0f, 8.0f, 0.0f));

    m_ortho_camera.set_position(glm::vec3(0.0f, 0.0f, 10.0f));
    m_ortho_camera.set_target(glm::vec3(0.0f, 0.0f, 0.0f));

    Transform transform;

    m_scene.children().push_back(std::make_unique<RenderObject>(
        app_state.renderer(),
        transform,
        m_grid_mesh,
        app_state.wireframe_material()
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
    m_app_state.renderer().render(m_subwindow_3d, m_scene);
}

void TilemapEditor::draw_toolbar()
{
    if (ImGui::Button("View 3D")) {
        m_scene.set_camera(m_camera);
        m_current_mode = &m_view_3d_mode;
    }
    ImGui::SameLine();
    if (ImGui::Button("Edit Tiles")) {
        m_scene.set_camera(m_ortho_camera);
        m_current_mode = &m_tile_mode;
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

    if (ImGui::Button("-")) {
        m_app_state.push_command(
            std::make_unique<DeleteLayerCommand>(m_selected_layer));
        m_selected_layer = std::max(-1, m_selected_layer - 1);
    }
    ImGui::SameLine();
    if (ImGui::Button("+")) {
        m_app_state.push_command(std::make_unique<CreateLayerCommand>());
    }

    for (int i = 0; i < project.layer_count(); ++i) {
        draw_layer_item(i);
    }
}

void TilemapEditor::draw_layer_item(int i)
{
    auto& project = m_app_state.project();
    const Layer& layer = project.layer_at(i);

    const bool selected = i == m_selected_layer;
    const ImVec4 bg_color =
        selected ? ImVec4(1, 1, 1, 1) : ImVec4(0.5, 0.5, 0.5, 1);
    const ImVec4 tint_color =
        selected ? ImVec4(0.8, 0.8, 0.8, 1) : ImVec4(1, 1, 1, 1);

    if (ImGui::ImageButton(
            std::to_string(i).c_str(),
            (ImTextureID)(intptr_t)m_layer_nodes[i]->thumbnail().view(),
            ImVec2(100, 100),
            ImVec2(0, 0),
            ImVec2(1, 1),
            bg_color,
            tint_color)) {

        m_selected_layer = i;
    }
}

void TilemapEditor::add_layer(Layer& layer, int index)
{
    auto ptr = std::make_unique<LayerNode>(m_app_state, layer);
    m_layer_nodes.insert(m_layer_nodes.begin() + index, ptr.get());
    m_scene.children().insert(
        m_scene.children().begin() + index,
        std::move(ptr));
}

void TilemapEditor::remove_layer(int index)
{
    m_scene.children().erase(m_scene.children().begin() + index);
    m_layer_nodes.erase(m_layer_nodes.begin() + index);
}
