#include "FileDialog.h"

#include <utility>
#include "imgui.h"

namespace fs = std::filesystem;

FileDialog::FileDialog(std::string name, fs::path path)
    : m_name(std::move(name)), m_current_dir(std::move(path)),
      m_state(FileDialog::State::Closed)
{
    m_backtrack_path = m_current_dir;
    iterate_dir();
}

const std::string& FileDialog::name() const
{
    return m_name;
}

void FileDialog::open()
{
    m_state = FileDialog::State::Open;
}

std::optional<std::filesystem::path> FileDialog::update()
{
    std::optional<std::filesystem::path> output;

    if (m_state == FileDialog::State::Open) {
        ImGui::OpenPopup(m_name.c_str());
    }

    if (ImGui::BeginPopupModal(m_name.c_str())) {
        fs::path next_dir = m_current_dir;
        fs::path header;
        int n = 0;
        for (fs::path comp : m_backtrack_path) {
            ImGui::PushID(n++);
            const char* name = comp.c_str();
            header /= comp;
            if (ImGui::Button(name)) {
                next_dir = header;
            }
            ImGui::SameLine();
            ImGui::PopID();
        }
        ImGui::NewLine();

        if (ImGui::BeginListBox(
                "##Directories and files",
                ImVec2(-FLT_MIN, 0))) {

            for (auto& entry : m_subdirs) {
                if (ImGui::Selectable(
                        entry.path().filename().c_str(),
                        false,
                        ImGuiSelectableFlags_AllowDoubleClick)) {

                    if (ImGui::IsMouseDoubleClicked(0)) {
                        next_dir = entry.path();
                    }
                }
            }

            for (auto& entry : m_files) {
                if (ImGui::Selectable(
                        entry.path().filename().c_str(),
                        false,
                        ImGuiSelectableFlags_AllowDoubleClick)) {

                    if (ImGui::IsMouseDoubleClicked(0)) {
                        output = entry.path();
                        m_state = FileDialog::State::Closed;
                    }
                }
            }
            ImGui::EndListBox();
        }

        if (ImGui::Button("Cancel", ImVec2(0, 0))) {
            m_state = FileDialog::State::Closed;
        }

        if (next_dir != m_current_dir) {
            m_current_dir = next_dir;
            iterate_dir();
            if (backtrack_path_needs_update()) {
                m_backtrack_path = m_current_dir;
            }
        }

        if (m_state == FileDialog::State::Closed) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    return output;
}

void FileDialog::iterate_dir()
{
    m_subdirs.clear();
    m_files.clear();

    fs::directory_iterator end;
    for (auto it = fs::directory_iterator(m_current_dir); it != end; ++it) {
        auto& entry = *it;
        if (entry.is_directory()) {
            m_subdirs.push_back(entry);
        } else {
            m_files.push_back(entry);
        }
    }
}

bool FileDialog::backtrack_path_needs_update()
{
    auto end = m_current_dir.end();
    auto it = m_current_dir.begin();
    for (fs::path comp : m_backtrack_path) {
        if (it == end) {
            return false;
        }
        if (comp != *it) {
            return true;
        }
        ++it;
    }
    return true;
}
