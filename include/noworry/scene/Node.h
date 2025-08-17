#ifndef NODE_H
#define NODE_H

#include "../Gfx3D/Renderable.h"
#include <memory>
#include <vector>

class Node
{
public:
    std::vector<std::unique_ptr<Renderable>>& children()
    {
        return m_children;
    }

    const std::vector<std::unique_ptr<Renderable>>& children() const
    {
        return m_children;
    }

private:
    std::vector<std::unique_ptr<Renderable>> m_children;
};

#endif
