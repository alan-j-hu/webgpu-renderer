#ifndef ORTHOGRAPHICCAMERA_H
#define ORTHOGRAPHICCAMERA_H

#include "abstractcamera.h"

// Orthographic camera, where objects appear the same size regardless
// of their distance from the camera.
class OrthographicCamera : public AbstractCamera
{
public:
    OrthographicCamera();

    virtual void update_matrix(CameraData*) override;

private:
    float m_left;
    float m_right;
    float m_bottom;
    float m_top;
    float m_near;
    float m_far;
};

#endif
