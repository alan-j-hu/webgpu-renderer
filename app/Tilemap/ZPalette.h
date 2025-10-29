#ifndef ZPALETTE_H
#define ZPALETTE_H

#include "noworry/texture.h"
#include <array>
#include <filesystem>
#include <memory>

class AppState;

class ZPalette
{
public:
    ZPalette(AppState&);

    void render();

private:
    AppState* m_app_state;
    short m_selected;

    std::array<std::shared_ptr<Texture>, 32> m_textures;

    void load(int index, const std::filesystem::path& path);

    bool render_button(int index, bool selected);
};

#endif
