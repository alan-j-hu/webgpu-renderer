#ifndef TILESET_TILESETEDITOR_H
#define TILESET_TILESETEDITOR_H

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
#include <string_view>
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

    Material& wireframe_material() { return *m_wireframe_material; }

    Tileset& tileset() { return m_tileset; }

    Mesh& grid_mesh() { return m_grid_mesh; }

    const std::map<std::string_view, TileMesh*>& mesh_map() const
    { return m_mesh_map; }

    void render();

private:
    ModalStack& m_modals;
    Renderer& m_renderer;
    Tileset m_tileset;
    std::map<std::string_view, TileMesh*> m_mesh_map;
    std::vector<std::unique_ptr<TileMesh>> m_meshes;

    TileMesh* m_selected_tile = nullptr;

    RenderTarget m_tile_preview;
    Mesh m_grid_mesh;
    Scene m_scene;
    ResourceTable m_resources;
    Material* m_default_material;
    Material* m_wireframe_material;

    std::vector<std::filesystem::path> m_sink;

    bool render_preview();

    void load_meshes(std::filesystem::path& path);
    void visit_node(const aiScene* scene, const aiNode* node);
    TileMesh& load_mesh(const char* name, aiMesh* mesh);
};

#endif
