#ifndef CAMERA_H
#define CAMERA_H

#include <glm/vec3.hpp>

class CameraData;

/* The camera uses a left-handed coordinate system with Y-up. */
class Camera
{
public:
    Camera();

    void set_position(const glm::vec3& position);
    void set_target(const glm::vec3& target);
    void update_matrix(CameraData*);

private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;
};

#endif
