#include "noworry/Draw2D/Frame2D.h"
#include <utility>

Frame2D::Frame2D(WGPUDevice device, RenderTarget& target)
    : m_target(&target)
{
    m_queue = wgpuDeviceGetQueue(device);

    WGPUCommandEncoderDescriptor encoder_desc = { 0 };
    m_encoder =
        wgpuDeviceCreateCommandEncoder(device, &encoder_desc);

    m_load_op = WGPULoadOp_Clear;
}

Frame2D::Frame2D(Frame2D&& other)
{
    m_target = nullptr;
    m_queue = nullptr;
    m_encoder = nullptr;
    *this = std::move(other);
}

Frame2D& Frame2D::operator=(Frame2D&& other)
{
    std::swap(m_target, other.m_target);
    std::swap(m_queue, other.m_queue);
    std::swap(m_encoder, other.m_encoder);
    std::swap(m_load_op, other.m_load_op);

    return *this;
}

Frame2D::~Frame2D()
{
    if (m_encoder != nullptr) {
        wgpuCommandEncoderRelease(m_encoder);
    }
}

WGPURenderPassEncoder Frame2D::begin_pass()
{
    WGPURenderPassColorAttachment color_attachment = { 0 };
    color_attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
    color_attachment.loadOp = m_load_op;
    color_attachment.storeOp = WGPUStoreOp_Store;
    color_attachment.clearValue = m_target->clear_color();
    color_attachment.view = m_target->texture().view();

    m_load_op = WGPULoadOp_Load;

    WGPURenderPassDepthStencilAttachment depth_stencil_attachment = { 0 };
    depth_stencil_attachment.view = m_target->depth_texture().view();
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

    return wgpuCommandEncoderBeginRenderPass(
        m_encoder, &render_pass_desc);
}

void Frame2D::end_pass(WGPURenderPassEncoder pass)
{
    wgpuRenderPassEncoderEnd(pass);
    wgpuRenderPassEncoderRelease(pass);
}

void Frame2D::finish()
{
    WGPUCommandBufferDescriptor buffer_desc = { 0 };
    WGPUCommandBuffer command_buffer =
        wgpuCommandEncoderFinish(m_encoder, &buffer_desc);
    wgpuQueueSubmit(m_queue, 1, &command_buffer);
    wgpuCommandBufferRelease(command_buffer);

    m_load_op = WGPULoadOp_Clear;
}
