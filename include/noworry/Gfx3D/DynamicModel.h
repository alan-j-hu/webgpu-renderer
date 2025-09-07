#ifndef DYNAMIC_MODEL_H
#define DYNAMIC_MODEL_H

#include "DynamicMesh.h"
#include "../Material/Material.h"
#include <unordered_map>
#include <glm/mat4x4.hpp>

class ModelData;

class DynamicModel
{
    using map = std::unordered_map<Material*, DynamicMesh>;
public:
    using const_iterator = map::const_iterator;

    void add_model(const ModelData&, const glm::mat4&);

    void flush(WGPUDevice device);

    void reset();

    const_iterator begin() const;
    const_iterator end() const;

private:
    map m_parts;
};

#endif
