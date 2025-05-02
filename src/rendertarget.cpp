#include "noworry/rendertarget.h"

RenderTarget::RenderTarget(WGPUDevice device, int width, int height)
    : m_clear_color {0, 0, 0, 1},
      m_texture(device, width, height,
                WGPUTextureFormat_BGRA8Unorm,
                WGPUTextureUsage_TextureBinding
                | WGPUTextureUsage_RenderAttachment),
      m_depth_texture(device, width, height,
                      WGPUTextureFormat_Depth24Plus,
                      WGPUTextureUsage_RenderAttachment)
{
}

void RenderTarget::set_clear_color(const WGPUColor& color)
{
    m_clear_color = color;
}
