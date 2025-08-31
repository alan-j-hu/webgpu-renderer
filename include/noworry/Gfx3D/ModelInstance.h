#ifndef MODEL_INSTANCE_H
#define MODEL_INSTANCE_H

#include "Renderable.h"
#include <vector>
#include <glm/mat4x4.hpp>

class Model;

class ModelInstance : public Renderable
{
public:
    ModelInstance(const Model&);

    const glm::mat4& transform() const;

    glm::mat4& transform();

    virtual void render(Frame& frame) override;

private:
    glm::mat4 m_transform;
    const Model* m_model;
    std::vector<glm::mat4> m_parts;
};

#endif
