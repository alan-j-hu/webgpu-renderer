#include <webgpu/webgpu.h>

#ifdef __cplusplus
extern "C" {
#endif

WGPUAdapter request_adapter(WGPUInstance instance, WGPUSurface surface);

WGPUDevice request_device(WGPUInstance instance, WGPUAdapter adapter);

#ifdef __cplusplus
}
#endif
