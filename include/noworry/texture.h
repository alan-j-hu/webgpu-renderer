#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstddef>
#include <filesystem>
#include <optional>
#include <webgpu/webgpu.h>

/// A 2D texture stored in GPU memory.
class Texture
{
public:
    Texture();
    Texture(WGPUDevice device, int w, int h);
    Texture(WGPUDevice device, int w, int h,
            WGPUTextureFormat format, WGPUTextureUsage usage);

    /// Loads an image file from memory.
    static std::optional<Texture> from_memory(
        WGPUDevice device,
        unsigned char* buffer,
        std::size_t size);

    /// Loads an image file from the path.
    static std::optional<Texture> from_path(
        WGPUDevice device,
        const std::filesystem::path& path);

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&&);
    Texture& operator=(Texture&&);
    ~Texture();

    std::uint32_t width() const;
    std::uint32_t height() const;

    WGPUTextureView view() const { return m_view; }

    void upload(WGPUDevice device, void* data);

    const std::optional<std::filesystem::path>& path() const;

    void set_path(std::filesystem::path);

private:
    WGPUTexture m_texture;
    WGPUTextureView m_view;
    std::optional<std::filesystem::path> m_path;

    void init(WGPUDevice, int w, int h,
              WGPUTextureFormat format, WGPUTextureUsage usage);
};

#endif
