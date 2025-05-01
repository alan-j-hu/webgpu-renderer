#ifndef TILESET_TILESETEDITOR_H
#define TILESET_TILESETEDITOR_H

#include "tileset.h"
#include "../modal.h"
#include "noworry/renderer.h"

#include <filesystem>
#include <string>
#include <vector>

#include <assimp/mesh.h>
#include <assimp/scene.h>

class TilesetEditor
{
public:
    TilesetEditor(ModalStack&, Renderer&);

    void render();

private:
    ModalStack& m_modals;
    Renderer& m_renderer;
    Tileset m_tileset;
    std::vector<std::string> m_names;

    std::vector<std::filesystem::path> m_sink;

    void load_meshes(std::filesystem::path& path);
    void visit_node(aiNode* node);
    void load_mesh(aiMesh* mesh);
};

#endif
