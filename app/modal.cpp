#include "modal.h"
#include "imgui.h"
#include <utility>

Modal::Modal(std::string_view title)
    : m_title(title)
{
}

ModalStack::ModalStack()
    : m_should_pop(false)
{
}

void ModalStack::push(std::unique_ptr<Modal> modal)
{
    m_modals.push_back(std::move(modal));
}

void ModalStack::render()
{
    if (m_should_pop) {
        m_should_pop = false;
        m_modals.pop_back();
    }

    int last = m_modals.size() - 1;
    if (last < 0) {
        return;
    }
    ImGui::BeginDisabled(true);
    for (int i = 0; i < last; ++i) {
        render_modal(*m_modals[i]);
    }
    ImGui::EndDisabled();

    const ModalResponse response = render_modal(*m_modals[last]);

    // Pop on the next frame to let ImGui process the draw commands first
    if (response == ModalResponse::Close) {
        m_should_pop = true;
    }
}

ModalResponse ModalStack::render_modal(Modal& modal)
{
    ImGui::SetNextWindowSize(ImVec2(0, 0));
    ImGui::Begin(modal.title().c_str());

    const ModalResponse response = modal.render();

    ImGui::End();
    return response;
}
