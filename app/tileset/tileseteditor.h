#ifndef TILESET_TILESETEDITOR_H
#define TILESET_TILESETEDITOR_H

#include "tileset.h"
#include "../modal.h"
#include "noworry/mesh.h"
#include "noworry/renderer.h"
#include "noworry/rendertarget.h"
#include "noworry/scene/scene.h"

#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include <assimp/mesh.h>
#include <assimp/scene.h>

class TilesetEditor
{
public:
    TilesetEditor(ModalStack&, Renderer&);

    Tileset& tileset() { return m_tileset; }
    const std::map<std::string, Mesh*>& mesh_map() const { return m_mesh_map; }

    void render();

private:
    ModalStack& m_modals;
    Renderer& m_renderer;
    Tileset m_tileset;
    std::vector<std::string> m_names;
    std::map<std::string, Mesh*> m_mesh_map;
    std::vector<std::unique_ptr<Mesh>> m_meshes;

    RenderTarget m_tile_preview;
    Scene m_scene;

    std::vector<std::filesystem::path> m_sink;

    bool render_preview();

    void load_meshes(std::filesystem::path& path);
    void visit_node(aiNode* node);
    void load_mesh(aiMesh* mesh);
};

#endif
