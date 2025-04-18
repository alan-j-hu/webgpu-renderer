#include "modal.h"
#include "imgui.h"
#include <utility>

Modal::Modal(std::string_view title)
    : m_title(title)
{
}

void Modal::render()
{
}

void Modal::on_submit()
{
}

void Modal::on_cancel()
{
}

void ModalStack::push(std::unique_ptr<Modal> modal)
{
    m_modals.push_back(std::move(modal));
}

void ModalStack::render()
{
    int last = m_modals.size() - 1;
    if (last < 0) {
        return;
    }
    ImGui::BeginDisabled(true);
    for (int i = 0; i < last; ++i) {
        render_modal(*m_modals[i]);
    }
    ImGui::EndDisabled();

    bool close = render_modal(*m_modals[last]);

    if (close) {
        m_modals.pop_back();
    }
}

bool ModalStack::render_modal(Modal& modal)
{
    bool close = false;
    ImGui::Begin(modal.title().c_str());

    modal.render();

    if (ImGui::Button("Ok", ImVec2(0, 0))) {
        close = true;
        modal.on_submit();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(0, 0))) {
        close = true;
        modal.on_cancel();
    }
    ImGui::End();
    return close;
}
