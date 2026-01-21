#include "FileDialog.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <algorithm>
#include <utility>

namespace fs = std::filesystem;

FileDialog::FileDialog(std::string name, fs::path path)
    : m_name(std::move(name)), m_current_dir(std::move(path)),
      m_state(State::CLOSED)
{
    m_backtrack_path = m_current_dir;
    iterate_dir();
}

const std::string& FileDialog::name() const
{
    return m_name;
}

void FileDialog::open(FileDialog::Flags flags, short width, short height)
{
    m_width = width;
    m_height = height;

    m_backtrack_path = m_current_dir;
    m_new_file.clear();
    m_state = State::OPEN;
    m_flags = flags;
    m_path_to_confirm.reset();
    m_selected = nullptr;
    iterate_dir();
}

std::optional<std::filesystem::path> FileDialog::update()
{
    std::optional<std::filesystem::path> output;

    if (m_path_to_confirm.has_value()) {
        return draw_overwrite_confirm();
    }

    if (m_state == State::OPEN) {
        ImGui::OpenPopup(m_name.c_str());
    }

    const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize;
    ImGui::SetNextWindowSize(ImVec2(m_width, m_height));
    if (ImGui::BeginPopupModal(m_name.c_str(), nullptr, window_flags)) {
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
                ImVec2(-FLT_MIN, 0.8 * m_height))) {

            for (auto& entry : m_subdirs) {
                auto fname = entry.path().filename();
                const char* c_str = fname.c_str();

                if (ImGui::Selectable(
                        c_str,
                        false,
                        ImGuiSelectableFlags_AllowDoubleClick)) {

                    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                        next_dir = entry.path();
                    }
                }
            }

            for (auto& entry : m_files) {
                auto fname = entry.path().filename();
                const char* cstr = fname.c_str();

                bool select = m_selected == &entry;
                const ImGuiSelectableFlags flags =
                    ImGuiSelectableFlags_AllowDoubleClick;
                if (ImGui::Selectable(cstr, select, flags)) {
                    m_selected = &entry;
                    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                        output = entry.path();
                    }
                }
            }
            ImGui::EndListBox();
        }

        if (m_flags == FileDialog::WRITE) {
            ImGui::InputText("New File", &m_new_file);
        }

        if (ImGui::Button("Ok", ImVec2(0, 0)) && !m_new_file.empty()) {
            output = m_current_dir / m_new_file;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(0, 0))) {
            m_state = State::CLOSED;
        }

        if (next_dir != m_current_dir) {
            m_current_dir = next_dir;
            iterate_dir();
            if (backtrack_path_needs_update()) {
                m_backtrack_path = m_current_dir;
            }
        }

        if (output) {
            m_state = State::CLOSED;
        }

        if (m_state == State::CLOSED) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if (m_flags == FileDialog::WRITE && output && fs::exists(*output)) {
        m_path_to_confirm = output;
        return std::nullopt;
    }

    return output;
}

std::optional<std::filesystem::path> FileDialog::draw_overwrite_confirm()
{
    std::optional<std::filesystem::path> output;
    ImGui::OpenPopup("Overwrite?");

    if (ImGui::BeginPopupModal("Overwrite?")) {
        if (ImGui::Button("Yes", ImVec2(0, 0))) {
            output = m_path_to_confirm;
            ImGui::CloseCurrentPopup();
            m_state = State::CLOSED;
            m_path_to_confirm.reset();
        }
        if (ImGui::Button("No", ImVec2(0, 0))) {
          ImGui::CloseCurrentPopup();
          m_state = State::CLOSED;
          m_path_to_confirm.reset();
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

    std::sort(m_subdirs.begin(), m_subdirs.end());
    std::sort(m_files.begin(), m_files.end());
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
