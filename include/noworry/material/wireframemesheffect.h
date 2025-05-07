#ifndef WIREFRAMEMESHEFFECT_H
#define WIREFRAMEMESHEFFECT_H

#include "flatmesheffect.h"

class WireframeMeshEffect : public FlatMeshEffect
{
public:
    WireframeMeshEffect(WGPUDevice device, UniformLayout& ul)
        : FlatMeshEffect(device, ul)
    {}
    WireframeMeshEffect(const WireframeMeshEffect&) = delete;
    WireframeMeshEffect& operator=(const WireframeMeshEffect&) = delete;

    virtual WGPUPrimitiveTopology topology() override
    {
        return WGPUPrimitiveTopology_LineList;
    }
};

#endif
