#include "noworry/layout.h"

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

UniformLayout::UniformLayout(WGPUDevice device)
{
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

UniformLayout::~UniformLayout()
{
    wgpuBindGroupLayoutRelease(m_model_layout);
    wgpuBindGroupLayoutRelease(m_global_layout);
}

WGPUBindGroup UniformLayout::create_global_group(
    WGPUDevice device, WGPUBuffer buffer)
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

WGPUBindGroup UniformLayout::create_model_group(
    WGPUDevice device,
    WGPUBuffer transform)
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