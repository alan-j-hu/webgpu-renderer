#include "noworry/Pipeline/MeshVertexShader.h"

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

MeshVertexShader::MeshVertexShader(WGPUDevice device)
{
    // Vertex shader
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

    // Global layout
    WGPUBindGroupLayoutEntry global_layout_entries[1] = { 0 };
    global_layout_entries[0].binding = 0;
    global_layout_entries[0].visibility = WGPUShaderStage_Vertex;
    global_layout_entries[0].buffer.type = WGPUBufferBindingType_Uniform;
    global_layout_entries[0].buffer.minBindingSize = sizeof(GlobalUniforms);

    WGPUBindGroupLayoutDescriptor global_layout_desc = { 0 };
    global_layout_desc.label = {"UniformLayout", WGPU_STRLEN};
    global_layout_desc.entryCount = 1;
    global_layout_desc.entries = global_layout_entries;

    m_global_layout =
        wgpuDeviceCreateBindGroupLayout(device, &global_layout_desc);

    // Model layout
    WGPUBindGroupLayoutEntry model_layout_entries[2] = { 0 };
    model_layout_entries[0].binding = 0;
    model_layout_entries[0].visibility = WGPUShaderStage_Vertex;
    model_layout_entries[0].buffer.type = WGPUBufferBindingType_Uniform;
    model_layout_entries[0].buffer.minBindingSize = sizeof(ModelUniforms);

    WGPUBindGroupLayoutDescriptor model_layout_desc = { 0 };
    model_layout_desc.label = {"ModelLayout", WGPU_STRLEN};
    model_layout_desc.entryCount = 1;
    model_layout_desc.entries = model_layout_entries;

    m_model_layout =
        wgpuDeviceCreateBindGroupLayout(device, &model_layout_desc);
}

MeshVertexShader::~MeshVertexShader()
{
    if (m_model_layout != nullptr) {
        wgpuBindGroupLayoutRelease(m_model_layout);
    }
    if (m_global_layout != nullptr) {
        wgpuBindGroupLayoutRelease(m_global_layout);
    }
    if (m_vertex_shader != nullptr) {
        wgpuShaderModuleRelease(m_vertex_shader);
    }
}

WGPUBindGroup MeshVertexShader::create_global_group(
    WGPUDevice device, WGPUBuffer buffer) const
{
    WGPUBindGroupEntry entries[1] = { 0 };
    entries[0].binding = 0;
    entries[0].buffer = buffer;
    entries[0].size = sizeof(GlobalUniforms);

    WGPUBindGroupDescriptor bind_group_desc = { 0 };
    bind_group_desc.label = {"UniformBindGroup", WGPU_STRLEN};
    bind_group_desc.layout = m_global_layout;
    bind_group_desc.entryCount = 1;
    bind_group_desc.entries = entries;

    return wgpuDeviceCreateBindGroup(device, &bind_group_desc);
}

WGPUBindGroup MeshVertexShader::create_model_group(
    WGPUDevice device,
    WGPUBuffer transform) const
{
    WGPUBindGroupEntry entries[1] = { 0 };
    entries[0].binding = 0;
    entries[0].buffer = transform;
    entries[0].size = sizeof(ModelUniforms);

    WGPUBindGroupDescriptor bind_group_desc = { 0 };
    bind_group_desc.label = {"ModelBindGroup", WGPU_STRLEN};
    bind_group_desc.layout = m_model_layout;
    bind_group_desc.entryCount = 1;
    bind_group_desc.entries = entries;

    return wgpuDeviceCreateBindGroup(device, &bind_group_desc);
}
