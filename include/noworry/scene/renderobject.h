#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "../transform.h"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

struct Material;
struct BasicMesh;
struct ModelGroup;
struct Renderer;

/// Tells the renderer to draw the given mesh with the given material,
/// at the given transform.
class RenderObject
{
public:
    RenderObject(Renderer&, Transform&, const BasicMesh&, Material&);

    Transform& transform() { return m_transform; }
    const BasicMesh& mesh() const { return *m_mesh; }
    Material& material() { return *m_material; }

private:
    Transform m_transform;
    const BasicMesh* m_mesh;
    Material* m_material;
};

#endif
