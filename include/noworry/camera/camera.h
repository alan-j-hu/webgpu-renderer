#ifndef CAMERA_H
#define CAMERA_H

#include <glm/vec3.hpp>

class CameraData;

struct Camera
{
public:
    virtual void set_position(const glm::vec3& position) = 0;
    virtual void set_target(const glm::vec3& target) = 0;
    virtual void update_matrix(CameraData*) = 0;
};

#endif
