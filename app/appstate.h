#ifndef APPSTATE_H
#define APPSTATE_H

#include "modal.h"
#include "tileset/rotationdropdown.h"
#include "tileset/tilemesh.h"
#include "tileset/tilerotations.h"
#include "tileset/tileset.h"

#include "noworry/renderer.h"
#include "noworry/resourcetable.h"

#include <filesystem>
#include <map>
#include <string_view>
#include <vector>

#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <immer/vector.hpp>
#include <webgpu/webgpu.h>

struct TextureRef
{
    std::filesystem::path path;
    std::shared_ptr<TextureMaterial> material;
};

struct TileDef
{
    std::optional<std::shared_ptr<std::string>> mesh;
    RotationTag rotation;
    std::optional<TextureRef> texture;
};

struct TileInst
{
    int def;
    short z;
};

struct Layer
{
    immer::vector<std::optional<TileInst>> tiles;

    Layer()
    {
        for (int i = 0; i < 16 * 16; ++i) {
            tiles = tiles.push_back(std::nullopt);
        }
    }

    const std::optional<TileInst>& at(int x, int y) const
    {
        return tiles.at(y * 16 + x);
    }

    Layer set(int x, int y, std::optional<TileInst> option) const
    {
        Layer new_layer = *this;
        new_layer.tiles = tiles.set(y * 16 + x, option);
        return new_layer;
    }
};

struct Project
{
    immer::vector<TileDef> tile_defs;
    immer::vector<Layer> layers;
};

/// Helper class containing data used by multiple sub-editors.
class AppState
{
public:
    AppState(WGPUDevice);

    WGPUColor background_color() { return m_background_color; }

    Renderer& renderer() { return m_renderer; }
    ModalStack& modals() { return m_modals; }
    ResourceTable& resources() { return m_resources; }

    const std::map<std::string_view, TileRotations*>& mesh_map() const
    {
        return m_mesh_map;
    }

    Material& default_material() { return *m_default_material; }
    Material& wireframe_material() { return *m_wireframe_material; }
    Mesh& small_grid_mesh() { return m_small_grid_mesh; }

    Tileset& tileset() { return m_tileset; }
    const Project& project() const { return m_project; }
    void set_project(Project project) { m_project = project; }

    void load_meshes(std::filesystem::path& path);

private:
    WGPUColor m_background_color;
    Renderer m_renderer;
    ModalStack m_modals;
    ResourceTable m_resources;

    std::map<std::string_view, TileRotations*> m_mesh_map;
    std::vector<std::unique_ptr<TileRotations>> m_meshes;

    Material* m_default_material;
    Material* m_wireframe_material;
    Mesh m_small_grid_mesh;

    Tileset m_tileset;
    Project m_project;

    void visit_node(const aiScene* scene, const aiNode* node);
    TileRotations& load_mesh(const char* name, aiMesh* mesh);
};

#endif
