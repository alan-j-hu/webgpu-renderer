#ifndef RENDERABLE_H
#define RENDERABLE_H

class RenderContext;

class Renderable
{
public:
    virtual ~Renderable() = default;
    virtual void render(RenderContext&) = 0;
};

#endif
