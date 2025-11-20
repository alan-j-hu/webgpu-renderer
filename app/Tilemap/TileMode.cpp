#include "TileMode.h"
#include "TilemapEditor.h"
#include "../Commands/PlaceTileCommand.h"

TileMode::TileMode(AppState& app_state, TilemapEditor& editor)
    : View2DMode(app_state, editor),
      m_selected_tile(-1),
      m_tile_picker(app_state)
{
}

void TileMode::handle_click(int x, int y)
{
    auto& project = app_state().project();
    int selected_layer = editor().selected_layer();

    if (selected_layer != -1 && m_selected_tile != -1) {
        short z = editor().z_palette().selected_z();

        app_state().push_command(std::make_unique<PlaceTileCommand>(
            selected_layer, x, y, z,
            project.tiledef_at(m_selected_tile)));
    }
}

void TileMode::draw_overlay(
    RenderTarget& target,
    SpriteRenderer& sprite_renderer)
{
    const Project& project = app_state().project();
    TileThumbnail* thumb = nullptr;
    TileDef* tiledef = nullptr;
    if (m_selected_tile != -1) {
        thumb = &app_state().tile_thumbnail(m_selected_tile);
        tiledef = project.tiledef_at(m_selected_tile).get();
    }

    std::optional<std::pair<int, int>> cell_opt = editor().mouseover_cell();
    if (cell_opt && thumb != nullptr) {
        Region src;
        src.x = 0;
        src.y = 1 - tiledef->depth() / 5.0;
        src.w = tiledef->width() / 5.0;
        src.h = tiledef->depth() / 5.0;

        Region dest = region(
            cell_opt->first,
            cell_opt->second,
            tiledef->width(),
            tiledef->depth());

        sprite_renderer.draw(thumb->spritesheet(), dest, src);
    }
}

void TileMode::draw_controls()
{
    m_tile_picker.render(m_selected_tile);
}
