#include "appstate.h"

AppState::AppState(WGPUDevice device)
    : m_renderer(device),
      m_resources(m_renderer)
{
    m_default_material = &m_resources.add_flat_material(0.5, 0.5, 0.5);
    m_wireframe_material = &m_resources.add_wireframe_material(0.5, 0.5, 0.5);
}
