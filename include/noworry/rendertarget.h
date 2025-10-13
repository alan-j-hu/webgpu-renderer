#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include "texture.h"
#include <webgpu/webgpu.h>

class RenderTarget
{
public:
    RenderTarget() = default;
    RenderTarget(WGPUDevice device, int w, int h);

    Texture& texture() { return m_texture; }
    Texture& depth_texture() { return m_depth_texture; }

    int width() const { return m_texture.width(); }
    int height() const { return m_texture.height(); }

    WGPUColor clear_color() { return m_clear_color; }

    void set_clear_color(const WGPUColor& color);

private:
    WGPUColor m_clear_color;
    Texture m_texture;
    Texture m_depth_texture;
};

#endif
