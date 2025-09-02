#ifndef RESOURCE_LOADER_H
#define RESOURCE_LOADER_H

#include "../resourcetable.h"

#include <memory>
#include <optional>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

template<>
class ResourceLoader<Texture>
{
public:
    std::optional<std::shared_ptr<Texture>> load(
        ResourceTable& resources,
        const std::filesystem::path& path)
    {
        auto texture = Texture::from_path(resources.renderer().device(), path);
        if (!texture) {
            return std::nullopt;
        }

        return std::make_shared<Texture>(std::move(texture.value()));
    }
};

template<>
class ResourceLoader<TextureMaterial>
{
public:
    std::optional<std::shared_ptr<TextureMaterial>> load(
        ResourceTable& resources,
        const std::filesystem::path& path)
    {
        auto texture = resources.load<Texture>(path);
        if (!texture) {
            return std::nullopt;
        }

        return std::make_shared<TextureMaterial>(
            0,
            resources.renderer().device(),
            *resources.renderer().effect<TextureEffect>(),
            std::move(texture.value()),
            resources.renderer().default_sampler());
    }
};

template<>
class ResourceLoader<Model>
{
public:
    std::optional<std::shared_ptr<Model>> load(
        ResourceTable& resources,
        const std::filesystem::path& path)
    {
        std::shared_ptr<Model> model = std::make_shared<Model>();
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(
            path.string(),
            aiProcess_CalcTangentSpace      |
            aiProcess_GlobalScale           |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType           |
            aiProcess_Triangulate);
        if (scene == nullptr) {
            return std::nullopt;
        }

        for (int i = 0; i < scene->mNumMeshes; ++i) {
            auto mesh = load_mesh(resources, scene->mMeshes[i]);
            model->add_mesh(std::move(mesh));
        }

        for (int i = 0; i < scene->mNumMaterials; ++i) {
            aiMaterial* ai_mat = scene->mMaterials[i];
            std::optional<std::shared_ptr<Material>> mat;
            if (ai_mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                aiString texture_name;
                aiReturn retcode =
                    ai_mat->GetTexture(aiTextureType_DIFFUSE, 0,
                                       &texture_name);
                if (AI_SUCCESS == retcode) {
                    std::filesystem::path name(
                        texture_name.C_Str(),
                        texture_name.C_Str() + texture_name.length);

                    mat = resources.load<TextureMaterial>(
                        path.parent_path() / name);
                }
            }

            if (mat) {
                model->add_material(std::move(*mat));
            } else {
                model->add_material(resources.default_material());
            }
        }

        if (!load_node(*model, scene, glm::mat4(1), scene->mRootNode)) {
            return std::nullopt;
        }

        return model;
    }


    std::unique_ptr<Mesh> load_mesh(
        ResourceTable& resources,
        aiMesh* ai_mesh)
    {
        std::vector<Vertex> vertices;
        const int vc = ai_mesh->mNumVertices;
        vertices.reserve(vc);
        for (int i = 0; i < vc; ++i) {
            Vertex v;
            v.x = ai_mesh->mVertices[i].x;
            v.y = -ai_mesh->mVertices[i].z;
            v.z = ai_mesh->mVertices[i].y;
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
            resources.renderer().device(),
            WGPUPrimitiveTopology_TriangleList,
            vertices,
            indices,
            logical_count);

        return std::move(gpu_mesh);
    }

    bool load_node(
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
};

#endif
