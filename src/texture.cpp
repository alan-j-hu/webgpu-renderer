#include "noworry/texture.h"
#include <utility>
#include "../third_party/stb/stb_image.h"

namespace fs = std::filesystem;

Texture::Texture(WGPUDevice device, int w, int h)
    : m_moved_from{false}
{
    init(device, w, h,
         WGPUTextureFormat_RGBA8Unorm,
         WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst);
}

Texture::Texture(WGPUDevice device, int w, int h,
                 WGPUTextureFormat format,
                 WGPUTextureUsage usage)
    : m_moved_from{false}
{
    init(device, w, h, format, usage);
}

Texture::Texture(Texture&& other)
    : m_moved_from {false},
      m_texture {other.m_texture},
      m_view {other.m_view}
{
    other.m_moved_from = true;
}

Texture::Texture(WGPUDevice device, unsigned char* buffer, std::size_t size)
    : m_moved_from{false}
{
    int w;
    int h;
    int n;
    stbi_uc* data = stbi_load_from_memory(buffer, size, &w, &h, &n, 4);
    init(device, w, h,
         WGPUTextureFormat_RGBA8Unorm,
         WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst);
    upload(device, data);
    stbi_image_free(data);
}

Texture::Texture(WGPUDevice device, const fs::path& path)
    : m_moved_from{false}
{
    int w;
    int h;
    int n;
    stbi_uc* data = stbi_load(path.string().c_str(), &w, &h, &n, 4);
    init(device, w, h,
         WGPUTextureFormat_RGBA8Unorm,
         WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst);
    upload(device, data);
    stbi_image_free(data);
}

Texture::~Texture()
{
    if (m_moved_from) {
        return;
    }
    wgpuTextureViewRelease(m_view);
    wgpuTextureRelease(m_texture);
}

void Texture::upload(WGPUDevice device, void* data)
{
    const int w = wgpuTextureGetWidth(m_texture);
    const int h = wgpuTextureGetHeight(m_texture);

    WGPUTexelCopyTextureInfo dst = { 0 };
    dst.texture = m_texture;
    dst.mipLevel = 0;
    dst.origin = { 0, 0, 0 };
    dst.aspect = WGPUTextureAspect_All;

    WGPUTexelCopyBufferLayout src = { 0 };
    src.offset = 0;
    src.bytesPerRow = 4 * w;
    src.rowsPerImage = h;

    WGPUQueue queue = wgpuDeviceGetQueue(device);
    WGPUExtent3D extent = { 0 };
    extent.width = w;
    extent.height = h;
    extent.depthOrArrayLayers = 1;

    wgpuQueueWriteTexture(queue, &dst, data, 4 * w * h, &src, &extent);
}

void Texture::init(
    WGPUDevice device, int w, int h,
    WGPUTextureFormat format,
    WGPUTextureUsage usage)
{
    WGPUTextureDescriptor texture_desc = { 0 };
    texture_desc.usage = usage;
    texture_desc.dimension = WGPUTextureDimension_2D;
    texture_desc.size.width = w;
    texture_desc.size.height = h;
    texture_desc.size.depthOrArrayLayers = 1;
    texture_desc.format = format;
    texture_desc.mipLevelCount = 1;
    texture_desc.sampleCount = 1;
    texture_desc.viewFormatCount = 0;
    texture_desc.viewFormats = nullptr;

    m_texture = wgpuDeviceCreateTexture(device, &texture_desc);

    WGPUTextureViewDescriptor view_desc = { 0 };
    view_desc.format = format;
    view_desc.dimension = WGPUTextureViewDimension_2D;
    view_desc.baseMipLevel = 0;
    view_desc.mipLevelCount = 1;
    view_desc.baseArrayLayer = 0;
    view_desc.arrayLayerCount = 1;
    view_desc.aspect = WGPUTextureAspect_All;

    m_view = wgpuTextureCreateView(m_texture, &view_desc);
}