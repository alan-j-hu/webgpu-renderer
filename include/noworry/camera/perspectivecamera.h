#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H

#include "abstractcamera.h"

// Perspective camera, where objects that are far away appear smaller.
class PerspectiveCamera : public AbstractCamera
{
public:
    PerspectiveCamera();

    virtual void update_matrix(CameraData*) override;

private:
};

#endif
