#ifndef ORTHOGRAPHICCAMERA_H
#define ORTHOGRAPHICCAMERA_H

#include "abstractcamera.h"

// Orthographic camera, where objects appear the same size regardless
// of their distance from the camera.
class OrthographicCamera : public AbstractCamera
{
public:
    OrthographicCamera();

    int top() { return m_top; }
    void set_top(int top)
    {
        m_top = top;
        update_viewproj();
    }

    int bottom() { return m_bottom; }
    void set_bottom(int bottom)
    {
        m_bottom = bottom;
        update_viewproj();
    }

    int left() { return m_left; }
    void set_left(int left)
    {
        m_left = left;
        update_viewproj();
    }

    int right() { return m_right; }
    void set_right(int right)
    {
        m_right = right;
        update_viewproj();
    }

protected:
    virtual glm::mat4 proj() const override;

private:
    float m_left;
    float m_right;
    float m_bottom;
    float m_top;
    float m_near;
    float m_far;
};

#endif
