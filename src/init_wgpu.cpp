#include "noworry/init_wgpu.h"
#include <iostream>
#include <string_view>
#include <webgpu/webgpu.h>

std::string_view convert_stringview(WGPUStringView view)
{
    if (view.length == 0) {
        return std::string_view();
    } else if (view.data == nullptr) {
        return std::string_view();
    } else if (view.length == WGPU_STRLEN) {
        return std::string_view(view.data);
    } else {
        return std::string_view(view.data, view.length);
    }
}

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
    std::cerr << "Device lost.\n";
}

void uncaptured_error_callback(
    WGPUDevice const* device,
    WGPUErrorType type,
    WGPUStringView message,
    void* userdata1,
    void* userdata2)
{
    std::cerr << "Error: " << convert_stringview(message) << std::endl;
}

void wait_future(WGPUInstance instance, WGPUFuture future)
{
    WGPUFutureWaitInfo info = {};
    info.future = future;
    while (!info.completed) {
        wgpuInstanceWaitAny(instance, 1, &info, 0);
    }
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
    wait_future(instance, wgpuInstanceRequestAdapter(instance, &options, info));

    return adapter;
}

WGPUDevice request_device(WGPUInstance instance, WGPUAdapter adapter)
{
    WGPUDevice device;

    WGPUDeviceDescriptor desc = { 0 };
    desc.nextInChain = NULL;
    desc.label.data = "device";
    desc.label.length = WGPU_STRLEN;
    desc.deviceLostCallbackInfo.mode = WGPUCallbackMode_AllowSpontaneous;
    desc.deviceLostCallbackInfo.callback = device_lost_callback;
    desc.uncapturedErrorCallbackInfo.callback = uncaptured_error_callback;

    WGPURequestDeviceCallbackInfo info = { 0 };
    info.nextInChain = NULL;
    info.mode = WGPUCallbackMode_AllowSpontaneous;
    info.callback = request_device_callback;
    info.userdata1 = &device;
    info.userdata2 = NULL;
    wait_future(instance, wgpuAdapterRequestDevice(adapter, &desc, info));

    return device;
}
