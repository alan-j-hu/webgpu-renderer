#include "noworry/pipeline.h"

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

struct Vertex {
  x: f32,
  y: f32,
  z: f32,
  u: f32,
  v: f32,
};

@group(0) @binding(0) var<uniform> camera: Camera;

@group(1) @binding(0) var the_texture: texture_2d<f32>;
@group(1) @binding(1) var the_sampler: sampler;

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

@fragment
fn fs_main(input: FragmentInput) -> @location(0) vec4f {
  return textureSample(the_texture, the_sampler, input.tex_coords);
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

    WGPUBindGroupLayoutEntry camera_layout_entries[1] = { 0 };
    camera_layout_entries[0].binding = 0;
    camera_layout_entries[0].visibility = WGPUShaderStage_Vertex;
    camera_layout_entries[0].buffer.type = WGPUBufferBindingType_Uniform;
    camera_layout_entries[0].buffer.minBindingSize = sizeof(CameraData);

    WGPUBindGroupLayoutDescriptor camera_layout_desc = { 0 };
    camera_layout_desc.label = {"CameraLayout", WGPU_STRLEN};
    camera_layout_desc.entryCount = 1;
    camera_layout_desc.entries = camera_layout_entries;

    m_camera_layout =
        wgpuDeviceCreateBindGroupLayout(device, &camera_layout_desc);

    WGPUTextureBindingLayout texture_layout = { 0 };
    texture_layout.sampleType = WGPUTextureSampleType_Float;
    texture_layout.viewDimension = WGPUTextureViewDimension_2D;
    texture_layout.multisampled = 0;

    WGPUSamplerBindingLayout sampler_layout = { 0 };
    sampler_layout.type = WGPUSamplerBindingType_Filtering;

    WGPUBindGroupLayoutEntry material_layout_entries[2] = { 0 };
    material_layout_entries[0].binding = 0;
    material_layout_entries[0].visibility = WGPUShaderStage_Fragment;
    material_layout_entries[0].texture = texture_layout;
    material_layout_entries[1].binding = 1;
    material_layout_entries[1].visibility = WGPUShaderStage_Fragment;
    material_layout_entries[1].sampler = sampler_layout;

    WGPUBindGroupLayoutDescriptor material_layout_desc = { 0 };
    material_layout_desc.label = {"MaterialLayout", WGPU_STRLEN};
    material_layout_desc.entryCount = 2;
    material_layout_desc.entries = material_layout_entries;

    m_material_layout =
        wgpuDeviceCreateBindGroupLayout(device, &material_layout_desc);

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

    WGPUBindGroupLayout layouts[3] = {
        m_camera_layout,
        m_material_layout,
        m_model_layout
    };

    WGPUPipelineLayoutDescriptor pipeline_layout_desc = { 0 };
    pipeline_layout_desc.bindGroupLayoutCount = 3;
    pipeline_layout_desc.bindGroupLayouts = layouts;

    WGPUPipelineLayout pipeline_layout =
        wgpuDeviceCreatePipelineLayout(device, &pipeline_layout_desc);

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
    fragment.module = fragment_shader;
    fragment.entryPoint = {"fs_main", WGPU_STRLEN};
    fragment.targetCount = 1;
    fragment.targets = &color_target;

    WGPURenderPipelineDescriptor pipeline_desc = { 0 };
    pipeline_desc.label = {"pipeline", WGPU_STRLEN};
    pipeline_desc.layout = pipeline_layout;
    pipeline_desc.vertex.module = vertex_shader;
    pipeline_desc.vertex.entryPoint = {"vs_main", WGPU_STRLEN};
    pipeline_desc.vertex.bufferCount = 0;
    pipeline_desc.vertex.buffers = nullptr;
    pipeline_desc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    pipeline_desc.primitive.frontFace = WGPUFrontFace_CCW;
    pipeline_desc.primitive.cullMode = WGPUCullMode_None;
    pipeline_desc.depthStencil = &depth_stencil;
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
    wgpuBindGroupLayoutRelease(m_model_layout);
    wgpuBindGroupLayoutRelease(m_camera_layout);
    wgpuRenderPipelineRelease(m_pipeline);
}

WGPUBindGroup Pipeline::create_camera_group(WGPUBuffer buffer) const
{
    WGPUBindGroupEntry entries[1] = { 0 };
    entries[0].binding = 0;
    entries[0].buffer = buffer;
    entries[0].size = sizeof(CameraData);

    WGPUBindGroupDescriptor bind_group_desc = { 0 };
    bind_group_desc.label = {"CameraBindGroup", WGPU_STRLEN};
    bind_group_desc.layout = m_camera_layout;
    bind_group_desc.entryCount = 1;
    bind_group_desc.entries = entries;

    return wgpuDeviceCreateBindGroup(m_device, &bind_group_desc);
}

WGPUBindGroup Pipeline::create_material_group(
    WGPUTextureView texture, WGPUSampler sampler) const
{
    WGPUBindGroupEntry entries[2] = { 0 };
    entries[0].binding = 0;
    entries[0].textureView = texture;
    entries[1].binding = 1;
    entries[1].sampler = sampler;

    WGPUBindGroupDescriptor bind_group_desc = { 0 };
    bind_group_desc.label = {"MaterialBindGroup", WGPU_STRLEN};
    bind_group_desc.layout = m_material_layout;
    bind_group_desc.entryCount = 2;
    bind_group_desc.entries = entries;

    return wgpuDeviceCreateBindGroup(m_device, &bind_group_desc);
}

WGPUBindGroup Pipeline::create_model_group(
    WGPUBuffer transform, WGPUBuffer vertices, int tri_count) const
{
    WGPUBindGroupEntry entries[2] = { 0 };
    entries[0].binding = 0;
    entries[0].buffer = transform;
    entries[0].size = sizeof(ModelData);
    entries[1].binding = 1;
    entries[1].buffer = vertices;
    entries[1].size = 3 * tri_count * sizeof(Vertex);

    WGPUBindGroupDescriptor bind_group_desc = { 0 };
    bind_group_desc.label = {"ModelBindGroup", WGPU_STRLEN};
    bind_group_desc.layout = m_model_layout;
    bind_group_desc.entryCount = 2;
    bind_group_desc.entries = entries;

    return wgpuDeviceCreateBindGroup(m_device, &bind_group_desc);
}
