#ifndef TEXTURE_H
#define TEXTURE_H

#include <cstddef>
#include <filesystem>
#include <webgpu/webgpu.h>

class Texture
{
public:
    Texture(WGPUDevice device, int w, int h);
    Texture(WGPUDevice device, unsigned char* buffer, std::size_t size);
    Texture(WGPUDevice device, const std::filesystem::path& path);

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&&);
    ~Texture();

    WGPUTextureView view() const { return m_view; }

    void upload(WGPUDevice device, void* data);
private:
    bool m_moved_from;
    WGPUTexture m_texture;
    WGPUTextureView m_view;

    void init(WGPUDevice, int w, int h);
};

#endif
