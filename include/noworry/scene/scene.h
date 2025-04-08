#ifndef SCENE_H
#define SCENE_H

#include "../material.h"
#include "../mesh.h"
#include "model.h"
#include "../renderer.h"
#include <memory>
#include <vector>

class Scene
{
public:
    using ModelIterator = std::vector<std::unique_ptr<Model>>::iterator;

    Scene(Renderer&);

    Model& add_model(const Mesh& mesh, Material& mat);

    ModelIterator models_begin() { return m_models.begin(); }
    ModelIterator models_end() { return m_models.end(); }

private:
    Renderer* m_renderer;
    std::vector<std::unique_ptr<Model>> m_models;
};

#endif
