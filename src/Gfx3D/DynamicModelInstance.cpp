#include "noworry/Gfx3D/DynamicModelInstance.h"
#include "noworry/Gfx3D/DynamicModel.h"
#include "noworry/renderer.h"

DynamicModelInstance::DynamicModelInstance(const DynamicModel& dynamic)
    : m_dynamic(&dynamic), m_transform(1)
{
}

void DynamicModelInstance::render(Frame& frame)
{
    for (const auto& part : *m_dynamic) {
        frame.add(part.second, *part.first, m_transform);
    }
}
