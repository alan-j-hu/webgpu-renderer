#include "noworry/material/mesheffect.h"

MeshEffect::MeshEffect(WGPUDevice device)
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
