#include "noworry/Material/FlatEffect.h"

FlatEffect::FlatEffect(WGPUDevice device)
{
    const char* code = R"(
@group(1) @binding(0) var<uniform> color: vec3f;

struct FragmentInput {
  @builtin(position) pos: vec4f,
  @location(0) tex_coords: vec2f,
};

@fragment
fn fs_main(input: FragmentInput) -> @location(0) vec4f {
  return vec4(color, 1);
}
)";

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
}

FlatEffect::~FlatEffect()
{
    wgpuBindGroupLayoutRelease(m_material_layout);
    wgpuShaderModuleRelease(m_fragment_shader);
}

WGPUBindGroup FlatEffect::create_material_group(
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
