#ifndef APPLICATION_H
#define APPLICATION_H

#include <GLFW/glfw3.h>
#include <webgpu/webgpu.h>

class Application
{
public:
    Application(int width, int height, WGPUTextureFormat format);

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    virtual ~Application();

    int width() { return m_width; }
    int height() { return m_height; }

    int run();

    void tick();

    virtual void render(WGPUTextureView view) = 0;

protected:
    GLFWwindow* window() { return m_window; }

    WGPUDevice device() { return m_device; }

    WGPUSurface surface() { return m_surface; }

    virtual void resize(int width, int height);

private:
    int m_width;
    int m_height;
    GLFWwindow* m_window;
    WGPUInstance m_instance;
    WGPUAdapter m_adapter;
    WGPUDevice m_device;
    WGPUSurface m_surface;
    WGPUTextureFormat m_format;

    static void resize_callback(GLFWwindow* window, int width, int height);
};

#endif
