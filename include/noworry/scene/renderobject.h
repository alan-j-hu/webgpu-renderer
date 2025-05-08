#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "../material/mesheffect.h"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

struct Material;
struct Mesh;

class RenderObject
{
public:
    RenderObject(
        WGPUDevice device, MeshEffect& effect, const Mesh& mesh, Material&);
    virtual ~RenderObject();

    const Mesh& mesh() const { return m_mesh; }
    Material& material() { return *m_material; }
    WGPUBindGroup bind_group() { return m_bind_group; }

    void set_material(Material& material);

    void set_translation(const glm::vec3& translation);
    void set_yaw(float yaw);
    void set_scale(float scale);

    void copy_to_gpu(WGPUDevice device);

private:
    ModelData m_model;
    const Mesh& m_mesh;
    Material* m_material;
    WGPUBuffer m_buffer;
    WGPUBindGroup m_bind_group;
    glm::vec3 m_translation;
    float m_yaw;
    float m_scale;

    void update_matrix();
};

#endif
