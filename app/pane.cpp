#include "pane.h"
#include <utility>
#include "imgui.h"

Pane::Pane(std::string name, int flex)
    : m_name(std::move(name)), m_flex(flex),
      m_width(0), m_height(0)
{
}

void Pane::render()
{
    if (ImGui::BeginChild(m_name.c_str(),
                          ImVec2(m_width, m_height))) {
        content();
    }
    ImGui::EndChild();
}

void Pane::resize(int w, int h)
{
    m_width = w;
    m_height = h;
}

Layout::Layout(std::string name, int flex)
    : Pane(std::move(name), flex)
{
}

void Layout::resize(int width, int height)
{
    Pane::resize(width, height);

    int flex_total = 0;
    for (auto& pane : children) {
        flex_total += pane->flex();
    }

    int dim = variable_dim();
    double unit = ((double) dim) / flex_total;
    for (auto& pane : children) {
        set_variable_dim(*pane, (int) (unit * pane->flex()));
    }
}

HorizontalLayout::HorizontalLayout(std::string name, int flex)
    : Layout(std::move(name), flex)
{
}

void HorizontalLayout::content()
{
    for (auto& pane : children) {
        pane->render();
        ImGui::SameLine();
    }
    ImGui::NewLine();
}

int HorizontalLayout::variable_dim()
{
    return width();
}

void HorizontalLayout::set_variable_dim(Pane& pane, int width)
{
    pane.resize(width, height());
}

VerticalLayout::VerticalLayout(std::string name, int flex)
    : Layout(std::move(name), flex)
{
}

void VerticalLayout::content()
{
    for (auto& pane : children) {
        pane->render();
    }
}

int VerticalLayout::variable_dim()
{
    return height();
}

void VerticalLayout::set_variable_dim(Pane& pane, int height)
{
    pane.resize(width(), height);
}
