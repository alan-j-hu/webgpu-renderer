#ifndef MATERIAL_H
#define MATERIAL_H

#include "Effect.h"
#include "../texture.h"
#include <glm/vec4.hpp>
#include <webgpu/webgpu.h>
#include <optional>

class Material
{
public:
    virtual ~Material() = default;

    virtual WGPUBindGroup bind_group() const = 0;
    virtual Effect& effect() const = 0;

    virtual glm::vec4 diffuse_factor() const
    {
        return glm::vec4(1, 1, 1, 1);
    }

    virtual std::optional<const Texture*> diffuse_map() const
    {
        return std::nullopt;
    }
};

#endif
