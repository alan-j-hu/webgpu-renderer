#include "noworry/camera/perspectivecamera.h"
#include "noworry/layout.h"
#include <cstring>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

PerspectiveCamera::PerspectiveCamera()
{
    init();
}

glm::mat4 PerspectiveCamera::proj() const
{
    return glm::perspectiveRH_ZO(
        glm::pi<float>() * 0.5f,
        4.0f / 3.0f,
        0.1f,
        100.f);
}
