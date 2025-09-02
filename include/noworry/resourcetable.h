#ifndef RESOURCE_TABLE_H
#define RESOURCE_TABLE_H

#include "texture.h"
#include "renderer.h"
#include "Gfx3D/BasicMesh.h"
#include "Gfx3D/Model.h"
#include "Material/Material.h"
#include "Material/FlatMaterial.h"
#include "Material/TextureMaterial.h"
#include "Resource/ResourceCache.h"

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>

#include <assimp/scene.h>
#include <glm/mat4x4.hpp>

template<class T>
class ResourceLoader;

/// A cache for loading and storing textures and materials. If a file has
/// already been loaded and is in use, it will not be loaded again.
///
/// This class keeps weak_ptrs to its resources, so it will unload resources
/// which are no longer in use.
class ResourceTable
{
public:
    ResourceTable(Renderer&);
    ResourceTable(const ResourceTable&) = delete;
    ResourceTable& operator=(const ResourceTable&) = delete;
    ResourceTable(ResourceTable&&) = default;
    ResourceTable& operator=(ResourceTable&&) = default;

    Renderer& renderer() { return *m_renderer; }

    std::shared_ptr<Material> default_material()
    {
        return m_default_material;
    }

    template<class T>
    ResourceCache<T>& cache();

    template<class T>
    std::optional<std::shared_ptr<T>> load(const std::filesystem::path& path)
    {
        auto opt = cache<T>().find(path);
        if (opt) {
            return opt;
        }

        ResourceLoader<T> loader;
        auto t = loader.load(*this, path);
        if (t) {
            cache<T>().store(path, *t);
        }
        return t;
    }

    FlatMaterial& add_flat_material(float, float, float);

    FlatMaterial& add_wireframe_material(float, float, float);

    TextureMaterial& add_texture_material(std::shared_ptr<Texture>);

private:
    WGPUDevice m_device;
    Renderer* m_renderer;

    int m_mat_id;

    std::shared_ptr<Material> m_default_material;

    ResourceCache<Texture> m_textures;
    ResourceCache<TextureMaterial> m_texture_materials;
    ResourceCache<Model> m_models;

    std::vector<std::unique_ptr<Material>> m_materials;
    std::vector<std::unique_ptr<BasicMesh>> m_meshes;
};

template<>
inline ResourceCache<Texture>& ResourceTable::cache<Texture>()
{
    return m_textures;
}

template<>
inline ResourceCache<TextureMaterial>& ResourceTable::cache<TextureMaterial>()
{
    return m_texture_materials;
}

template<>
inline ResourceCache<Model>& ResourceTable::cache<Model>()
{
    return m_models;
}

#endif
