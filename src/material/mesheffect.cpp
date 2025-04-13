#include "noworry/material/mesheffect.h"

MeshEffect::MeshEffect(WGPUDevice device)
{
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
