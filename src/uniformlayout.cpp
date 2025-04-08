#include "noworry/uniformlayout.h"

UniformLayout::UniformLayout(WGPUDevice device)
{
    WGPUBindGroupLayoutEntry layout_entries[1] = { 0 };
    layout_entries[0].binding = 0;
    layout_entries[0].visibility = WGPUShaderStage_Vertex;
    layout_entries[0].buffer.type = WGPUBufferBindingType_Uniform;
    layout_entries[0].buffer.minBindingSize = sizeof(Uniforms);

    WGPUBindGroupLayoutDescriptor layout_desc = { 0 };
    layout_desc.label = {"UniformLayout", WGPU_STRLEN};
    layout_desc.entryCount = 1;
    layout_desc.entries = layout_entries;

    m_layout = wgpuDeviceCreateBindGroupLayout(device, &layout_desc);
}

WGPUBindGroup UniformLayout::create_bind_group(
    WGPUDevice device, WGPUBuffer buffer)
{
    WGPUBindGroupEntry entries[1] = { 0 };
    entries[0].binding = 0;
    entries[0].buffer = buffer;
    entries[0].size = sizeof(Uniforms);

    WGPUBindGroupDescriptor bind_group_desc = { 0 };
    bind_group_desc.label = {"UniformBindGroup", WGPU_STRLEN};
    bind_group_desc.layout = m_layout;
    bind_group_desc.entryCount = 1;
    bind_group_desc.entries = entries;

    return wgpuDeviceCreateBindGroup(device, &bind_group_desc);
}
