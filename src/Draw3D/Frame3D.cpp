#include "noworry/Draw3D/Frame3D.h"
#include "noworry/rendertarget.h"
#include <utility>

Frame3D::Frame3D(WGPUDevice device, RenderTarget& target)
{
    m_queue = wgpuDeviceGetQueue(device);

    WGPUCommandEncoderDescriptor encoder_desc = { 0 };
    m_encoder =
        wgpuDeviceCreateCommandEncoder(device, &encoder_desc);

    WGPURenderPassColorAttachment color_attachment = { 0 };
    color_attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
    color_attachment.loadOp = WGPULoadOp_Clear;
    color_attachment.storeOp = WGPUStoreOp_Store;
    color_attachment.clearValue = target.clear_color();
    color_attachment.view = target.texture().view();

    WGPURenderPassDepthStencilAttachment depth_stencil_attachment = { 0 };
    depth_stencil_attachment.view = target.depth_texture().view();
    depth_stencil_attachment.depthClearValue = 1.0f;
    depth_stencil_attachment.depthLoadOp = WGPULoadOp_Clear;
    depth_stencil_attachment.depthStoreOp = WGPUStoreOp_Store;
    depth_stencil_attachment.depthReadOnly = false;
    depth_stencil_attachment.stencilClearValue = 0.0f;
    depth_stencil_attachment.stencilLoadOp = WGPULoadOp_Undefined;
    depth_stencil_attachment.stencilStoreOp = WGPUStoreOp_Undefined;
    depth_stencil_attachment.stencilReadOnly = true;

    WGPURenderPassDescriptor render_pass_desc = { 0 };
    render_pass_desc.colorAttachmentCount = 1;
    render_pass_desc.colorAttachments = &color_attachment;
    render_pass_desc.depthStencilAttachment = &depth_stencil_attachment;

    m_pass = wgpuCommandEncoderBeginRenderPass(
        m_encoder, &render_pass_desc);
}

Frame3D::Frame3D(Frame3D&& other)
{
    m_queue = nullptr;
    m_encoder = nullptr;
    m_pass = nullptr;

    *this = std::move(other);
}

Frame3D& Frame3D::operator=(Frame3D&& other)
{
    std::swap(m_queue, other.m_queue);
    std::swap(m_encoder, other.m_encoder);
    std::swap(m_pass, other.m_pass);

    return *this;
}

Frame3D::~Frame3D()
{
    if (m_pass != nullptr) {
        wgpuRenderPassEncoderRelease(m_pass);
    }
    if (m_encoder != nullptr) {
        wgpuCommandEncoderRelease(m_encoder);
    }
}

void Frame3D::finish()
{
    wgpuRenderPassEncoderEnd(m_pass);

    WGPUCommandBufferDescriptor buffer_desc = { 0 };
    WGPUCommandBuffer command_buffer =
        wgpuCommandEncoderFinish(m_encoder, &buffer_desc);
    wgpuQueueSubmit(m_queue, 1, &command_buffer);

    wgpuCommandBufferRelease(command_buffer);
}
