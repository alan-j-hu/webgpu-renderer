#include "Editor.h"
#include "Tilemap/LayerNode.h"
#include "../Commands/CreateLayerCommand.h"
#include "../Commands/CreateLevelCommand.h"
#include "../Commands/DeleteLayerCommand.h"
#include "../Commands/ResizeWorldCommand.h"
#include "noworry/grid.h"

#include <fstream>
#include "imgui.h"

Editor::Editor(AppState& app_state)
    : m_camera_selection(0),
      m_app_state(&app_state),
      m_main_file_dialog("Choose File", std::filesystem::current_path()),
      m_export_dialog(app_state),
      m_tile_list(app_state, *this),
      m_subwindow_2d(app_state.renderer().device(), 500, 500),
      m_subwindow_3d(app_state.renderer().device(), 500, 500),
      m_spritesheet(app_state.renderer().device(),
                    app_state.sprite_renderer().pipeline(),
                    m_subwindow_3d.texture(),
                    app_state.renderer().default_sampler()),
      m_viewer_mouse_down(false),
      m_viewer_mouse_over(false),
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
      m_z_palette(app_state)
{
    m_app_state->project().world_at(0).listenable().add_listener(*this);

    setup_scene();
}

void Editor::level_added(Level& level, int x, int y)
{
    m_level_nodes.emplace(
        &level,
        std::make_unique<LevelNode>(*m_app_state, level));
}

void Editor::level_removed(Level& level, int x, int y)
{
    m_level_nodes.erase(&level);
}

const ZPalette& Editor::z_palette() const
{
    return m_z_palette;
}

glm::vec3 Editor::map_2d_to_3d(const glm::vec2& vec2d) const
{
    glm::vec2 v = vec2d;
    v.x /= m_subwindow_2d.width() / 2;
    v.y /= m_subwindow_2d.height() / 2;
    v.x -= 1;
    v.y -= 1;
    v.y *= -1;
    return m_ortho_camera.unproject(glm::vec3(v, 0));
}

glm::vec2 Editor::map_3d_to_2d(const glm::vec3& vec3d) const
{
    glm::vec2 v = m_ortho_camera.project(vec3d);
    v.y *= -1;
    v.x += 1;
    v.y += 1;
    v.x *= m_subwindow_2d.width() / 2;
    v.y *= m_subwindow_2d.height() / 2;
    return v;
}

std::optional<std::pair<int, int>> Editor::mouseover_cell()
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

glm::vec2 Editor::mouse_pos() const
{
    return glm::vec2(m_mouse_rel_x, m_mouse_rel_y);
}

void Editor::open_error_modal(const std::string& message)
{
    m_error_modal.open(message.c_str());
}

void Editor::draw()
{
    draw_menubar();
    draw_main_pane();
    m_error_modal.update("Error");
}

void Editor::draw_menubar()
{
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open")) {
                m_io_state = Editor::IOState::OPENING;
                m_main_file_dialog.open();
            }
            if (ImGui::MenuItem("Save As")) {
                m_io_state = Editor::IOState::SAVING;
                m_main_file_dialog.open(FileDialog::WRITE);
            }

            if (ImGui::MenuItem("Export")) {
                m_io_state = Editor::IOState::EXPORTING;
                m_export_dialog.open();
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            ImGui::EndMenu();
        }

        if (auto path = m_main_file_dialog.update()) {
            if (m_io_state == IOState::OPENING) {
                load_project(*path);
            } else if (m_io_state == IOState::SAVING) {
                save_project(*path);
            }
            m_io_state = IOState::NONE;
        }

        if (m_io_state == IOState::EXPORTING) {
            if (m_export_dialog.update() == ExportDialog::State::CLOSED) {
                m_io_state = IOState::NONE;
            }
        }

        ImGui::EndMenuBar();
    }
}

void Editor::draw_main_pane()
{
    if (ImGui::BeginTabBar("Tabs")) {
        if (ImGui::BeginTabItem("Tilemap Editor")) {
            draw_tilemap_editor();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Tileset Editor")) {
            draw_tileset_editor();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("World Editor")) {
            draw_world_editor();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void Editor::draw_world_editor()
{
    const int THUMBNAIL_SIZE = 64;
    const int EXPAND_BY = 2;

    auto& world = m_app_state->project().world_at(0);

    if (ImGui::Button("Expand World")) {
        auto* level = m_app_state->selected_level();
        m_app_state->push_command(std::make_unique<ResizeWorldCommand>(
            world,
            EXPAND_BY,
            EXPAND_BY,
            EXPAND_BY,
            EXPAND_BY));
        if (level != nullptr) {
            m_app_state->select_level(
                glm::ivec2(level->x() + EXPAND_BY, level->y() + EXPAND_BY));
        }
    }

    const int width = world.grid_width();
    const int depth = world.grid_depth();
    ImGuiTableFlags flags =
        ImGuiTableFlags_BordersOuterH
      | ImGuiTableFlags_BordersOuterV
      | ImGuiTableFlags_SizingFixedFit;

    if (ImGui::BeginTable("World Grid", width, flags)) {
        for (int x = 0; x < width; ++x) {
            ImGui::PushID(x);
            ImGui::TableSetupColumn(
                "#Col", ImGuiTableColumnFlags_None, THUMBNAIL_SIZE);
            ImGui::PopID();
        }

        for (int y = 0; y < depth; ++y) {
          ImGui::PushID(y);
          ImGui::TableNextRow(ImGuiTableRowFlags_None, THUMBNAIL_SIZE);
          for (int x = 0; x < width; ++x) {
              ImGui::PushID(x);
              ImGui::TableNextColumn();
              Level* level = world.level_at(x, y);
              if (level == nullptr) {
                  if (ImGui::Button("+")) {
                      m_app_state->push_command(
                          std::make_unique<CreateLevelCommand>(world, x, y));
                  }
              } else {
                  auto& level_node = m_level_nodes.at(level);
                  auto& layer_node = level_node->layer_at(0);
                  auto tex_id =
                      (ImTextureID)(intptr_t)layer_node.thumbnail().view();

                  if (ImGui::ImageButton(
                          "#Button",
                          tex_id,
                          ImVec2(THUMBNAIL_SIZE, THUMBNAIL_SIZE),
                          ImVec2(0, 0),
                          ImVec2(1, 1))) {

                  }
              }

              ImGui::PopID();
          }

          ImGui::PopID();
        }

        ImGui::EndTable();
    }
}

void Editor::draw_tileset_editor()
{
    m_tile_list.draw();
}

void Editor::draw_tilemap_editor()
{
    draw_toolbar();

    m_current_mode->handle_input();
    render_preview();

    {
        m_app_state->sprite_renderer().begin(m_subwindow_2d);

        m_app_state->sprite_renderer().draw(
            m_spritesheet,
            glm::vec2(0, 0),
            m_spritesheet.texture().width(),
            m_spritesheet.texture().height());

        m_current_mode->draw_overlay(
            m_subwindow_2d,
            m_app_state->sprite_renderer());

        m_app_state->sprite_renderer().end();
    }

    if (ImGui::BeginChild("Map", ImVec2(500, 700))) {
        ImVec2 screen_pos = ImGui::GetCursorScreenPos();
        ImVec2 mouse_pos = ImGui::GetMousePos();
        m_mouse_rel_x = mouse_pos.x - screen_pos.x;
        m_mouse_rel_y = mouse_pos.y - screen_pos.y;

        auto size = ImVec2(m_subwindow_2d.width(), m_subwindow_2d.height());
        auto pos = ImGui::GetCursorScreenPos();

        // If the mouse happens to click down while interacting with
        // overlapping UI widgets, the mouse event should not be sent to the
        // canvas. See the solution at https://github.com/ocornut/imgui/issues/4234#issuecomment-863344948.
        ImGui::InvisibleButton(
            "canvas", size,
            ImGuiButtonFlags_MouseButtonLeft
            | ImGuiButtonFlags_MouseButtonRight);
        m_viewer_mouse_over = ImGui::IsItemHovered();
        m_viewer_mouse_down = ImGui::IsItemActive();
        ImGui::SetCursorScreenPos(pos);
        ImGui::Image((ImTextureID)(intptr_t)m_subwindow_2d.texture().view(),
                     size);
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

void Editor::render_preview()
{
    m_app_state->renderer().render(m_subwindow_3d, m_scene, *m_current_camera);
}

void Editor::draw_toolbar()
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
        m_app_state->undo();
    }
    ImGui::SameLine();
    if (ImGui::Button("Redo")) {
        m_app_state->redo();
    }
}

void Editor::draw_layer_list()
{
    auto& project = m_app_state->project();
    auto* level = m_app_state->selected_level();
    if (level == nullptr) {
        return;
    }

    if (ImGui::Button("-")) {
        if (auto* layer = m_app_state->selected_layer()) {
            m_app_state->push_command(
                std::make_unique<DeleteLayerCommand>(
                    *level,
                    *m_app_state->selected_layer_idx()));
            m_app_state->select_layer(std::nullopt);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("+")) {
        m_app_state->push_command(
            std::make_unique<CreateLayerCommand>(*level));
    }

    for (int i = 0; i < level->layer_count(); ++i) {
        draw_layer_item(i);
    }
}

void Editor::draw_layer_item(int i)
{
    auto& project = m_app_state->project();
    const Layer& layer = m_app_state->selected_level()->layer_at(i);

    const bool selected = m_app_state->selected_layer() == &layer;
    const ImVec4 bg_color =
        selected ? ImVec4(1, 1, 1, 1) : ImVec4(0.5, 0.5, 0.5, 1);
    const ImVec4 tint_color =
        selected ? ImVec4(0.8, 0.8, 0.8, 1) : ImVec4(1, 1, 1, 1);

    auto& level_node = *m_level_nodes.at(m_app_state->selected_level());
    ImTextureID tex_id =
        (ImTextureID)(intptr_t)level_node.layer_at(i).thumbnail().view();

    if (ImGui::ImageButton(
            std::to_string(i).c_str(),
            tex_id,
            ImVec2(100, 100),
            ImVec2(0, 0),
            ImVec2(1, 1),
            bg_color,
            tint_color)) {

        m_app_state->select_layer(i);
    }
}

void Editor::load_project(const std::filesystem::path& path)
{
    try {
        std::ifstream f(path);
        nlohmann::json json = nlohmann::json::parse(f);
        Project project = m_app_state->deserializer().load_project(
            path.parent_path(),
            json);

        destroy_scene();

        m_app_state->set_project(std::move(project));

        setup_scene();
    } catch (std::exception& ex) {
        m_error_modal.open(ex.what());
    }
}

void Editor::save_project(const std::filesystem::path& path)
{
    try {
        std::ofstream f(path);
        nlohmann::json json = m_app_state->serializer().save_project(
            m_app_state->project()
        );
        f << json << std::flush;
    } catch (std::exception& ex) {
        m_error_modal.open(ex.what());
    }
}

void Editor::destroy_scene()
{
    m_scene.children().clear();
    m_tileset_thumbnails.clear();
    m_level_nodes.clear();
}

void Editor::setup_scene()
{
    auto& project = m_app_state->project();

    for (int i = 0; i < project.tileset_count(); ++i) {
        m_tileset_thumbnails.push_back(std::make_unique<TilesetThumbnails>(
            *m_app_state,
            *project.tileset_at(i)));
    }

    auto& world = project.world_at(0);
    for (auto it = world.begin(); it != world.end(); ++it) {
        m_level_nodes.emplace(
            it->second.get(),
            std::make_unique<LevelNode>(*m_app_state, *it->second));
    }

    m_subwindow_3d.set_clear_color(m_app_state->background_color());

    m_persp_camera.set_position(glm::vec3(8.0f, -1.0f, 10.0f));
    m_persp_camera.set_target(glm::vec3(8.0f, 8.0f, 0.0f));

    m_ortho_camera.set_position(glm::vec3(0.0f, 0.0f, 10.0f));
    m_ortho_camera.set_target(glm::vec3(0.0f, 0.0f, 0.0f));

    m_current_camera = &m_persp_camera;

    Transform transform;

    m_scene.children().clear();
    m_scene.children().push_back(std::make_unique<RenderObject>(
        m_app_state->renderer(),
        transform,
        m_grid_mesh,
        m_app_state->wireframe_material()
    ));
    m_scene.children().push_back(std::make_unique<RenderableRef>(
        *m_level_nodes.at(world.level_at(0, 0))
    ));
}
