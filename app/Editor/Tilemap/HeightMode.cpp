#include "HeightMode.h"
#include "../Editor.h"

HeightMode::HeightMode(AppState& app_state, Editor& editor)
    : View2DMode(app_state, editor)
{
}

void HeightMode::draw_overlay(RenderTarget&, SpriteRenderer& sprite_renderer)
{
    const Project& project = app_state().project();
    const Layer* layer = app_state().selected_layer();
    if (layer == nullptr) {
        return;
    }

    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            auto& opt = layer->at(x, y);
            if (!opt) {
                continue;
            }

            auto& inst = opt.value();
            if (!inst.def()->model_data()) {
                continue;
            }
            short z = opt.value().z();

            Region dest = region(x, y, 0.5, 0.5);
            const Spritesheet* spritesheet =
                editor().z_palette().spritesheet(z);

            sprite_renderer.draw(
                *spritesheet,
                glm::vec2(dest.x, dest.y),
                dest.w,
                dest.h,
                { 0.0, 0.0, 1.0, 1.0 },
                0,
                glm::vec2(0, dest.h));
        }
    }
}

void HeightMode::draw_controls()
{
}

void HeightMode::handle_left_mouse_down(int x, int y)
{
    auto& project = app_state().project();
    Layer* layer = app_state().selected_layer();
    if (layer == nullptr) {
        return;
    }

    std::optional<TileInst> inst_opt = layer->at(x, y);

    if (inst_opt) {
        short z = editor().z_palette().selected_z();

        if (m_command.get() == nullptr) {
            auto command = std::make_unique<PlaceTilesCommand>(
                *layer, z, inst_opt->rotation(), inst_opt->def());
            app_state().push_command(std::move(command), &m_command);
            m_command.get()->add_placement(x, y);
        } else {
            m_command.get()->add_placement(x, y);
            m_command.set_needs_update(true);
        }
    }
}

void HeightMode::handle_left_mouse_released(int x, int y)
{
    if (m_command.get() != nullptr) {
        app_state().finish_long_command();
    }
}
