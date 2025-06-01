#ifndef APPSTATE_H
#define APPSTATE_H

#include "modal.h"
#include "noworry/renderer.h"
#include "noworry/resourcetable.h"
#include <webgpu/webgpu.h>

/// Helper class containing data used by multiple sub-editors.
class AppState
{
public:
    AppState(WGPUDevice);

    Renderer& renderer() { return m_renderer; }
    ModalStack& modals() { return m_modals; }
    ResourceTable& resources() { return m_resources; }

    Material& default_material() { return *m_default_material; }
    Material& wireframe_material() { return *m_wireframe_material; }

private:
    Renderer m_renderer;
    ModalStack m_modals;
    ResourceTable m_resources;

    Material* m_default_material;
    Material* m_wireframe_material;
};

#endif
