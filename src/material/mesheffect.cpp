#include "noworry/material/mesheffect.h"
#include "noworry/material/material.h"
#include "noworry/material/uniformlayout.h"
#include "noworry/scene/renderobject.h"
#include "noworry/mesh.h"

const std::array<WGPUVertexAttribute, 5>& Vertex::attributes()
{
    static std::array<WGPUVertexAttribute, 5> attributes;
    static bool initialized = false;

    if (!initialized) {
        attributes[0].shaderLocation = 0;
        attributes[0].format = WGPUVertexFormat_Float32;
        attributes[0].offset = offsetof(Vertex, x);

        attributes[1].shaderLocation = 1;
        attributes[1].format = WGPUVertexFormat_Float32;
        attributes[1].offset = offsetof(Vertex, y);

        attributes[2].shaderLocation = 2;
        attributes[2].format = WGPUVertexFormat_Float32;
        attributes[2].offset = offsetof(Vertex, z);

        attributes[3].shaderLocation = 3;
        attributes[3].format = WGPUVertexFormat_Float32;
        attributes[3].offset = offsetof(Vertex, u);

        attributes[4].shaderLocation = 4;
        attributes[4].format = WGPUVertexFormat_Float32;
        attributes[4].offset = offsetof(Vertex, v);

        initialized = true;
    }

    return attributes;
}

MeshEffect::MeshEffect(WGPUDevice device, UniformLayout& ul)
    : m_pipeline(nullptr), m_pipeline_layout(nullptr),
      m_device(device), m_ul(&ul)
{
    const char* code = R"(
struct Camera {
  viewproj: mat4x4<f32>,
};

struct Model {
  transform: mat4x4<f32>,
};

struct Vertex {
  @location(0) x: f32,
  @location(1) y: f32,
  @location(2) z: f32,
  @location(3) u: f32,
  @location(4) v: f32,
};

@group(0) @binding(0) var<uniform> camera: Camera;

@group(2) @binding(0) var<uniform> model: Model;

struct FragmentInput {
  @builtin(position) pos: vec4f,
  @location(0) tex_coords: vec2f,
};

@vertex
fn vs_main(v: Vertex) -> FragmentInput {
  var out: FragmentInput;
  out.pos = camera.viewproj * model.transform * vec4(v.x, v.y, v.z, 1);
  out.tex_coords = vec2(v.u, v.v);
  return out;
}
)";

    WGPUShaderModuleWGSLDescriptor vertex_wgsl_desc = { 0 };
    vertex_wgsl_desc.chain.sType = WGPUSType_ShaderSourceWGSL;
    vertex_wgsl_desc.code = {code, WGPU_STRLEN};

    WGPUShaderModuleDescriptor vertex_desc = { 0 };
    vertex_desc.nextInChain = &vertex_wgsl_desc.chain;
    m_vertex_shader =
        wgpuDeviceCreateShaderModule(device, &vertex_desc);

    WGPUBindGroupLayoutEntry model_layout_entries[2] = { 0 };
    model_layout_entries[0].binding = 0;
    model_layout_entries[0].visibility = WGPUShaderStage_Vertex;
    model_layout_entries[0].buffer.type = WGPUBufferBindingType_Uniform;
    model_layout_entries[0].buffer.minBindingSize = sizeof(ModelData);

    WGPUBindGroupLayoutDescriptor model_layout_desc = { 0 };
    model_layout_desc.label = {"ModelLayout", WGPU_STRLEN};
    model_layout_desc.entryCount = 1;
    model_layout_desc.entries = model_layout_entries;

    m_model_layout =
        wgpuDeviceCreateBindGroupLayout(device, &model_layout_desc);
}

MeshEffect::~MeshEffect()
{
    if (m_pipeline != nullptr) {
        wgpuRenderPipelineRelease(m_pipeline);
    }
    if (m_pipeline_layout != nullptr) {
        wgpuPipelineLayoutRelease(m_pipeline_layout);
    }
    wgpuBindGroupLayoutRelease(m_model_layout);
    wgpuShaderModuleRelease(m_vertex_shader);
}

WGPUBindGroup MeshEffect::create_model_group(
    WGPUDevice device,
    WGPUBuffer transform)
{
    WGPUBindGroupEntry entries[1] = { 0 };
    entries[0].binding = 0;
    entries[0].buffer = transform;
    entries[0].size = sizeof(ModelData);

    WGPUBindGroupDescriptor bind_group_desc = { 0 };
    bind_group_desc.label = {"ModelBindGroup", WGPU_STRLEN};
    bind_group_desc.layout = model_layout();
    bind_group_desc.entryCount = 1;
    bind_group_desc.entries = entries;

    return wgpuDeviceCreateBindGroup(device, &bind_group_desc);
}

WGPUPipelineLayout MeshEffect::pipeline_layout()
{
    // Pipeline layout cannot be created in the constructor because it
    // requires a virtual call to get the material layout.
    if (m_pipeline_layout != nullptr) {
        return m_pipeline_layout;
    }

    WGPUBindGroupLayout layouts[3] = {
        m_ul->layout(),
        material_layout(),
        model_layout()
    };

    WGPUPipelineLayoutDescriptor pipeline_layout_desc = { 0 };
    pipeline_layout_desc.bindGroupLayoutCount = 3;
    pipeline_layout_desc.bindGroupLayouts = layouts;

    m_pipeline_layout =
        wgpuDeviceCreatePipelineLayout(m_device, &pipeline_layout_desc);
    return m_pipeline_layout;
}

WGPURenderPipeline MeshEffect::pipeline()
{
    if (m_pipeline != nullptr) {
        return m_pipeline;
    }

    WGPUVertexBufferLayout vertex_buffer_layout;
    vertex_buffer_layout.attributeCount = Vertex::attributes().size();
    vertex_buffer_layout.attributes = Vertex::attributes().data();
    vertex_buffer_layout.arrayStride = sizeof(Vertex);
    vertex_buffer_layout.stepMode = WGPUVertexStepMode_Vertex;

    WGPUDepthStencilState depth_stencil = { 0 };
    depth_stencil.format = WGPUTextureFormat_Depth24Plus;
    depth_stencil.depthWriteEnabled = WGPUOptionalBool_True;
    depth_stencil.depthCompare = WGPUCompareFunction_Less;

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
    fragment.module = fragment_shader();
    fragment.entryPoint = {"fs_main", WGPU_STRLEN};
    fragment.targetCount = 1;
    fragment.targets = &color_target;

    WGPURenderPipelineDescriptor pipeline_desc = { 0 };
    pipeline_desc.label = {"pipeline", WGPU_STRLEN};
    pipeline_desc.layout = pipeline_layout();
    pipeline_desc.vertex.module = vertex_shader();
    pipeline_desc.vertex.entryPoint = {"vs_main", WGPU_STRLEN};
    pipeline_desc.vertex.bufferCount = 1;
    pipeline_desc.vertex.buffers = &vertex_buffer_layout;
    pipeline_desc.primitive.topology = topology();
    pipeline_desc.primitive.frontFace = WGPUFrontFace_CCW;
    pipeline_desc.primitive.cullMode = WGPUCullMode_None;
    pipeline_desc.depthStencil = &depth_stencil;
    pipeline_desc.multisample.count = 1;
    pipeline_desc.multisample.count = 1;
    pipeline_desc.multisample.mask = ~0u;
    pipeline_desc.fragment = &fragment;

    m_pipeline = wgpuDeviceCreateRenderPipeline(m_device, &pipeline_desc);
    return m_pipeline;
}

void MeshEffect::enqueue(RenderObject& model)
{
    m_queue.push_back(&model);
}

void MeshEffect::draw(WGPURenderPassEncoder encoder)
{
    wgpuRenderPassEncoderSetPipeline(encoder, pipeline());
    for (auto model : m_queue) {
        int count = (model->mesh().index_count() / 3) * 3;
        wgpuRenderPassEncoderSetBindGroup(
            encoder, 1, model->material().bind_group(), 0, nullptr);
        wgpuRenderPassEncoderSetBindGroup(
            encoder, 2, model->bind_group(), 0, nullptr);

        wgpuRenderPassEncoderSetVertexBuffer(
            encoder,
            0,
            model->mesh().vertex_buffer(),
            0,
            wgpuBufferGetSize(model->mesh().vertex_buffer()));

        wgpuRenderPassEncoderSetIndexBuffer(
            encoder,
            model->mesh().index_buffer(),
            WGPUIndexFormat_Uint16,
            0,
            count * sizeof(std::uint16_t));

        wgpuRenderPassEncoderDrawIndexed(encoder, count, 1, 0, 0, 0);
    }
    m_queue.clear();
}
