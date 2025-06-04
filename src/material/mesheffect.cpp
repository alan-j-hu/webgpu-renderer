#include "noworry/material/mesheffect.h"
#include "noworry/material/material.h"
#include "noworry/scene/renderobject.h"
#include "noworry/layout.h"
#include "noworry/mesh.h"
#include "noworry/renderer.h"

MeshEffect::MeshEffect(WGPUDevice device, UniformLayout& ul)
    : m_pipeline(nullptr), m_pipeline_layout(nullptr),
      m_device(device), m_ul(&ul)
{
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
}

MeshEffect::~MeshEffect()
{
    if (m_pipeline != nullptr) {
        wgpuRenderPipelineRelease(m_pipeline);
    }
    if (m_pipeline_layout != nullptr) {
        wgpuPipelineLayoutRelease(m_pipeline_layout);
    }
    wgpuShaderModuleRelease(m_vertex_shader);
}

WGPUPipelineLayout MeshEffect::pipeline_layout()
{
    // Pipeline layout cannot be created in the constructor because it
    // requires a virtual call to get the material layout.
    if (m_pipeline_layout != nullptr) {
        return m_pipeline_layout;
    }

    WGPUBindGroupLayout layouts[3] = {
        m_ul->global_layout(),
        material_layout(),
        m_ul->model_layout()
    };

    WGPUPipelineLayoutDescriptor pipeline_layout_desc = { 0 };
    pipeline_layout_desc.bindGroupLayoutCount = 3;
    pipeline_layout_desc.bindGroupLayouts = layouts;

    m_pipeline_layout =
        wgpuDeviceCreatePipelineLayout(m_device, &pipeline_layout_desc);
    return m_pipeline_layout;
}

WGPURenderPipeline MeshEffect::pipeline()
{
    if (m_pipeline != nullptr) {
        return m_pipeline;
    }

    WGPUVertexBufferLayout vertex_buffer_layout;
    vertex_buffer_layout.attributeCount = Vertex::attributes().size();
    vertex_buffer_layout.attributes = Vertex::attributes().data();
    vertex_buffer_layout.arrayStride = sizeof(Vertex);
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
    fragment.module = fragment_shader();
    fragment.entryPoint = {"fs_main", WGPU_STRLEN};
    fragment.targetCount = 1;
    fragment.targets = &color_target;

    WGPURenderPipelineDescriptor pipeline_desc = { 0 };
    pipeline_desc.label = {"pipeline", WGPU_STRLEN};
    pipeline_desc.layout = pipeline_layout();
    pipeline_desc.vertex.module = vertex_shader();
    pipeline_desc.vertex.entryPoint = {"vs_main", WGPU_STRLEN};
    pipeline_desc.vertex.bufferCount = 1;
    pipeline_desc.vertex.buffers = &vertex_buffer_layout;
    pipeline_desc.primitive.topology = topology();
    pipeline_desc.primitive.frontFace = WGPUFrontFace_CCW;
    pipeline_desc.primitive.cullMode = WGPUCullMode_None;
    pipeline_desc.depthStencil = &depth_stencil;
    pipeline_desc.multisample.count = 1;
    pipeline_desc.multisample.count = 1;
    pipeline_desc.multisample.mask = ~0u;
    pipeline_desc.fragment = &fragment;

    m_pipeline = wgpuDeviceCreateRenderPipeline(m_device, &pipeline_desc);
    return m_pipeline;
}

void MeshEffect::enqueue(RenderObject& model)
{
    m_queue.push_back(&model);
}

void MeshEffect::draw(WGPURenderPassEncoder encoder)
{
    wgpuRenderPassEncoderSetPipeline(encoder, pipeline());
    for (auto model : m_queue) {
        int count = (model->mesh().index_count() / 3) * 3;
        wgpuRenderPassEncoderSetBindGroup(
            encoder, 1, model->material().bind_group(), 0, nullptr);
        wgpuRenderPassEncoderSetBindGroup(
            encoder, 2, model->bind_group(), 0, nullptr);

        wgpuRenderPassEncoderSetVertexBuffer(
            encoder,
            0,
            model->mesh().vertex_buffer(),
            0,
            wgpuBufferGetSize(model->mesh().vertex_buffer()));

        wgpuRenderPassEncoderSetIndexBuffer(
            encoder,
            model->mesh().index_buffer(),
            WGPUIndexFormat_Uint16,
            0,
            count * sizeof(std::uint16_t));

        wgpuRenderPassEncoderDrawIndexed(encoder, count, 1, 0, 0, 0);
    }
    m_queue.clear();
}
