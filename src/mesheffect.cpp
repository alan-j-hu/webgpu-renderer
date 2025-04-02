#include "noworry/mesheffect.h"

MeshEffect::MeshEffect(WGPUDevice device)
{
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
    wgpuBindGroupLayoutRelease(m_camera_layout);
}

WGPUBindGroup MeshEffect::create_camera_group(
    WGPUDevice device,
    WGPUBuffer buffer)
{
    WGPUBindGroupEntry entries[1] = { 0 };
    entries[0].binding = 0;
    entries[0].buffer = buffer;
    entries[0].size = sizeof(CameraData);

    WGPUBindGroupDescriptor bind_group_desc = { 0 };
    bind_group_desc.label = {"CameraBindGroup", WGPU_STRLEN};
    bind_group_desc.layout = camera_layout();
    bind_group_desc.entryCount = 1;
    bind_group_desc.entries = entries;

    return wgpuDeviceCreateBindGroup(device, &bind_group_desc);
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
