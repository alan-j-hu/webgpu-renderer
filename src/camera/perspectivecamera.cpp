#include "noworry/camera/perspectivecamera.h"
#include "noworry/layout.h"
#include <cstring>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

PerspectiveCamera::PerspectiveCamera()
{
}

void PerspectiveCamera::update_matrix(CameraData* data)
{
    glm::mat4 proj = glm::perspectiveRH_ZO(
        glm::pi<float>() * 0.5f,
        4.0f / 3.0f,
        0.1f,
        100.f);
    glm::mat4 view = glm::lookAtRH(position(), target(), up());
    data->viewproj = proj * view;
}
