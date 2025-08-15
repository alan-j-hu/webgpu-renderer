#ifndef GRID_H
#define GRID_H

#include "Gfx3D/BasicMesh.h"
#include <glm/vec3.hpp>

BasicMesh create_grid(
    WGPUDevice device,
    const glm::vec3& pos,
    int w,
    int h,
    const glm::vec3& axis1,
    const glm::vec3& axis2);

#endif
