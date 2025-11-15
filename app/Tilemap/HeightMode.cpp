#include "HeightMode.h"
#include "TilemapEditor.h"
#include "../Commands/PlaceTileCommand.h"

HeightMode::HeightMode(AppState& app_state, TilemapEditor& editor)
    : View2DMode(app_state, editor)
{
}

void HeightMode::draw_overlay(RenderTarget&, SpriteRenderer& sprite_renderer)
{
    int layer_idx = editor().selected_layer();

    if (layer_idx == -1) {
        return;
    }

    const Project& project = app_state().project();
    const Layer& layer = project.layer_at(layer_idx);

    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            auto& opt = layer.at(x, y);
            if (!opt) {
                continue;
            }

            auto& inst = opt.value();
            if (!inst.def()->model_data) {
                continue;
            }
            short z = opt.value().z();

            Region src;
            src.x = 0;
            src.y = 0;
            src.w = 1;
            src.h = 1;

            Region dest = region(x, y, 0.5, 0.5);

            const Spritesheet* spritesheet =
                editor().z_palette().spritesheet(z);

            sprite_renderer.draw(*spritesheet, dest, src);
        }
    }
}

void HeightMode::draw_controls()
{
}

void HeightMode::handle_click(int tile_x, int tile_y)
{
    auto& project = app_state().project();
    int selected_layer = editor().selected_layer();

    std::optional<TileInst> inst_opt = project
        .layer_at(selected_layer)
        .at(tile_x, tile_y);

    if (inst_opt) {
        short z = editor().z_palette().selected_z();

        app_state().push_command(std::make_unique<PlaceTileCommand>(
            selected_layer, tile_x, tile_y, z,
            inst_opt->def()));
    }
}
