#ifndef GRID_H
#define GRID_H

#include "mesh.h"
#include <glm/vec3.hpp>

Mesh create_grid(
    WGPUDevice device,
    const glm::vec3& pos,
    int w,
    int h,
    const glm::vec3& axis1,
    const glm::vec3& axis2);

#endif
