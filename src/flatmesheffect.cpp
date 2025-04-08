#include "noworry/flatmesheffect.h"
#include "noworry/uniformlayout.h"

FlatMeshEffect::FlatMeshEffect(WGPUDevice device, UniformLayout& ul)
    : MeshEffect(device)
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

@group(1) @binding(0) var<uniform> color: vec3f;

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
  return vec4(color, 1);
}
)";

    WGPUShaderModuleWGSLDescriptor vertex_wgsl_desc = { 0 };
    vertex_wgsl_desc.chain.sType = WGPUSType_ShaderSourceWGSL;
    vertex_wgsl_desc.code = {code, WGPU_STRLEN};

    WGPUShaderModuleDescriptor vertex_desc = { 0 };
    vertex_desc.nextInChain = &vertex_wgsl_desc.chain;
    m_vertex_shader =
        wgpuDeviceCreateShaderModule(device, &vertex_desc);

    WGPUShaderModuleWGSLDescriptor frag_wgsl_desc = { 0 };
    frag_wgsl_desc.chain.sType = WGPUSType_ShaderSourceWGSL;
    frag_wgsl_desc.code = {code, WGPU_STRLEN};

    WGPUShaderModuleDescriptor frag_desc = { 0 };
    frag_desc.nextInChain = &frag_wgsl_desc.chain;
    m_fragment_shader =
        wgpuDeviceCreateShaderModule(device, &frag_desc);

    WGPUBindGroupLayoutEntry material_layout_entries[1] = { 0 };
    material_layout_entries[0].binding = 0;
    material_layout_entries[0].visibility = WGPUShaderStage_Fragment;
    material_layout_entries[0].buffer.type = WGPUBufferBindingType_Uniform;
    material_layout_entries[0].buffer.minBindingSize = 12;

    WGPUBindGroupLayoutDescriptor material_layout_desc = { 0 };
    material_layout_desc.label = {"MaterialLayout", WGPU_STRLEN};
    material_layout_desc.entryCount = 1;
    material_layout_desc.entries = material_layout_entries;

    m_material_layout =
        wgpuDeviceCreateBindGroupLayout(device, &material_layout_desc);

    WGPUBindGroupLayout layouts[3] = {
        ul.layout(),
        material_layout(),
        model_layout()
    };

    WGPUPipelineLayoutDescriptor pipeline_layout_desc = { 0 };
    pipeline_layout_desc.bindGroupLayoutCount = 3;
    pipeline_layout_desc.bindGroupLayouts = layouts;

    m_pipeline_layout =
        wgpuDeviceCreatePipelineLayout(device, &pipeline_layout_desc);
}

FlatMeshEffect::~FlatMeshEffect()
{
    wgpuPipelineLayoutRelease(m_pipeline_layout);
    wgpuBindGroupLayoutRelease(m_material_layout);
    wgpuShaderModuleRelease(m_fragment_shader);
    wgpuShaderModuleRelease(m_vertex_shader);
}

WGPUBindGroup FlatMeshEffect::create_material_group(
    WGPUDevice device, WGPUBuffer buffer)
{
    WGPUBindGroupEntry entries[2] = { 0 };
    entries[0].binding = 0;
    entries[0].buffer = buffer;
    entries[0].size = 12;

    WGPUBindGroupDescriptor bind_group_desc = { 0 };
    bind_group_desc.label = {"MaterialBindGroup", WGPU_STRLEN};
    bind_group_desc.layout = m_material_layout;
    bind_group_desc.entryCount = 1;
    bind_group_desc.entries = entries;

    return wgpuDeviceCreateBindGroup(device, &bind_group_desc);
}
