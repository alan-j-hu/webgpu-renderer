#include "Export.h"
#include "noworry/Gfx3D/DynamicModel.h"

Assimp::Exporter& Exporter::ai_exporter()
{
    return m_ai_exporter;
}

void Exporter::export_project(
    const Project& project,
    const aiExportFormatDesc& format,
    const std::filesystem::path& path)
{
    std::filesystem::create_directory(path);

    for (int i = 0; i < project.world_count(); ++i) {
        export_world(project.world_at(i), format, path / std::to_string(i));
    }
}

void Exporter::export_world(
    const World& world,
    const aiExportFormatDesc& format,
    const std::filesystem::path& path)
{
    std::filesystem::create_directory(path);

    for (auto& [name, level] : world) {
        export_level(
            *level,
            format,
            path / (name + '.' + format.fileExtension));
    }
}

void Exporter::export_level(
    const Level& level,
    const aiExportFormatDesc& format,
    const std::filesystem::path& path)
{
    clear_materials();

    DynamicModel model;
    for (int i = 0; i < level.layer_count(); ++i) {
        level.layer_at(i).fill_model(model);
    }

    std::vector<std::unique_ptr<aiMesh>> ai_meshes;

    for (auto& part : model) {
        ai_meshes.push_back(create_mesh(part.second));
    }

    auto ai_scene = std::make_unique<aiScene>();
    ai_scene->mFlags |= AI_SCENE_FLAGS_NON_VERBOSE_FORMAT;

    ai_scene->mNumMaterials = m_materials.size();
    ai_scene->mMaterials = new aiMaterial*[ai_scene->mNumMaterials];
    for (int i = 0; i < ai_scene->mNumMaterials; ++i) {
        ai_scene->mMaterials[i] = m_materials.at(i).release();
    }

    auto root_node = std::make_unique<aiNode>("root");
    std::vector<std::unique_ptr<aiNode>> children;

    ai_scene->mNumMeshes = ai_meshes.size();
    ai_scene->mMeshes = new aiMesh*[ai_scene->mNumMeshes];
    for (int i = 0; i < ai_scene->mNumMeshes; ++i) {
        ai_scene->mMeshes[i] = ai_meshes.at(i).release();

        auto node = std::make_unique<aiNode>(std::to_string(i));
        node->mNumMeshes = 1;
        node->mMeshes = new unsigned int[1];
        node->mMeshes[0] = i;
        children.push_back(std::move(node));
    }

    root_node->mNumChildren = children.size();
    root_node->mChildren = new aiNode*[root_node->mNumChildren];
    for (int i = 0; i < root_node->mNumChildren; ++i) {
        root_node->mChildren[i] = children.at(i).release();
    }
    ai_scene->mRootNode = root_node.release();

    auto string_path = path.string();
    m_ai_exporter.Export(ai_scene.get(), format.id, string_path.c_str());
}

void Exporter::clear_materials()
{
    m_material_indices.clear();
    m_materials.clear();
}

std::unique_ptr<aiMaterial> Exporter::create_material(const Material& material)
{
    auto ai_material = std::make_unique<aiMaterial>();

    if (auto diffuse_map = material.diffuse_map()) {
        if (auto path = (*diffuse_map)->path()) {
            aiString name(path->string());
            ai_material->AddProperty(&name, AI_MATKEY_TEXTURE_DIFFUSE(0));
        }
    }

    return ai_material;
}

int Exporter::get_or_create_material(const Material& material)
{
    auto it = m_material_indices.find(&material);
    if (it == m_material_indices.end()) {
        int index = m_materials.size();
        m_material_indices.emplace(&material, index);
        m_materials.push_back(create_material(material));
        return index;
    }

    return it->second;
}

std::unique_ptr<aiMesh> Exporter::create_mesh(const DynamicMesh& mesh)
{
    auto vertices = mesh.mesh_data().vertices();
    auto vertex_count = vertices.size();
    auto ai_positions = std::make_unique<aiVector3D[]>(vertices.size());
    auto ai_texture_coords = std::make_unique<aiVector3D[]>(vertex_count);
    for (int i = 0; i < vertex_count; ++i) {
        auto& vertex = vertices[i];
        ai_positions.get()[i] = aiVector3D(vertex.x, vertex.z, -vertex.y);
        ai_texture_coords.get()[i] = aiVector3D(vertex.u, 1 - vertex.v, 0);
    }

    auto face_count = mesh.index_count() / 3;
    auto ai_faces = std::make_unique<aiFace[]>(face_count);
    auto indices = mesh.mesh_data().indices();
    for (int i = 0; i < face_count; ++i) {
        aiFace& face = ai_faces.get()[i];
        face.mNumIndices = 3;
        face.mIndices = new unsigned int[3];
        face.mIndices[0] = indices[3 * i];
        face.mIndices[1] = indices[3 * i + 1];
        face.mIndices[2] = indices[3 * i + 2];
    }

    auto ai_mesh = std::make_unique<aiMesh>();
    ai_mesh->mPrimitiveTypes = aiPrimitiveType_TRIANGLE;
    ai_mesh->mNumVertices = vertex_count;
    ai_mesh->mVertices = ai_positions.release();
    ai_mesh->mNumUVComponents[0] = 2;
    ai_mesh->mTextureCoords[0] = ai_texture_coords.release();
    ai_mesh->mNumFaces = face_count;
    ai_mesh->mFaces = ai_faces.release();

    ai_mesh->mMaterialIndex =
        get_or_create_material(*mesh.mesh_data().material());

    return ai_mesh;
}
