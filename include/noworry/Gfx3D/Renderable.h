#ifndef RENDERABLE_H
#define RENDERABLE_H

class Frame;

class Renderable
{
public:
    virtual ~Renderable() = default;
    virtual void render(Frame& frame) = 0;
};

#endif
