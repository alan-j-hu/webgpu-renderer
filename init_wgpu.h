#include <webgpu/webgpu.h>

WGPUAdapter request_adapter(WGPUInstance instance, WGPUSurface surface);

WGPUDevice request_device(WGPUInstance instance, WGPUAdapter adapter);
