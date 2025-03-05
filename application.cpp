#include "application.h"
#include "init_wgpu.h"
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
    WGPUQueue queue = wgpuDeviceGetQueue(m_device);
    pre_render(queue);

    WGPUCommandEncoderDescriptor encoder_desc = { 0 };
    WGPUCommandEncoder encoder =
        wgpuDeviceCreateCommandEncoder(m_device, &encoder_desc);

    WGPUSurfaceTexture surface_texture;
    wgpuSurfaceGetCurrentTexture(m_surface, &surface_texture);

    WGPURenderPassColorAttachment color_attachments = { 0 };
    color_attachments.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
    color_attachments.loadOp = WGPULoadOp_Clear;
    color_attachments.storeOp = WGPUStoreOp_Store;
    color_attachments.clearValue = { 1, 1, 0, 1 };
    color_attachments.view = wgpuTextureCreateView(
        surface_texture.texture, nullptr);

    WGPURenderPassDescriptor render_pass_desc = {};
    render_pass_desc.colorAttachmentCount = 1;
    render_pass_desc.colorAttachments = &color_attachments;
    render_pass_desc.depthStencilAttachment = nullptr;

    WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(
        encoder, &render_pass_desc);
    render(pass);
    wgpuRenderPassEncoderEnd(pass);

    WGPUCommandBufferDescriptor buffer_desc = { 0 };
    WGPUCommandBuffer command_buffer =
        wgpuCommandEncoderFinish(encoder, &buffer_desc);
    wgpuQueueSubmit(queue, 1, &command_buffer);

    wgpuSurfacePresent(m_surface);

    wgpuTextureViewRelease(color_attachments.view);
    wgpuRenderPassEncoderRelease(pass);
    wgpuCommandEncoderRelease(encoder);
    wgpuCommandBufferRelease(command_buffer);
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
