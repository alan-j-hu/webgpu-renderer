#ifndef TILESET_ADDNEWTILE_H
#define TILESET_ADDNEWTILE_H

#include "../modal.h"
#include <filesystem>
#include <optional>
#include <vector>

class TilesetEditor;

class AddNewTile : public Modal
{
public:
    enum class Error
    {
        None,
        BadMesh
    };

    AddNewTile(TilesetEditor&, ModalStack&);

    bool render() override;

private:
    TilesetEditor& m_tileset_editor;
    ModalStack& m_modals;
    std::vector<std::filesystem::path> m_sink;
    int m_width;
    int m_height;
    Error m_error;

    bool try_add_tile();
    void render_error();
};

#endif
