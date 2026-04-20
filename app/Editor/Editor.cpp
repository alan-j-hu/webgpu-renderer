#include "Editor.h"
#include "Tilemap/LayerNode.h"
#include "../Commands/CreateLayerCommand.h"
#include "../Commands/CreateLevelCommand.h"
#include "../Commands/DeleteLayerCommand.h"
#include "../Commands/DeleteLevelCommand.h"
#include "../Commands/ResizeWorldCommand.h"
#include "noworry/grid.h"

#include <fstream>
#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"

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
      m_render_ctx(app_state.renderer()),
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
    m_layer_list(app_state, *this)
{
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

void Editor::render(RenderContext& ctx)
{
    ctx.add(m_grid_mesh, m_app_state->wireframe_material(), glm::mat4(1));
    m_level_nodes.at(m_app_state->selected_level())->render(ctx);
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

LevelNode* Editor::level_node(Level& level)
{
    auto it = m_level_nodes.find(m_app_state->selected_level());
    if (it == m_level_nodes.end()) {
        return nullptr;
    }
    return it->second.get();
}

void Editor::draw()
{
    ImGuiID dockspace_id = ImGui::GetID("My Dockspace");
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    if (ImGui::DockBuilderGetNode(dockspace_id) == nullptr) {
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        ImGuiID left_panel_id = 0;
        ImGuiID map_editor_id = 0;
        ImGui::DockBuilderSplitNode(
            dockspace_id,
            ImGuiDir_Left,
            0.2,
            &left_panel_id,
            &map_editor_id);

        ImGuiID tileset_editor_id = 0;
        ImGuiID tile_editor_id = 0;
        ImGui::DockBuilderSplitNode(
            left_panel_id,
            ImGuiDir_Up,
            0.5,
            &tileset_editor_id,
            &tile_editor_id);

        ImGuiID map_screen_id = 0;
        ImGuiID map_right_panel_id = 0;
        ImGui::DockBuilderSplitNode(
            map_editor_id,
            ImGuiDir_Left,
            0.75,
            &map_screen_id,
            &map_right_panel_id);

        ImGuiID world_map_id = 0;
        ImGuiID layer_list_id = 0;
        ImGui::DockBuilderSplitNode(
            map_right_panel_id,
            ImGuiDir_Up,
            0.25,
            &world_map_id,
            &layer_list_id);

        ImGui::DockBuilderDockWindow("Tile", tile_editor_id);
        ImGui::DockBuilderDockWindow("Tiles", tileset_editor_id);
        ImGui::DockBuilderDockWindow("Map", map_screen_id);
        ImGui::DockBuilderDockWindow("World", world_map_id);
        ImGui::DockBuilderDockWindow("Layers", layer_list_id);

        ImGui::DockBuilderFinish(dockspace_id);
    }
    ImGui::DockSpaceOverViewport(dockspace_id);

    draw_menubar();

    ImGui::Begin("Tiles");
    m_tile_list.draw();
    ImGui::End();

    ImGui::Begin("Tile");
    m_tile_list.draw_tile_editor();
    ImGui::End();

    ImGui::Begin("Map");
    draw_tilemap_editor();
    ImGui::End();

    ImGui::Begin("World");
    draw_world_editor();
    ImGui::End();

    ImGui::Begin("Layers");
    m_layer_list.draw();
    ImGui::End();

    if (auto error = m_app_state->check_error()) {
        m_error_modal.open(error->c_str());
    }

    m_error_modal.update("Error");
}

void Editor::draw_menubar()
{
    if (ImGui::BeginMainMenuBar()) {
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

        ImGui::EndMainMenuBar();
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
                level->x() + EXPAND_BY,
                level->y() + EXPAND_BY);
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

                      m_app_state->select_level(x, y);

                  } else if (ImGui::BeginPopupContextItem()) {
                      if (ImGui::MenuItem("Delete")) {
                          m_app_state->push_command(
                              std::make_unique<DeleteLevelCommand>(
                                  world, *level));
                      }
                      ImGui::EndPopup();
                  }
              }

              ImGui::PopID();
          }

          ImGui::PopID();
        }

        ImGui::EndTable();
    }
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

    {
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

    ImGui::SameLine();

    m_z_palette.render();
}

void Editor::render_preview()
{
    m_app_state->renderer().render(
        m_render_ctx,
        *this,
        m_subwindow_3d,
        *m_current_camera);
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
    m_tileset_thumbnails.clear();
    m_level_nodes.clear();
}

void Editor::setup_scene()
{
    auto& project = m_app_state->project();
    project.world_at(0).listenable().add_listener(*this);

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
}
