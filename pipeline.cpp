#include "pipeline.h"

Pipeline::Pipeline(WGPUDevice device)
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
    layout0_entries[0].buffer.minBindingSize = sizeof(CameraData);

    WGPUBindGroupLayoutDescriptor layout0_desc = { 0 };
    layout0_desc.label = {"CameraLayout", WGPU_STRLEN};
    layout0_desc.entryCount = 1;
    layout0_desc.entries = layout0_entries;

    m_layout0 = wgpuDeviceCreateBindGroupLayout(device, &layout0_desc);

    WGPUBindGroupLayoutEntry layout1_entries[1] = { 0 };
    layout1_entries[0].binding = 0;
    layout1_entries[0].visibility = WGPUShaderStage_Vertex;
    layout1_entries[0].buffer.type = WGPUBufferBindingType_Uniform;
    layout1_entries[0].buffer.minBindingSize = sizeof(ModelData);

    WGPUBindGroupLayoutDescriptor layout1_desc = { 0 };
    layout1_desc.label = {"ModelLayout", WGPU_STRLEN};
    layout1_desc.entryCount = 1;
    layout1_desc.entries = layout0_entries;

    m_layout1 = wgpuDeviceCreateBindGroupLayout(device, &layout1_desc);

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

Pipeline::~Pipeline()
{
    wgpuBindGroupLayoutRelease(m_layout1);
    wgpuBindGroupLayoutRelease(m_layout0);
    wgpuRenderPipelineRelease(m_pipeline);
}

WGPUBindGroup Pipeline::create_camera_group(WGPUBuffer buffer)
{
    WGPUBindGroupEntry entries[1] = { 0 };
    entries[0].binding = 0;
    entries[0].buffer = buffer;
    entries[0].size = sizeof(CameraData);

    WGPUBindGroupDescriptor bind_group_desc = { 0 };
    bind_group_desc.label = {"CameraBindGroup", WGPU_STRLEN};
    bind_group_desc.layout = m_layout0;
    bind_group_desc.entryCount = 1;
    bind_group_desc.entries = entries;

    return wgpuDeviceCreateBindGroup(m_device, &bind_group_desc);
}

WGPUBindGroup Pipeline::create_model_group(WGPUBuffer buffer)
{
    WGPUBindGroupEntry entries[1] = { 0 };
    entries[0].binding = 0;
    entries[0].buffer = buffer;
    entries[0].size = sizeof(ModelData);

    WGPUBindGroupDescriptor bind_group_desc = { 0 };
    bind_group_desc.label = {"ModelBindGroup", WGPU_STRLEN};
    bind_group_desc.layout = m_layout1;
    bind_group_desc.entryCount = 1;
    bind_group_desc.entries = entries;

    return wgpuDeviceCreateBindGroup(m_device, &bind_group_desc);
}
