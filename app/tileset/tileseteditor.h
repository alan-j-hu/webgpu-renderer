#ifndef TILESET_TILESETEDITOR_H
#define TILESET_TILESETEDITOR_H

#include "tiledefinition.h"
#include "tilemesh.h"
#include "tileset.h"
#include "../modal.h"
#include "noworry/mesh.h"
#include "noworry/renderer.h"
#include "noworry/rendertarget.h"
#include "noworry/resourcetable.h"
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

    Renderer& renderer() { return m_renderer; }

    ResourceTable& resources() { return m_resources; }

    Material& default_material() { return *m_default_material; }

    Tileset& tileset() { return m_tileset; }

    const std::map<std::string, TileDefinition*>& mesh_map() const
    { return m_mesh_map; }

    void render();

private:
    ModalStack& m_modals;
    Renderer& m_renderer;
    Tileset m_tileset;
    std::map<std::string, TileDefinition*> m_mesh_map;
    std::vector<std::unique_ptr<TileDefinition>> m_tiles;
    std::vector<std::unique_ptr<TileMesh>> m_meshes;

    TileDefinition* m_selected_tile = nullptr;

    RenderTarget m_tile_preview;
    Scene m_scene;
    ResourceTable m_resources;
    Material* m_default_material;

    std::vector<std::filesystem::path> m_sink;

    bool render_preview();

    void load_meshes(std::filesystem::path& path);
    void visit_node(aiNode* node);
    void load_mesh(aiMesh* mesh);
};

#endif
