#ifndef NODE_H
#define NODE_H

#include "renderobject.h"
#include <memory>
#include <vector>

class Node
{
public:
    std::vector<std::unique_ptr<RenderObject>>& children()
    {
        return m_children;
    }

    const std::vector<std::unique_ptr<RenderObject>>& children() const
    {
        return m_children;
    }

private:
    std::vector<std::unique_ptr<RenderObject>> m_children;
};

#endif
