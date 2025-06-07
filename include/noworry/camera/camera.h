#ifndef CAMERA_H
#define CAMERA_H

#include <glm/vec3.hpp>

class CameraData;

// Camera interface.
struct Camera
{
public:
    virtual void set_position(const glm::vec3& position) = 0;
    virtual void set_target(const glm::vec3& target) = 0;
    virtual void update_matrix(CameraData*) = 0;

    /// Given a point in [-1, 1] NDC, returns the point in world space.
    /// The z coordinate interval is [0, 1].
    virtual glm::vec3 unproject(glm::vec3 ndc) = 0;
};

#endif
