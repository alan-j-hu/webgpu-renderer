#ifndef APPSTATE_H
#define APPSTATE_H

#include "Modal.h"
#include "Reducer.h"
#include "TileThumbnail.h"
#include "Commands/Command.h"
#include "Tileset/RotationDropdown.h"
#include "Tileset/TileMesh.h"
#include "Tileset/TileRotations.h"

#include "noworry/renderer.h"
#include "noworry/Draw2D/SpriteRenderer.h"
#include "noworry/rendertarget.h"
#include "noworry/resourcetable.h"
#include "noworry/camera/orthographiccamera.h"
#include "noworry/scene/scene.h"

#include <filesystem>
#include <map>
#include <string_view>
#include <vector>

#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <webgpu/webgpu.h>

/// The mesh and material of a tile definition.
struct ResolvedTile
{
    const TileMesh* mesh = nullptr;
    TextureMaterial* material = nullptr;
};

/// Helper class containing data used by multiple sub-editors.
class AppState
{
public:
    AppState(WGPUDevice);

    WGPUColor background_color() { return m_background_color; }

    Renderer& renderer() { return m_renderer; }
    SpriteRenderer& sprite_renderer() { return m_sprite_renderer; }
    ModalStack& modals() { return m_modals; }
    ResourceTable& resources() { return m_resources; }

    const std::map<std::string_view, TileRotations*>& mesh_map() const
    {
        return m_mesh_map;
    }

    Material& default_material() { return *m_default_material; }
    Material& wireframe_material() { return *m_wireframe_material; }
    Mesh& small_grid_mesh() { return m_small_grid_mesh; }

    /// Resolves a tile to its mesh and material.
    ResolvedTile resolve_tile(const TileDef& def);

    const Project& project() const { return m_project; }

    void push_command(std::unique_ptr<Command>);

    TileThumbnail& tile_thumbnail(int i)
    {
        return m_thumbnail_cache.at(i);
    }
    void refresh_thumbnails();

    void load_meshes(std::filesystem::path& path);

private:
    std::vector<std::unique_ptr<Command>> m_history;

    WGPUColor m_background_color;
    Renderer m_renderer;
    SpriteRenderer m_sprite_renderer;
    ModalStack m_modals;
    ResourceTable m_resources;

    std::map<std::string_view, TileRotations*> m_mesh_map;
    std::vector<std::unique_ptr<TileRotations>> m_meshes;

    Material* m_default_material;
    Material* m_wireframe_material;
    Mesh m_small_grid_mesh;

    Project m_project;

    Scene m_thumbnail_scene;
    OrthographicCamera m_thumbnail_camera;
    std::vector<TileThumbnail> m_thumbnail_cache;

    void visit_node(const aiScene* scene, const aiNode* node);
    TileRotations& load_mesh(const char* name, aiMesh* mesh);
};

#endif
