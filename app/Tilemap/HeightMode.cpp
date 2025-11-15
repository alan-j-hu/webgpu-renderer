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

            glm::vec3 pos_3d(x, y, 0);
            // Top-left in 2D space
            glm::vec2 pos_tl =
                editor().map_3d_to_2d(pos_3d + glm::vec3(0, 1, 0));
            // Bottom-right in 2D space
            glm::vec2 pos_br =
                editor().map_3d_to_2d(pos_3d + glm::vec3(1, 0, 0));

            Region src;
            src.x = 0;
            src.y = 0;
            src.w = 1;
            src.h = 1;

            Region dest;
            dest.x = pos_tl.x;
            dest.y = pos_tl.y;
            dest.w = pos_br.x - pos_tl.x;
            dest.h = pos_br.y - pos_tl.y;

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
