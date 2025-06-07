#ifndef ABSTRACTCAMERA_H
#define ABSTRACTCAMERA_H

#include "camera.h"
#include <utility>
#include <glm/mat4x4.hpp>

// A camera that handles adjusting the up vector from a lookat vector.
class AbstractCamera : public Camera
{
public:
    void init();
    virtual void set_position(const glm::vec3& position) override;
    virtual void set_target(const glm::vec3& target) override;
    virtual void update_matrix(CameraData*) override;
    virtual glm::vec3 unproject(glm::vec3 ndc) override;

protected:
    glm::vec3 position() const;
    glm::vec3 target() const;
    glm::vec3 up() const;

    virtual glm::mat4 proj() const = 0;
    void update_viewproj();

private:
    static constexpr float EPSILON = 0.000001;
    static constexpr glm::vec3 FORWARD = glm::vec3(0, 1, 0);
    static constexpr glm::vec3 UP = glm::vec3(0, 0, 1);

    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;
    glm::mat4 m_viewproj;
    glm::mat4 m_inv_viewproj;

    void update_lookat();
};

#endif
