#ifndef APPLICATION_H
#define APPLICATION_H

#include <GLFW/glfw3.h>
#include <webgpu/webgpu.h>

class Application
{
public:
    Application(int width, int height);

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    virtual ~Application();

    int run();

    void tick();

protected:
    GLFWwindow* window() { return m_window; }

    WGPUDevice device() { return m_device; }

    WGPUSurface surface()
    {
        return m_surface;
    }
    virtual void pre_render(WGPUQueue queue) = 0;

    virtual void render(WGPURenderPassEncoder) = 0;

private:
    int m_width;
    int m_height;
    GLFWwindow* m_window;
    WGPUInstance m_instance;
    WGPUAdapter m_adapter;
    WGPUDevice m_device;
    WGPUSurface m_surface;

    void resize(int width, int height);

    static void resize_callback(GLFWwindow* window, int width, int height);
};

#endif
