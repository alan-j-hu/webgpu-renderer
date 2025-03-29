#include "noworry/application.h"
#include "noworry/init_wgpu.h"
#include <webgpu/webgpu_glfw.h>

Application::Application(int width, int height)
{
    m_width = width;
    m_height = height;

    if (!glfwInit()) {
        return;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(
        width, height, "WebGPU window", nullptr, nullptr);
    if (window == nullptr) {
        return;
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, resize_callback);

    WGPUInstance instance = wgpuCreateInstance(nullptr);
    WGPUSurface surface =
        wgpu::glfw::CreateSurfaceForWindow(instance, window)
        .MoveToCHandle();

    WGPUAdapter adapter = request_adapter(instance, surface);
    WGPUDevice device = request_device(instance, adapter);

    m_window = window;
    m_instance = instance;
    m_adapter = adapter;
    m_device = device;
    m_surface = surface;

    resize(width, height);
}

Application::~Application()
{
    wgpuDeviceRelease(m_device);
    wgpuAdapterRelease(m_adapter);
    wgpuInstanceRelease(m_instance);

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

int Application::run()
{
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
        tick();
    }
    return 0;
}

void Application::tick()
{
    WGPUSurfaceTexture surface_texture;
    wgpuSurfaceGetCurrentTexture(m_surface, &surface_texture);
    WGPUTextureView view = wgpuTextureCreateView(
        surface_texture.texture, nullptr);
    render(view);
    wgpuSurfacePresent(m_surface);
    wgpuTextureViewRelease(view);
}

void Application::resize(int width, int height)
{
    m_width = width;
    m_height = height;

    WGPUSurfaceConfiguration config = {};

    config.width = width;
    config.height = height;
    config.usage = WGPUTextureUsage_RenderAttachment;
    config.format = WGPUTextureFormat_BGRA8Unorm;

    config.viewFormatCount = 0;
    config.viewFormats = nullptr;
    config.device = m_device;
    config.presentMode = WGPUPresentMode_Fifo;
    config.alphaMode = WGPUCompositeAlphaMode_Auto;

    wgpuSurfaceConfigure(m_surface, &config);
}

void Application::resize_callback(GLFWwindow* window, int width, int height)
{
    auto app =
        reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->resize(width, height);
}
