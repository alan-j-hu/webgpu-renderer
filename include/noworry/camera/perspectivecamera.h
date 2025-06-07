#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H

#include "abstractcamera.h"

// Perspective camera, where objects that are far away appear smaller.
class PerspectiveCamera : public AbstractCamera
{
public:
    PerspectiveCamera();

protected:
    virtual glm::mat4 proj() const override;
};

#endif
