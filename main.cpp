#include "init_wgpu.h"

#include <iostream>
#include <GLFW/glfw3.h>
#include <webgpu/webgpu.h>
#include <webgpu/webgpu_glfw.h>

class Application
{
public:
    Application(int width, int height)
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

    virtual ~Application()
    {
        wgpuDeviceRelease(m_device);
        wgpuAdapterRelease(m_adapter);
        wgpuInstanceRelease(m_instance);

        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    int run()
    {
        while (!glfwWindowShouldClose(m_window)) {
            glfwPollEvents();
            tick();
        }
        return 0;
    }

    void tick()
    {
        WGPUQueue queue = wgpuDeviceGetQueue(device());

        WGPUCommandEncoderDescriptor encoder_desc = { 0 };
        WGPUCommandEncoder encoder =
            wgpuDeviceCreateCommandEncoder(device(), &encoder_desc);

        WGPUSurfaceTexture surface_texture;
        wgpuSurfaceGetCurrentTexture(surface(), &surface_texture);

        WGPURenderPassColorAttachment color_attachments = { 0 };
        color_attachments.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
        color_attachments.loadOp = WGPULoadOp_Clear;
        color_attachments.storeOp = WGPUStoreOp_Store;
        color_attachments.clearValue = { 0, 0, 0, 1 };
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

        wgpuSurfacePresent(surface());

        wgpuTextureViewRelease(color_attachments.view);
        wgpuRenderPassEncoderRelease(pass);
        wgpuCommandEncoderRelease(encoder);
        wgpuCommandBufferRelease(command_buffer);
    }

protected:
    GLFWwindow* window()
    {
        return m_window;
    }

    WGPUDevice device()
    {
        return m_device;
    }

    WGPUSurface surface()
    {
        return m_surface;
    }

    virtual void render(WGPURenderPassEncoder&) = 0;

private:
    int m_width;
    int m_height;
    GLFWwindow* m_window;
    WGPUInstance m_instance;
    WGPUAdapter m_adapter;
    WGPUDevice m_device;
    WGPUSurface m_surface;

    void resize(int width, int height)
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

    static void resize_callback(GLFWwindow* window, int width, int height)
    {
        auto app =
            reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
        app->resize(width, height);
    }
};

struct Vertex
{
    float pos[3];
    float color[4];
};

struct Camera
{
    float viewproj[4][4];
};

struct Model
{
    float matrix[4][4];
};

class Main : public Application
{
public:
    Main(int width, int height) : Application(width, height)
    {
        const char* code = R"(
struct Camera {
  viewproj: mat4x4<f32>,
};
struct Model {
  transform: mat4x4<f32>,
};
@group(0) @binding(0) var<uniform> camera: Camera;
@group(1) @binding(0) var<uniform> model: Model;

struct Vertex {
  @location(0) pos: vec3f,
  @location(1) color: vec4f,
}

struct FragmentInput {
  @builtin(position) pos: vec4f,
  @location(0) color: vec4f,
};

@vertex
fn vs_main(vertex: Vertex) -> FragmentInput {
  var out: FragmentInput;
  out.pos = camera.viewproj * model.transform * vec4(vertex.pos, 1);
  out.color = vertex.color;
  return out;
}

@fragment
fn fs_main(input: FragmentInput) -> @location(0) vec4f {
  return input.color;
}
)";

        WGPUShaderModuleWGSLDescriptor vertex_wgsl_desc = { 0 };
        vertex_wgsl_desc.chain.sType = WGPUSType_ShaderSourceWGSL;
        vertex_wgsl_desc.code = {code, WGPU_STRLEN};

        WGPUShaderModuleDescriptor vertex_desc = { 0 };
        vertex_desc.nextInChain = &vertex_wgsl_desc.chain;
        WGPUShaderModule vertex_shader =
            wgpuDeviceCreateShaderModule(device(), &vertex_desc);

        WGPUShaderModuleWGSLDescriptor frag_wgsl_desc = { 0 };
        frag_wgsl_desc.chain.sType = WGPUSType_ShaderSourceWGSL;
        frag_wgsl_desc.code = {code, WGPU_STRLEN};

        WGPUShaderModuleDescriptor frag_desc = { 0 };
        frag_desc.nextInChain = &frag_wgsl_desc.chain;
        WGPUShaderModule fragment_shader =
            wgpuDeviceCreateShaderModule(device(), &frag_desc);

        WGPUVertexAttribute vertex_attrs[2];
        vertex_attrs[0].shaderLocation = 0;
        vertex_attrs[0].format = WGPUVertexFormat_Float32x3;
        vertex_attrs[0].offset = 0;
        vertex_attrs[1].shaderLocation = 1;
        vertex_attrs[1].format = WGPUVertexFormat_Float32x3;
        vertex_attrs[1].offset = offsetof(Vertex, color);

        WGPUVertexBufferLayout vertex_layout = { 0 };
        vertex_layout.stepMode = WGPUVertexStepMode_Vertex;
        vertex_layout.arrayStride = sizeof(Vertex);
        vertex_layout.attributeCount = 2;
        vertex_layout.attributes = vertex_attrs;

        WGPUBindGroupLayoutEntry layout0_entries[1] = { 0 };
        layout0_entries[0].binding = 0;
        layout0_entries[0].visibility = WGPUShaderStage_Vertex;
        layout0_entries[0].buffer.type = WGPUBufferBindingType_Uniform;
        layout0_entries[0].buffer.minBindingSize = sizeof(Camera);

        WGPUBindGroupLayoutDescriptor layout0_desc = { 0 };
        layout0_desc.entryCount = 1;
        layout0_desc.entries = layout0_entries;

        WGPUBindGroupLayout layout0 =
            wgpuDeviceCreateBindGroupLayout(device(), &layout0_desc);

        WGPUBindGroupLayoutEntry layout1_entries[1] = { 0 };
        layout1_entries[0].binding = 0;
        layout1_entries[0].visibility = WGPUShaderStage_Vertex;
        layout1_entries[0].buffer.type = WGPUBufferBindingType_Uniform;
        layout1_entries[0].buffer.minBindingSize = sizeof(Model);

        WGPUBindGroupLayoutDescriptor layout1_desc = { 0 };
        layout1_desc.entryCount = 1;
        layout1_desc.entries = layout0_entries;

        WGPUBindGroupLayout layout1 =
            wgpuDeviceCreateBindGroupLayout(device(), &layout1_desc);

        WGPUBindGroupLayout layouts[2] = { layout0, layout1 };

        WGPUPipelineLayoutDescriptor pipeline_layout_desc = { 0 };
        pipeline_layout_desc.bindGroupLayoutCount = 2;
        pipeline_layout_desc.bindGroupLayouts = layouts;

        WGPUPipelineLayout pipeline_layout =
            wgpuDeviceCreatePipelineLayout(device(), &pipeline_layout_desc);

        WGPUBlendState blend = {
            .color= {
                .operation = WGPUBlendOperation_Add,
                .srcFactor = WGPUBlendFactor_Src,
                .dstFactor = WGPUBlendFactor_Zero
            },
            .alpha = {
                .operation = WGPUBlendOperation_Add,
                .srcFactor = WGPUBlendFactor_Src,
                .dstFactor = WGPUBlendFactor_Zero
            }
        };

        WGPUColorTargetState color_target = {
            .format = WGPUTextureFormat_BGRA8Unorm,
            .blend = &blend,
            .writeMask = WGPUColorWriteMask_All
        };

        WGPUFragmentState fragment = { 0 };
        fragment.module = fragment_shader;
        fragment.entryPoint = {"fs_main", WGPU_STRLEN};
        fragment.targetCount = 1;
        fragment.targets = &color_target;

        WGPURenderPipelineDescriptor pipeline_desc = { 0 };
        pipeline_desc.layout = pipeline_layout;
        pipeline_desc.vertex.module = vertex_shader;
        pipeline_desc.vertex.entryPoint = {"vs_main", WGPU_STRLEN};
        pipeline_desc.vertex.bufferCount = 1;
        pipeline_desc.vertex.buffers = &vertex_layout;
        pipeline_desc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
        pipeline_desc.primitive.frontFace = WGPUFrontFace_CCW;
        pipeline_desc.primitive.cullMode = WGPUCullMode_None;
        pipeline_desc.multisample.count = 1;
        pipeline_desc.multisample.count = 1;
        pipeline_desc.multisample.mask = ~0u;
        pipeline_desc.fragment = &fragment;

        WGPURenderPipeline pipeline =
            wgpuDeviceCreateRenderPipeline(device(), &pipeline_desc);

        wgpuPipelineLayoutRelease(pipeline_layout);
        wgpuBindGroupLayoutRelease(layout1);
        wgpuBindGroupLayoutRelease(layout0);
        wgpuShaderModuleRelease(fragment_shader);
        wgpuShaderModuleRelease(vertex_shader);
    }

    virtual void render(WGPURenderPassEncoder& encoder) override
    {
    }
};

int main()
{
    Main app(500, 500);
    app.run();

    return 0;
}
