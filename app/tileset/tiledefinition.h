#ifndef TILESET_TILEDEFINITION_H
#define TILESET_TILEDEFINITION_H

#include "tilemesh.h"
#include "noworry/scene/scene.h"

class TilesetEditor;

class TileDefinition
{
public:
    TileDefinition(TilesetEditor&, TileMesh&, int w, int h);
    TileDefinition(const TileDefinition&) = delete;
    TileDefinition& operator=(const TileDefinition&) = delete;
    TileDefinition(TileDefinition&&) = default;
    TileDefinition& operator=(TileDefinition&&) = default;

    Scene& scene() { return m_scene; }

    TileMesh& mesh() { return *m_mesh; }

private:
    int m_width;
    int m_height;
    TileMesh* m_mesh;
    TilesetEditor* m_tileset_editor;
    Scene m_scene;
    Model* m_model;
};

#endif
