#include "noworry/material/mesheffect.h"
#include "noworry/material/material.h"
#include "noworry/material/uniformlayout.h"
#include "noworry/scene/model.h"
#include "noworry/mesh.h"

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
  x: f32,
  y: f32,
  z: f32,
  u: f32,
  v: f32,
};

@group(0) @binding(0) var<uniform> camera: Camera;

@group(2) @binding(0) var<uniform> model: Model;
@group(2) @binding(1) var<storage, read> vertices: array<Vertex>;

struct VertexInput {
  @builtin(vertex_index) vertexID : u32,
}

struct FragmentInput {
  @builtin(position) pos: vec4f,
  @location(0) tex_coords: vec2f,
};

@vertex
fn vs_main(vertex: VertexInput) -> FragmentInput {
  var v = vertices[vertex.vertexID];

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
    model_layout_entries[1].binding = 1;
    model_layout_entries[1].visibility = WGPUShaderStage_Vertex;
    model_layout_entries[1].buffer.type =
        WGPUBufferBindingType_ReadOnlyStorage;
    model_layout_entries[1].buffer.minBindingSize = 0;

    WGPUBindGroupLayoutDescriptor model_layout_desc = { 0 };
    model_layout_desc.label = {"ModelLayout", WGPU_STRLEN};
    model_layout_desc.entryCount = 2;
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
    WGPUBuffer transform,
    WGPUBuffer vertices,
    int vertex_count)
{
    WGPUBindGroupEntry entries[2] = { 0 };
    entries[0].binding = 0;
    entries[0].buffer = transform;
    entries[0].size = sizeof(ModelData);
    entries[1].binding = 1;
    entries[1].buffer = vertices;
    entries[1].size = vertex_count * sizeof(Vertex);

    WGPUBindGroupDescriptor bind_group_desc = { 0 };
    bind_group_desc.label = {"ModelBindGroup", WGPU_STRLEN};
    bind_group_desc.layout = model_layout();
    bind_group_desc.entryCount = 2;
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
    pipeline_desc.vertex.bufferCount = 0;
    pipeline_desc.vertex.buffers = nullptr;
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

void MeshEffect::enqueue(Model& model)
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
