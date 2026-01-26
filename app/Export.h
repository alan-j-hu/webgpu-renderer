#ifndef EXPORT_H
#define EXPORT_H

#include "Project.h"
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <filesystem>
#include <memory>

class Exporter
{
public:
    Assimp::Exporter& ai_exporter();

    void export_project(
        const Project&,
        const aiExportFormatDesc&,
        const std::filesystem::path&);

    void export_world(
        const World&,
        const aiExportFormatDesc&,
        const std::filesystem::path&);

private:
    std::vector<std::unique_ptr<aiMaterial>> m_materials;
    std::unordered_map<const Material*, int> m_material_indices;
    Assimp::Exporter m_ai_exporter;

    void export_level(
        const Level&,
        const aiExportFormatDesc&,
        const std::filesystem::path&);

    void clear_materials();

    std::unique_ptr<aiMesh> create_mesh(const DynamicMesh&);

    std::unique_ptr<aiMaterial> create_material(const Material&);

    int get_or_create_material(const Material&);
};

#endif
