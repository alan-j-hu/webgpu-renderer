#ifndef APPSTATE_H
#define APPSTATE_H

#include "modal.h"
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
#include <webgpu/webgpu.h>

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

    void visit_node(const aiScene* scene, const aiNode* node);
    TileRotations& load_mesh(const char* name, aiMesh* mesh);
};

#endif
