#include "init_wgpu.h"

#include <array>
#include <cstring>
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

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

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
        WGPUQueue queue = wgpuDeviceGetQueue(m_device);

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
    float x, y, z;
    float _padding;
    float r, g, b, a;
};

struct Camera
{
    float viewproj[4][4];
};

struct Model
{
    float transform[4][4];
};

class Pipeline
{
public:
    Pipeline(WGPUDevice device)
        : m_device(device)
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
  out.pos = vec4(vertex.pos, 1);
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
            wgpuDeviceCreateShaderModule(device, &vertex_desc);

        WGPUShaderModuleWGSLDescriptor frag_wgsl_desc = { 0 };
        frag_wgsl_desc.chain.sType = WGPUSType_ShaderSourceWGSL;
        frag_wgsl_desc.code = {code, WGPU_STRLEN};

        WGPUShaderModuleDescriptor frag_desc = { 0 };
        frag_desc.nextInChain = &frag_wgsl_desc.chain;
        WGPUShaderModule fragment_shader =
            wgpuDeviceCreateShaderModule(device, &frag_desc);

        WGPUVertexAttribute vertex_attrs[2];
        vertex_attrs[0].shaderLocation = 0;
        vertex_attrs[0].format = WGPUVertexFormat_Float32x3;
        vertex_attrs[0].offset = 0;
        vertex_attrs[1].shaderLocation = 1;
        vertex_attrs[1].format = WGPUVertexFormat_Float32x3;
        vertex_attrs[1].offset = offsetof(Vertex, r);

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
        layout0_desc.label = {"CameraLayout", WGPU_STRLEN};
        layout0_desc.entryCount = 1;
        layout0_desc.entries = layout0_entries;

        m_layout0 =
            wgpuDeviceCreateBindGroupLayout(device, &layout0_desc);

        WGPUBindGroupLayoutEntry layout1_entries[1] = { 0 };
        layout1_entries[0].binding = 0;
        layout1_entries[0].visibility = WGPUShaderStage_Vertex;
        layout1_entries[0].buffer.type = WGPUBufferBindingType_Uniform;
        layout1_entries[0].buffer.minBindingSize = sizeof(Model);

        WGPUBindGroupLayoutDescriptor layout1_desc = { 0 };
        layout1_desc.label = {"ModelLayout", WGPU_STRLEN};
        layout1_desc.entryCount = 1;
        layout1_desc.entries = layout0_entries;

        m_layout1 =
            wgpuDeviceCreateBindGroupLayout(device, &layout1_desc);

        WGPUBindGroupLayout layouts[2] = { m_layout0, m_layout1 };

        WGPUPipelineLayoutDescriptor pipeline_layout_desc = { 0 };
        pipeline_layout_desc.bindGroupLayoutCount = 2;
        pipeline_layout_desc.bindGroupLayouts = layouts;

        WGPUPipelineLayout pipeline_layout =
            wgpuDeviceCreatePipelineLayout(device, &pipeline_layout_desc);

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
        pipeline_desc.label = {"pipeline", WGPU_STRLEN};
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

        m_pipeline = wgpuDeviceCreateRenderPipeline(device, &pipeline_desc);

        wgpuPipelineLayoutRelease(pipeline_layout);
        wgpuShaderModuleRelease(fragment_shader);
        wgpuShaderModuleRelease(vertex_shader);
    }

    ~Pipeline()
    {
        //wgpuBindGroupLayoutRelease(m_layout1);
        //wgpuBindGroupLayoutRelease(m_layout0);
        //wgpuRenderPipelineRelease(m_pipeline);
    }

    WGPURenderPipeline pipeline() { return m_pipeline; }

    WGPUBindGroupLayout camera_layout() { return m_layout0; }
    WGPUBindGroupLayout model_layout() { return m_layout1; }

    WGPUBindGroup create_camera_group(WGPUBuffer buffer)
    {
        WGPUBindGroupEntry entries[1] = { 0 };
        entries[0].binding = 0;
        entries[0].buffer = buffer;
        entries[0].size = sizeof(Camera);

        WGPUBindGroupDescriptor bind_group_desc = { 0 };
        bind_group_desc.label = {"CameraBindGroup", WGPU_STRLEN};
        bind_group_desc.layout = m_layout0;
        bind_group_desc.entryCount = 1;
        bind_group_desc.entries = entries;

        return wgpuDeviceCreateBindGroup(m_device, &bind_group_desc);
    }

    WGPUBindGroup create_model_group(WGPUBuffer buffer)
    {
        WGPUBindGroupEntry entries[1] = { 0 };
        entries[0].binding = 0;
        entries[0].buffer = buffer;
        entries[0].size = sizeof(Model);

        WGPUBindGroupDescriptor bind_group_desc = { 0 };
        bind_group_desc.label = {"ModelBindGroup", WGPU_STRLEN};
        bind_group_desc.layout = m_layout1;
        bind_group_desc.entryCount = 1;
        bind_group_desc.entries = entries;

        return wgpuDeviceCreateBindGroup(m_device, &bind_group_desc);
    }

private:
    WGPUDevice m_device;
    WGPURenderPipeline m_pipeline;
    WGPUBindGroupLayout m_layout0;
    WGPUBindGroupLayout m_layout1;
};

class CameraBinding
{
public:
    CameraBinding(WGPUDevice device, Pipeline pipeline)
    {
        std::memset(m_camera.viewproj, 0, sizeof(float[4][4]));
        m_camera.viewproj[0][0] = 1;
        m_camera.viewproj[1][1] = 1;
        m_camera.viewproj[2][2] = 1;
        m_camera.viewproj[3][3] = 1;

        WGPUBufferDescriptor buffer_desc = { 0 };
        buffer_desc.nextInChain = nullptr;
        buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
        buffer_desc.size = sizeof(Camera);
        buffer_desc.mappedAtCreation = false;

        m_buffer =
            wgpuDeviceCreateBuffer(device, &buffer_desc);

        m_bind_group = pipeline.create_camera_group(m_buffer);
    }

    void copy_to_gpu(WGPUDevice device)
    {
        WGPUQueue queue = wgpuDeviceGetQueue(device);
        wgpuQueueWriteBuffer(
            queue, m_buffer, 0, &m_camera.viewproj, sizeof(float[4][4]));
    }

    Camera m_camera;
    WGPUBindGroup m_bind_group;
    WGPUBuffer m_buffer;
};

class ModelBinding
{
public:
    ModelBinding(WGPUDevice device, Pipeline pipeline)
    {
        std::memset(m_model.transform, 0, sizeof(float[4][4]));
        m_model.transform[0][0] = 1;
        m_model.transform[1][1] = 1;
        m_model.transform[2][2] = 1;
        m_model.transform[3][3] = 1;

        WGPUBufferDescriptor buffer_desc = { 0 };
        buffer_desc.nextInChain = nullptr;
        buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
        buffer_desc.size = sizeof(Model);
        buffer_desc.mappedAtCreation = false;

        m_buffer =
            wgpuDeviceCreateBuffer(device, &buffer_desc);

        m_bind_group = pipeline.create_model_group(m_buffer);
    }

    void copy_to_gpu(WGPUDevice device)
    {
        WGPUQueue queue = wgpuDeviceGetQueue(device);
        wgpuQueueWriteBuffer(
            queue, m_buffer, 0, &m_model.transform, sizeof(float[4][4]));
    }

    Model m_model;
    WGPUBindGroup m_bind_group;
    WGPUBuffer m_buffer;
};

class Main : public Application
{
public:
    Main(int width, int height)
        : Application(width, height),
          m_pipeline(device()),
          m_camera(device(), m_pipeline),
          m_model(device(), m_pipeline)
    {
        std::array<Vertex, 3> vertices;
        vertices[0].x = 0;
        vertices[0].y = 0;
        vertices[0].z = 0;
        vertices[0]._padding = 0;
        vertices[0].r = 1;
        vertices[0].g = 0;
        vertices[0].b = 0;
        vertices[0].a = 1;

        vertices[1].x = 1;
        vertices[1].y = 1;
        vertices[1].z = 0;
        vertices[1]._padding = 0;
        vertices[1].r = 0;
        vertices[1].g = 1;
        vertices[1].b = 0;
        vertices[1].a = 1;

        vertices[2].x = 0;
        vertices[2].y = 1;
        vertices[2].z = 0;
        vertices[2]._padding = 0;
        vertices[2].r = 0;
        vertices[2].g = 0;
        vertices[2].b = 1;
        vertices[2].a = 1;
        create_vertices(vertices.data(), 3);
    }

    void create_vertices(Vertex* vertices, std::size_t count)
    {
        WGPUBufferDescriptor buffer_desc = { 0 };
        buffer_desc.nextInChain = nullptr;
        buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
        buffer_desc.size = count * sizeof(Vertex);
        buffer_desc.mappedAtCreation = false;

        m_vertex_buffer =
            wgpuDeviceCreateBuffer(device(), &buffer_desc);

        WGPUQueue queue = wgpuDeviceGetQueue(device());
        wgpuQueueWriteBuffer(
            queue, m_vertex_buffer, 0, vertices, sizeof(Vertex) * count);
    }


    virtual void pre_render(WGPUQueue queue)
    {
        m_camera.copy_to_gpu(device());
        m_model.copy_to_gpu(device());
    }

    virtual void render(WGPURenderPassEncoder encoder) override
    {
        wgpuRenderPassEncoderSetBindGroup(encoder, 0, m_camera.m_bind_group, 0, nullptr);
        wgpuRenderPassEncoderSetBindGroup(encoder, 1, m_model.m_bind_group, 0, nullptr);
        wgpuRenderPassEncoderSetPipeline(encoder, m_pipeline.pipeline());
        wgpuRenderPassEncoderSetVertexBuffer(encoder, 0, m_vertex_buffer, 0, sizeof(Vertex) * 3);
        wgpuRenderPassEncoderDraw(encoder, 3, 1, 0, 0);
    }

    ~Main()
    {
        //wgpuBufferRelease(m_vertex_buffer);
    }
private:
    Pipeline m_pipeline;
    CameraBinding m_camera;
    ModelBinding m_model;
    WGPUBuffer m_vertex_buffer;
};

int main()
{
    Main app(500, 500);
    app.run();

    return 0;
}
