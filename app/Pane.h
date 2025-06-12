#ifndef PANE_H
#define PANE_H

#include <memory>
#include <string>
#include <vector>

/// A pane has a fixed size. If the pane's contents are larger than the pane's
/// size, a scrollbar will appear. It has a flex parameter that dictates its
/// proportions to sibling panes in a layout.
class Pane
{
public:
    Pane(std::string name, int flex);

    void render();

    int flex() { return m_flex; }
    int width() { return m_width; }
    int height() { return m_height; }

    virtual void resize(int width, int height);

protected:

    virtual void content() {}

private:
    std::string m_name;
    int m_flex;
    int m_width;
    int m_height;
};

/// Helper class for horizontal and vertical layouts.
class Layout : public Pane
{
public:
    Layout(std::string name, int flex);

    virtual void resize(int width, int height) override;

protected:
    std::vector<Pane*> children;

    virtual int variable_dim() = 0;
    virtual void set_variable_dim(Pane&, int) = 0;
};

/// A horizontal layout arranges its children from left to right within its
/// borders.
class HorizontalLayout : public Layout
{
public:
    HorizontalLayout(std::string name, int flex);

protected:
    virtual void content() override;

    virtual int variable_dim() override;
    virtual void set_variable_dim(Pane&, int) override;
};

/// A vertical layout arranges its children from top to bottom within its
/// borders.
class VerticalLayout : public Layout
{
public:
    VerticalLayout(std::string name, int flex);

protected:
    virtual void content() override;

    virtual int variable_dim() override;
    virtual void set_variable_dim(Pane&, int) override;
};

#endif
