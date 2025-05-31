#ifndef TILESET_MODELINSPECTOR_H
#define TILESET_MODELINSPECTOR_H

#include "tilemesh.h"
#include "tilerotations.h"
#include "tileset.h"
#include "../modal.h"
#include "../pane.h"
#include "noworry/mesh.h"
#include "noworry/renderer.h"
#include "noworry/rendertarget.h"
#include "noworry/resourcetable.h"
#include "noworry/camera/perspectivecamera.h"
#include "noworry/scene/scene.h"

#include <filesystem>
#include <map>
#include <string_view>
#include <vector>

#include <assimp/mesh.h>
#include <assimp/scene.h>

class ModelInspector : public Pane
{
public:
    ModelInspector(std::string name, int flex, ModalStack&, Renderer&);

    Renderer& renderer() { return m_renderer; }

    ResourceTable& resources() { return m_resources; }

    Material& default_material() { return *m_default_material; }

    Material& wireframe_material() { return *m_wireframe_material; }

    Tileset& tileset() { return m_tileset; }

    Mesh& grid_mesh() { return m_grid_mesh; }

    const RenderObject& grid() const { return *m_grid; }

    const std::map<std::string_view, TileRotations*>& mesh_map() const
    { return m_mesh_map; }

protected:
    virtual void content() override;

private:
    ModalStack& m_modals;
    Renderer& m_renderer;
    Tileset m_tileset;
    std::map<std::string_view, TileRotations*> m_mesh_map;
    std::vector<std::unique_ptr<TileRotations>> m_meshes;

    TileRotations* m_selected_tile = nullptr;
    int m_rotation;

    RenderTarget m_tile_preview;
    Mesh m_grid_mesh;
    std::unique_ptr<RenderObject> m_grid;

    Scene m_scene;
    PerspectiveCamera m_camera;
    ResourceTable m_resources;
    Material* m_default_material;
    Material* m_wireframe_material;

    std::vector<std::filesystem::path> m_sink;

    bool render_preview();

    void load_meshes(std::filesystem::path& path);
    void visit_node(const aiScene* scene, const aiNode* node);
    TileRotations& load_mesh(const char* name, aiMesh* mesh);
};

#endif
