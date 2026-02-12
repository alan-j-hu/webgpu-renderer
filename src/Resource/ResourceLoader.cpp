#include "noworry/Resource/ResourceLoader.h"

std::optional<std::shared_ptr<Texture>> ResourceLoader<Texture>::load(
    ResourceTable& resources,
    const std::filesystem::path& path)
{
    auto texture = Texture::from_path(resources.renderer().device(), path);
    if (!texture) {
        return std::nullopt;
    }

    return std::make_shared<Texture>(std::move(texture.value()));
}

std::optional<std::shared_ptr<TextureMaterial>>
ResourceLoader<TextureMaterial>::load(
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

std::optional<std::shared_ptr<Model>> ResourceLoader<Model>::load(
    ResourceTable& resources,
    const std::filesystem::path& path)
{
    Assimp::Importer importer;

    ResourceLoader<ModelData> loader;
    auto opt = loader.load(resources, path);
    if (!opt) {
        return std::nullopt;
    }

    return std::make_shared<Model>(
        resources.renderer().device(),
        **opt
    );
}

std::optional<std::shared_ptr<ModelData>> ResourceLoader<ModelData>::load(
    ResourceTable& resources,
    const std::filesystem::path& path)
{
    std::shared_ptr<ModelData> model = std::make_shared<ModelData>();
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

    // Swap y and z
    glm::mat4 initial_transform;
    initial_transform[0] = glm::vec4(1, 0, 0, 0); // Col 1
    initial_transform[1] = glm::vec4(0, 0, 1, 0); // Col 2
    initial_transform[2] = glm::vec4(0, -1, 0, 0); // Col 3
    initial_transform[3] = glm::vec4(0, 0, 0, 1); // Col 4

    if (!load_node(resources,
                   path,
                   *model,
                   scene,
                   initial_transform,
                   scene->mRootNode)) {

        return std::nullopt;
    }

    return model;
}

bool ResourceLoader<ModelData>::load_node(
    ResourceTable& resources,
    const std::filesystem::path& path,
    ModelData& model,
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
        aiMesh* ai_mesh = ai_scene->mMeshes[mesh_idx];

        // Vertex buffer
        std::vector<Vertex> vertices;
        const int vc = ai_mesh->mNumVertices;
        vertices.reserve(vc);
        for (int j = 0; j < vc; ++j) {
            glm::vec4 input(
                ai_mesh->mVertices[j].x,
                ai_mesh->mVertices[j].y,
                ai_mesh->mVertices[j].z,
                1);
            glm::vec4 output = transform * input;

            Vertex v;
            v.x = output.x;
            v.y = output.y;
            v.z = output.z;
            v.u = ai_mesh->mTextureCoords[0][j].x;
            v.v = 1 - ai_mesh->mTextureCoords[0][j].y;
            vertices.push_back(v);
        }

        // Index buffer
        std::vector<std::uint16_t> indices;
        const int fc = ai_mesh->mNumFaces;
        indices.reserve(3 * fc);
        for (int j = 0; j < fc; ++j) {
            indices.push_back(ai_mesh->mFaces[j].mIndices[0]);
            indices.push_back(ai_mesh->mFaces[j].mIndices[1]);
            indices.push_back(ai_mesh->mFaces[j].mIndices[2]);
        }

        // Material
        aiMaterial* ai_mat = ai_scene->mMaterials[material_idx];
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

        int index_count = indices.size();
        if (index_count % 2 == 1) {
            // Padding
            indices.push_back(0);
        }

        auto material = mat ? std::move(*mat) : resources.default_material();
        model.add_part(
            ModelData::Part(std::move(vertices),
                            std::move(indices),
                            std::move(material),
                            index_count));
    }

    for (int i = 0; i < ai_node->mNumChildren; ++i) {
        if (!load_node(resources,
                       path,
                       model,
                       ai_scene,
                       transform,
                       ai_node->mChildren[i])) {

            return false;
        }
    }

    return true;
}
