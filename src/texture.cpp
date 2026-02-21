#include "noworry/texture.h"
#include <utility>
#include "stb_image.h"

namespace fs = std::filesystem;

Texture::Texture()
    : m_texture(nullptr), m_view(nullptr)
{
}

Texture::Texture(WGPUDevice device, int w, int h)
{
    init(device, w, h,
         WGPUTextureFormat_RGBA8Unorm,
         WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst);
}

Texture::Texture(WGPUDevice device, int w, int h,
                 WGPUTextureFormat format,
                 WGPUTextureUsage usage)
{
    init(device, w, h, format, usage);
}

Texture::Texture(Texture&& other)
    : m_texture {other.m_texture},
      m_view {other.m_view},
      m_path(std::move(other.m_path))
{
    other.m_texture = nullptr;
    other.m_view = nullptr;
}

Texture& Texture::operator=(Texture&& other)
{
    wgpuTextureViewRelease(m_view);
    wgpuTextureRelease(m_texture);

    m_texture = other.m_texture;
    other.m_texture = nullptr;

    m_view = other.m_view;
    other.m_view = nullptr;

    m_path = std::move(other.m_path);

    return *this;
}

Texture::~Texture()
{
    if (m_view != nullptr) {
        wgpuTextureViewRelease(m_view);
    }
    if (m_texture != nullptr) {
        wgpuTextureRelease(m_texture);
    }
}

std::optional<Texture> Texture::from_memory(
    WGPUDevice device,
    unsigned char* buffer,
    std::size_t size)
{
    int w;
    int h;
    int n;
    stbi_uc* data = stbi_load_from_memory(buffer, size, &w, &h, &n, 4);
    if (data == nullptr) {
        return std::nullopt;
    }

    Texture texture(device, w, h);
    texture.upload(device, data);
    stbi_image_free(data);

    return std::optional(std::move(texture));
}

std::optional<Texture> Texture::from_path(
    WGPUDevice device,
    const fs::path& path)
{
    int w;
    int h;
    int n;
    stbi_uc* data = stbi_load(path.string().c_str(), &w, &h, &n, 4);
    if (data == nullptr) {
        return std::nullopt;
    }

    Texture texture(device, w, h);
    texture.upload(device, data);
    stbi_image_free(data);

    texture.set_path(path);

    return std::optional(std::move(texture));
}

std::uint32_t Texture::width() const
{
    return wgpuTextureGetWidth(m_texture);
}

std::uint32_t Texture::height() const
{
    return wgpuTextureGetHeight(m_texture);
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

const std::optional<fs::path>& Texture::path() const
{
    return m_path;
}

void Texture::set_path(fs::path path)
{
    m_path = std::move(path);
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
