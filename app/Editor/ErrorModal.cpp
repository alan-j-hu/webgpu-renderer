#include "ErrorModal.h"
#include <imgui.h>

ErrorModal::ErrorModal()
    : m_state(ErrorModal::State::CLOSED)
{
}

void ErrorModal::open(const char* message)
{
    m_state = ErrorModal::State::OPEN;
    m_message = std::string(message);
}

void ErrorModal::update(const char* id)
{
    if (m_state == ErrorModal::State::OPEN) {
        ImGui::OpenPopup(id);
    }
    if (ImGui::BeginPopupModal(id)) {

        ImGui::Text("%s", m_message.c_str());

        if (ImGui::Button("OK")) {
            m_state = ErrorModal::State::CLOSED;
        }

        if (m_state == ErrorModal::State::CLOSED) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
