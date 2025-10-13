#ifndef RENDERABLE_H
#define RENDERABLE_H

class Frame;

class Renderable
{
public:
    virtual ~Renderable() = default;
    virtual void render(Frame& frame) = 0;
};

class RenderableRef : public Renderable
{
public:
    RenderableRef(Renderable& renderable) : m_inner(&renderable)
    {}

    void render(Frame& frame) override
    {
        m_inner->render(frame);
    }

private:
    Renderable* m_inner;
};

#endif
