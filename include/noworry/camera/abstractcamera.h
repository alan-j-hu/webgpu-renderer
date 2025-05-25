#ifndef ABSTRACTCAMERA_H
#define ABSTRACTCAMERA_H

#include "camera.h"

// A camera that handles adjusting the up vector from a lookat vector.
class AbstractCamera : public Camera
{
public:
    virtual void set_position(const glm::vec3& position) override;
    virtual void set_target(const glm::vec3& target) override;

protected:
    glm::vec3 position();
    glm::vec3 target();
    glm::vec3 up();

private:
    static constexpr float EPSILON = 0.000001;
    static constexpr glm::vec3 FORWARD = glm::vec3(0, 1, 0);
    static constexpr glm::vec3 UP = glm::vec3(0, 0, 1);

    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;

    void update_lookat();
};

#endif
