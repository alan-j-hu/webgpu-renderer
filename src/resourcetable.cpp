#include "noworry/resourcetable.h"
#include "noworry/Gfx3D/Model.h"
#include "noworry/Material/Effect.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

ResourceTable::ResourceTable(Renderer& renderer)
    : m_renderer(&renderer)
{
    m_default_material = std::make_shared<FlatMaterial>(
        m_mat_id++,
        m_renderer->device(),
        *m_renderer->effect<FlatEffect>(),
        0.5, 0.5, 0.5);
}

std::optional<std::shared_ptr<Texture>>
ResourceTable::load_texture(const std::filesystem::path& path)
{
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        if (auto shared = it->second.lock()) {
            return shared;
        }
    }

    auto optional = Texture::from_path(m_renderer->device(), path);
    if (!optional) {
        return std::nullopt;
    }

    auto ptr = std::make_shared<Texture>(std::move(optional.value()));
    std::weak_ptr<Texture> handle = ptr;
    m_textures.emplace(path, std::move(handle));

    return std::optional(std::move(ptr));
}

std::optional<std::shared_ptr<TextureMaterial>>
ResourceTable::load_texture_material(const std::filesystem::path& path)
{
    auto it = m_texture_materials.find(path);
    if (it != m_texture_materials.end()) {
        if (auto shared = it->second.lock()) {
            return shared;
        }
    }

    auto optional = load_texture(path);
    if (!optional) {
        return std::nullopt;
    }

    auto material = std::make_shared<TextureMaterial>(
        m_mat_id++,
        m_renderer->device(),
        *m_renderer->effect<TextureEffect>(),
        std::move(optional.value()),
        m_renderer->default_sampler());
    std::weak_ptr<TextureMaterial> handle = material;
    m_texture_materials.emplace(path, std::move(handle));

    return material;
}

std::optional<std::shared_ptr<Model>>
ResourceTable::load_model(const std::filesystem::path& path)
{
    auto it = m_models.find(path);
    if (it != m_models.end()) {
        if (auto shared = it->second.lock()) {
            return shared;
        }
    }

    std::shared_ptr<Model> model = std::make_shared<Model>();
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path.string(),
        aiProcess_CalcTangentSpace      |
        aiProcess_Triangulate           |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);
    if (scene == nullptr) {
        return std::nullopt;
    }

    for (int i = 0; i < scene->mNumMeshes; ++i) {
        auto mesh = load_mesh(scene->mMeshes[i]);
        model->add_mesh(std::move(mesh));
    }

    for (int i = 0; i < scene->mNumMaterials; ++i) {
        aiMaterial* ai_mat = scene->mMaterials[i];
        int num_textures = ai_mat->GetTextureCount(aiTextureType_DIFFUSE);

        std::optional<std::shared_ptr<Material>> mat;
        if (num_textures > 0) {
            aiString texture_name;
            if (ai_mat->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0),
                            texture_name)) {

                std::filesystem::path name(
                    texture_name.C_Str(),
                    texture_name.C_Str() + texture_name.length);

                mat = load_texture_material(path.parent_path() / name);
            }
        }

        if (mat) {
            model->add_material(std::move(*mat));
        } else {
            model->add_material(m_default_material);
        }
    }

    if (!load_node(*model, scene, glm::mat4(1), scene->mRootNode)) {
        return std::nullopt;
    }

    std::weak_ptr<Model> handle = model;
    m_models.emplace(path, std::move(handle));

    return model;
}

std::unique_ptr<Mesh> ResourceTable::load_mesh(aiMesh* ai_mesh)
{
    std::vector<Vertex> vertices;
    const int vc = ai_mesh->mNumVertices;
    vertices.reserve(vc);
    for (int i = 0; i < vc; ++i) {
        Vertex v;
        v.x = ai_mesh->mVertices[i].x;
        v.y = ai_mesh->mVertices[i].y;
        v.z = ai_mesh->mVertices[i].z;
        v.u = ai_mesh->mTextureCoords[0][i].x;
        v.v = ai_mesh->mTextureCoords[0][i].y;
        vertices.push_back(v);
    }

    std::vector<std::uint16_t> indices;
    const int fc = ai_mesh->mNumFaces;
    indices.reserve(3 * fc);
    for (int i = 0; i < fc; ++i) {
        indices.push_back(ai_mesh->mFaces[i].mIndices[0]);
        indices.push_back(ai_mesh->mFaces[i].mIndices[1]);
        indices.push_back(ai_mesh->mFaces[i].mIndices[2]);
    }

    // padding
    int logical_count = indices.size();
    if (indices.size() % 2 == 1) {
        indices.push_back(0);
    }

    std::unique_ptr<BasicMesh> gpu_mesh = std::make_unique<BasicMesh>(
        m_renderer->device(),
        WGPUPrimitiveTopology_TriangleList,
        vertices,
        indices,
        logical_count
    );

    return std::move(gpu_mesh);
}

bool ResourceTable::load_node(
    Model& model,
    const aiScene* ai_scene,
    const glm::mat4& parent_transform,
    aiNode* ai_node)
{
    // Assimp matrix is row-major, but glm matrix is column-major
    glm::mat4 transform;
    transform[0] = glm::vec4(
        ai_node->mTransformation.a1,
        ai_node->mTransformation.b1,
        ai_node->mTransformation.c1,
        ai_node->mTransformation.d1);
    transform[1] = glm::vec4(
        ai_node->mTransformation.a2,
        ai_node->mTransformation.b2,
        ai_node->mTransformation.c2,
        ai_node->mTransformation.d2);
    transform[2] = glm::vec4(
        ai_node->mTransformation.a3,
        ai_node->mTransformation.b3,
        ai_node->mTransformation.c3,
        ai_node->mTransformation.d3);
    transform[3] = glm::vec4(
        ai_node->mTransformation.a4,
        ai_node->mTransformation.b4,
        ai_node->mTransformation.c4,
        ai_node->mTransformation.d4);

    transform = transform * parent_transform;

    for (int i = 0; i < ai_node->mNumMeshes; ++i) {
        int mesh_idx = ai_node->mMeshes[i];
        int material_idx = ai_scene->mMeshes[mesh_idx]->mMaterialIndex;

        if (mesh_idx < 0 || mesh_idx >= model.meshes().size()) {
            return false;
        }
        if (material_idx < 0 || material_idx >= model.materials().size()) {
            return false;
        }

        model.add_part(ModelPart(mesh_idx, material_idx, transform));
    }

    for (int i = 0; i < ai_node->mNumChildren; ++i) {
        if (!load_node(model, ai_scene, transform, ai_node->mChildren[i])) {
            return false;
        }
    }

    return true;
}

FlatMaterial& ResourceTable::add_flat_material(float r, float g, float b)
{
    auto ptr = std::make_unique<FlatMaterial>(
        m_mat_id++,
        m_renderer->device(),
        *m_renderer->effect<FlatEffect>(),
        r, g, b);
    FlatMaterial& mat = *ptr;
    m_materials.emplace_back(std::move(ptr));
    return mat;
}

FlatMaterial& ResourceTable::add_wireframe_material(float r, float g, float b)
{
    auto ptr = std::make_unique<FlatMaterial>(
        m_mat_id++,
        m_renderer->device(),
        *m_renderer->effect<FlatEffect>(),
        r, g, b);
    FlatMaterial& mat = *ptr;
    m_materials.emplace_back(std::move(ptr));
    return mat;
}

TextureMaterial& ResourceTable::add_texture_material(
    std::shared_ptr<Texture> texture)
{
    auto ptr = std::make_unique<TextureMaterial>(
        m_mat_id++,
        m_renderer->device(),
        *m_renderer->effect<TextureEffect>(),
        std::move(texture),
        m_renderer->default_sampler());
    TextureMaterial& mat = *ptr;
    m_materials.emplace_back(std::move(ptr));
    return mat;
}
