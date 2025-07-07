#include "noworry/Draw2D/SpritePipeline.h"
#include <array>
#include <utility>
#include <glm/mat4x4.hpp>

SpritePipeline::SpritePipeline(WGPUDevice device)
{
    const char* code = R"(
@group(0) @binding(0) var<uniform> viewproj: mat4x4f;

@group(1) @binding(0) var the_texture: texture_2d<f32>;
@group(1) @binding(1) var the_sampler: sampler;

struct Vertex {
  @location(0) x: f32,
  @location(1) y: f32,
  @location(2) u: f32,
  @location(3) v: f32,
};

struct FragmentInput {
  @builtin(position) pos: vec4f,
  @location(0) tex_coords: vec2f,
};

@vertex
fn vs_main(v: Vertex) -> FragmentInput {
  var out: FragmentInput;
  out.pos = viewproj * vec4(v.x, v.y, 0, 1);
  out.tex_coords = vec2(v.u, v.v);
  return out;
}

@fragment
fn fs_main(input: FragmentInput) -> @location(0) vec4f {
  return textureSample(the_texture, the_sampler, input.tex_coords);
}
)";

    // Shader
    WGPUShaderModuleWGSLDescriptor wgsl_desc = { 0 };
    wgsl_desc.chain.sType = WGPUSType_ShaderSourceWGSL;
    wgsl_desc.code = {code, WGPU_STRLEN};

    WGPUShaderModuleDescriptor shader_desc = { 0 };
    shader_desc.nextInChain = &wgsl_desc.chain;
    WGPUShaderModule shader =
        wgpuDeviceCreateShaderModule(device, &shader_desc);

    // Bind group layout
    WGPUBufferBindingLayout buffer_layout = { 0 };
    buffer_layout.type = WGPUBufferBindingType_Uniform;
    buffer_layout.hasDynamicOffset = false;
    buffer_layout.minBindingSize = sizeof(glm::mat4);

    WGPUBindGroupLayoutEntry global_layout_entries[1] = { 0 };
    global_layout_entries[0].binding = 0;
    global_layout_entries[0].visibility = WGPUShaderStage_Vertex;
    global_layout_entries[0].buffer = buffer_layout;

    WGPUBindGroupLayoutDescriptor global_layout_desc = { 0 };
    global_layout_desc.label = {"SpriteGlobalBindGroupLayout", WGPU_STRLEN};
    global_layout_desc.entryCount = 1;
    global_layout_desc.entries = global_layout_entries;

    m_global_bg_layout =
        wgpuDeviceCreateBindGroupLayout(device, &global_layout_desc);

    WGPUTextureBindingLayout texture_layout = { 0 };
    texture_layout.sampleType = WGPUTextureSampleType_Float;
    texture_layout.viewDimension = WGPUTextureViewDimension_2D;
    texture_layout.multisampled = 0;

    WGPUSamplerBindingLayout sampler_layout = { 0 };
    sampler_layout.type = WGPUSamplerBindingType_Filtering;

    WGPUBindGroupLayoutEntry bg_layout_entries[2] = { 0 };
    bg_layout_entries[0].binding = 0;
    bg_layout_entries[0].visibility = WGPUShaderStage_Fragment;
    bg_layout_entries[0].texture = texture_layout;
    bg_layout_entries[1].binding = 1;
    bg_layout_entries[1].visibility = WGPUShaderStage_Fragment;
    bg_layout_entries[1].sampler = sampler_layout;

    WGPUBindGroupLayoutDescriptor bg_layout_desc = { 0 };
    bg_layout_desc.label = {"SpriteBindGroupLayout", WGPU_STRLEN};
    bg_layout_desc.entryCount = 2;
    bg_layout_desc.entries = bg_layout_entries;

    m_bg_layout = wgpuDeviceCreateBindGroupLayout(device, &bg_layout_desc);

    // Pipeline layout
    WGPUBindGroupLayout bg_layouts[2] = {
        m_global_bg_layout,
        m_bg_layout,
    };

    WGPUPipelineLayoutDescriptor pipeline_layout_desc = { 0 };
    pipeline_layout_desc.bindGroupLayoutCount = 2;
    pipeline_layout_desc.bindGroupLayouts = bg_layouts;

    m_pipeline_layout =
        wgpuDeviceCreatePipelineLayout(device, &pipeline_layout_desc);

    std::array<WGPUVertexAttribute, 4> attributes;
    for (int i = 0; i < 4; ++i) {
        attributes[i].shaderLocation = i;
        attributes[i].format = WGPUVertexFormat_Float32;
        attributes[i].offset = i * sizeof(float);
    }

    // Pipeline
    WGPUVertexBufferLayout vertex_buffer_layout;
    vertex_buffer_layout.attributeCount = attributes.size();
    vertex_buffer_layout.attributes = attributes.data();
    vertex_buffer_layout.arrayStride = sizeof(float[4]);
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
    fragment.module = shader;
    fragment.entryPoint = {"fs_main", WGPU_STRLEN};
    fragment.targetCount = 1;
    fragment.targets = &color_target;

    WGPURenderPipelineDescriptor pipeline_desc = { 0 };
    pipeline_desc.label = {"pipeline", WGPU_STRLEN};
    pipeline_desc.layout = m_pipeline_layout;
    pipeline_desc.vertex.module = shader;
    pipeline_desc.vertex.entryPoint = {"vs_main", WGPU_STRLEN};
    pipeline_desc.vertex.bufferCount = 1;
    pipeline_desc.vertex.buffers = &vertex_buffer_layout;
    pipeline_desc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    pipeline_desc.primitive.frontFace = WGPUFrontFace_CCW;
    pipeline_desc.primitive.cullMode = WGPUCullMode_None;
    pipeline_desc.depthStencil = &depth_stencil;
    pipeline_desc.multisample.count = 1;
    pipeline_desc.multisample.count = 1;
    pipeline_desc.multisample.mask = ~0u;
    pipeline_desc.fragment = &fragment;

    m_pipeline = wgpuDeviceCreateRenderPipeline(device, &pipeline_desc);

    wgpuShaderModuleRelease(shader);
}

SpritePipeline::SpritePipeline(SpritePipeline&& other)
{
    *this = std::move(other);
}

SpritePipeline& SpritePipeline::operator=(SpritePipeline&& other)
{
    std::swap(m_global_bg_layout, other.m_global_bg_layout);
    std::swap(m_bg_layout, other.m_bg_layout);
    std::swap(m_pipeline_layout, other.m_pipeline_layout);
    std::swap(m_pipeline, other.m_pipeline);

    return *this;
}

SpritePipeline::~SpritePipeline()
{
    if (m_pipeline != nullptr) {
        wgpuRenderPipelineRelease(m_pipeline);
    }
    if (m_pipeline_layout != nullptr) {
        wgpuPipelineLayoutRelease(m_pipeline_layout);
    }
    if (m_bg_layout != nullptr) {
        wgpuBindGroupLayoutRelease(m_bg_layout);
    }
    if (m_global_bg_layout != nullptr) {
        wgpuBindGroupLayoutRelease(m_global_bg_layout);
    }
}

WGPUBindGroup SpritePipeline::create_global_bind_group(
    WGPUDevice device,
    WGPUBuffer buffer) const
{
    WGPUBindGroupEntry entries[1] = { 0 };
    entries[0].binding = 0;
    entries[0].buffer = buffer;
    entries[0].size = sizeof(glm::mat4);

    WGPUBindGroupDescriptor bind_group_desc = { 0 };
    bind_group_desc.label = {"SpriteGlobalBindGroup", WGPU_STRLEN};
    bind_group_desc.layout = m_global_bg_layout;
    bind_group_desc.entryCount = 1;
    bind_group_desc.entries = entries;

    return wgpuDeviceCreateBindGroup(device, &bind_group_desc);
}

WGPUBindGroup SpritePipeline::create_bind_group(
    WGPUDevice device,
    WGPUTextureView texture,
    WGPUSampler sampler
) const
{
    WGPUBindGroupEntry entries[2] = { 0 };
    entries[0].binding = 0;
    entries[0].textureView = texture;
    entries[1].binding = 1;
    entries[1].sampler = sampler;

    WGPUBindGroupDescriptor bind_group_desc = { 0 };
    bind_group_desc.label = {"SpriteBindGroup", WGPU_STRLEN};
    bind_group_desc.layout = m_bg_layout;
    bind_group_desc.entryCount = 2;
    bind_group_desc.entries = entries;

    return wgpuDeviceCreateBindGroup(device, &bind_group_desc);
}
