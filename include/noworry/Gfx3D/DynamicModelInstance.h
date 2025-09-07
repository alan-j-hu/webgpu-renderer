#ifndef DYNAMIC_MODEL_INSTANCE_H
#define DYNAMIC_MODEL_INSTANCE_H

#include "Renderable.h"
#include <glm/mat4x4.hpp>

class DynamicModel;

class DynamicModelInstance : Renderable
{
public:
    DynamicModelInstance(const DynamicModel&);

    virtual void render(Frame& frame) override;

private:
    glm::mat4 m_transform;
    const DynamicModel* m_dynamic;
};

#endif
