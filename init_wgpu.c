#include <stdio.h>
#include <webgpu/webgpu.h>

void request_adapter_callback(
    WGPURequestAdapterStatus status,
    WGPUAdapter adapter,
    WGPUStringView message,
    void* userdata1,
    void* userdata2)
{
    *((WGPUAdapter*)userdata1) = adapter;
}

void request_device_callback(
    WGPURequestDeviceStatus status,
    WGPUDevice device,
    WGPUStringView message,
    void* userdata1,
    void* userdata2)
{
    *((WGPUDevice*)userdata1) = device;
}

void device_lost_callback(
    WGPUDevice const* device,
    WGPUDeviceLostReason reason,
    WGPUStringView message,
    void* userdata1,
    void* userdata2)
{
    fprintf(stderr, "Device lost.");
}

void uncaptured_error_callback(
    WGPUDevice const* device,
    WGPUErrorType type,
    WGPUStringView message,
    void* userdata1,
    void* userdata2)
{
    fprintf(stderr, "Error: %s\n", message.data);
}

WGPUAdapter request_adapter(WGPUInstance instance, WGPUSurface surface)
{
    WGPUAdapter adapter;

    WGPURequestAdapterOptions options = { 0 };
    options.nextInChain = NULL;
    options.featureLevel = WGPUFeatureLevel_Core;
    options.powerPreference = WGPUPowerPreference_Undefined;
    options.forceFallbackAdapter = false;
    options.backendType = WGPUBackendType_Undefined;
    options.compatibleSurface = surface;

    WGPURequestAdapterCallbackInfo info = { 0 };
    info.nextInChain = NULL;
    info.mode = WGPUCallbackMode_AllowSpontaneous;
    info.callback = request_adapter_callback;
    info.userdata1 = &adapter;
    info.userdata2 = NULL;
    WGPUFuture future = wgpuInstanceRequestAdapter(instance, &options, info);

    WGPUWaitStatus status = WGPUWaitStatus_TimedOut;
    while (status == WGPUWaitStatus_TimedOut) {
        WGPUFutureWaitInfo wait_info = {
            .future = future,
        };
        status = wgpuInstanceWaitAny(instance, 1, &wait_info, 0);
    }
    return adapter;
}

WGPUDevice request_device(WGPUInstance instance, WGPUAdapter adapter)
{
    WGPUDevice device;

    WGPUDeviceDescriptor desc = { 0 };
    desc.nextInChain = NULL;
    desc.deviceLostCallbackInfo.mode = WGPUCallbackMode_AllowSpontaneous;
    desc.deviceLostCallbackInfo.callback = device_lost_callback;
    desc.uncapturedErrorCallbackInfo.callback = uncaptured_error_callback;

    WGPURequestDeviceCallbackInfo info = { 0 };
    info.nextInChain = NULL;
    info.mode = WGPUCallbackMode_AllowSpontaneous;
    info.callback = request_device_callback;
    info.userdata1 = &device;
    info.userdata2 = NULL;

    WGPUFuture future = wgpuAdapterRequestDevice(adapter, &desc, info);
    WGPUWaitStatus status = WGPUWaitStatus_TimedOut;
    while (status == WGPUWaitStatus_TimedOut) {
        WGPUFutureWaitInfo wait_info = {
            .future = future,
        };
        status = wgpuInstanceWaitAny(instance, 1, &wait_info, 0);
    }

    return device;
}
