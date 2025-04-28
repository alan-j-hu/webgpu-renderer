#ifndef TILESET_ADDNEWTILE_H
#define TILESET_ADDNEWTILE_H

#include "../modal.h"
#include <filesystem>
#include <optional>
#include <vector>

class Tileset;

class AddNewTile : public Modal
{
public:
    enum class Error
    {
        None,
        BadMesh
    };

    AddNewTile(Tileset&, ModalStack&);

    bool render() override;

private:
    Tileset& m_tileset;
    ModalStack& m_modals;
    std::vector<std::filesystem::path> m_sink;
    int m_width;
    int m_height;
    Error m_error;

    bool try_add_tile();
    void render_error();
};

#endif
