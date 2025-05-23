#ifndef ORTHOGRAPHICCAMERA_H
#define ORTHOGRAPHICCAMERA_H

#include "camera.h"

class OrthographicCamera : public Camera
{
public:
    OrthographicCamera();

    virtual void set_position(const glm::vec3& position) override;
    virtual void set_target(const glm::vec3& target) override;
    virtual void update_matrix(CameraData*) override;

private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;

    float m_left;
    float m_right;
    float m_bottom;
    float m_top;
    float m_near;
    float m_far;
};

#endif
