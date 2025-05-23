#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H

#include "camera.h"

class PerspectiveCamera : public Camera
{
public:
    PerspectiveCamera();

    virtual void set_position(const glm::vec3& position) override;
    virtual void set_target(const glm::vec3& target) override;
    virtual void update_matrix(CameraData*) override;

private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;
};

#endif
