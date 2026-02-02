#include "ExportDialog.h"
#include <filesystem>
#include "imgui.h"

ExportDialog::ExportDialog(AppState& app_state)
    : m_app_state(&app_state),
      m_selected_format {nullptr},
      m_state {State::CLOSED}
{
}

void ExportDialog::open()
{
    m_state = State::OPEN;
    m_selected_format = nullptr;
}

ExportDialog::State ExportDialog::update()
{
    if (m_state == State::OPEN) {
        ImGui::OpenPopup("Export");
    }

    const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize;
    ImGui::SetNextWindowSize(ImVec2(250, 0));
    if (ImGui::BeginPopupModal("Export", nullptr, window_flags)) {
        Exporter& exporter = m_app_state->exporter();
        Assimp::Exporter& ai_exporter = exporter.ai_exporter();
        const char* preview = m_selected_format == nullptr
            ? "Choose a format"
            : m_selected_format->id;

        if (ImGui::BeginCombo("Format", preview)) {
            size_t count = ai_exporter.GetExportFormatCount();
            for (int i = 0; i < count; ++i) {
                auto* desc = ai_exporter.GetExportFormatDescription(i);
                if (ImGui::Selectable(desc->id, m_selected_format == desc)) {
                    m_selected_format = desc;
                }
            }

            ImGui::EndCombo();
        }

        if (ImGui::Button("Export", ImVec2(0, 0))) {
            if (m_selected_format == nullptr) {
            } else {
                m_state = State::CLOSED;
                exporter.export_project(
                    m_app_state->project(),
                    *m_selected_format,
                    std::filesystem::path("./export"));
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(0, 0))) {
            m_state = State::CLOSED;
        }

        if (m_state == State::CLOSED) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    return m_state;
}
