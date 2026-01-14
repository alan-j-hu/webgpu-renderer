#include "FileDialog.h"

#include <utility>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace fs = std::filesystem;

FileDialog::FileDialog(std::string name, fs::path path)
    : m_name(std::move(name)), m_current_dir(std::move(path)),
      m_state(FileDialog::CLOSED)
{
    m_backtrack_path = m_current_dir;
    iterate_dir();
}

const std::string& FileDialog::name() const
{
    return m_name;
}

void FileDialog::open(FileDialog::Mode mode)
{
    m_backtrack_path = m_current_dir;
    m_new_file.clear();
    m_state = mode;
    m_path_to_confirm.reset();
    m_selected = nullptr;
    iterate_dir();
}

std::optional<std::filesystem::path> FileDialog::update()
{
    const bool can_create = m_state & FileDialog::CREATE;
    const bool can_read = m_state & FileDialog::READ;

    std::optional<std::filesystem::path> output;

    if (m_path_to_confirm.has_value()) {
        ImGui::OpenPopup("Overwrite?");

        if (ImGui::BeginPopupModal("Overwrite?")) {
            if (ImGui::Button("Yes", ImVec2(0, 0))) {
                output = m_path_to_confirm;
                ImGui::CloseCurrentPopup();
                m_state = FileDialog::CLOSED;
                m_path_to_confirm.reset();
            }
            if (ImGui::Button("No", ImVec2(0, 0))) {
                ImGui::CloseCurrentPopup();
                m_state = FileDialog::CLOSED;
                m_path_to_confirm.reset();
            }
            ImGui::EndPopup();
        }

        return output;
    }

    if (m_state != 0) {
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
                auto fname = entry.path().filename();
                const char* c_str = fname.c_str();

                if (ImGui::Selectable(
                        c_str,
                        false,
                        ImGuiSelectableFlags_AllowDoubleClick)) {

                    if (ImGui::IsMouseDoubleClicked(0)) {
                        next_dir = entry.path();
                    }
                }
            }

            for (auto& entry : m_files) {
                auto fname = entry.path().filename();
                const char* cstr = fname.c_str();

                ImGuiSelectableFlags flags = can_read
                    ? ImGuiSelectableFlags_AllowDoubleClick
                    : ImGuiSelectableFlags_Disabled;

                bool select = m_selected == &entry;

                if (ImGui::Selectable(cstr, select, flags)) {
                    m_selected = &entry;
                    if (ImGui::IsMouseDoubleClicked(0)) {
                        if (can_create) {
                            m_path_to_confirm = entry.path();
                        } else {
                            output = entry.path();
                        }
                        m_state = FileDialog::CLOSED;
                    }
                }
            }
            ImGui::EndListBox();
        }

        if (can_create) {
            ImGui::InputText("New File", &m_new_file);
        }

        if (ImGui::Button("Ok", ImVec2(0, 0))) {
            if (m_new_file == "") {
                output = m_selected->path();
            } else {
                m_path_to_confirm = m_current_dir / m_new_file;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(0, 0))) {
            m_state = FileDialog::CLOSED;
        }

        if (next_dir != m_current_dir) {
            m_current_dir = next_dir;
            iterate_dir();
            if (backtrack_path_needs_update()) {
                m_backtrack_path = m_current_dir;
            }
        }

        if (m_state == 0) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if (m_path_to_confirm) {
        return std::nullopt;
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
