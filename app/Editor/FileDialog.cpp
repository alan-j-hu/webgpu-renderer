#include "FileDialog.h"

#include <utility>
#include "imgui.h"

namespace fs = std::filesystem;

FileDialog::FileDialog(fs::path path, std::vector<fs::path>& sink)
    : Modal("Choose a file"),
      m_current_dir(std::move(path)),
      m_sink(sink)
{
    m_backtrack_path = m_current_dir;
    iterate_dir();
}

ModalResponse FileDialog::render()
{
    ModalResponse response = ModalResponse::KeepOpen;
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

    if (ImGui::BeginListBox("##Directories and files", ImVec2(-FLT_MIN, 0))) {
        for (auto& entry : m_subdirs) {
            if (ImGui::Selectable(entry.path().filename().c_str(),
                                  false,
                                  ImGuiSelectableFlags_AllowDoubleClick)) {

                if (ImGui::IsMouseDoubleClicked(0)) {
                    next_dir = entry.path();
                }
            }
        }
        m_sink.clear();
        for (auto& entry : m_files) {
            if (ImGui::Selectable(entry.path().filename().c_str(),
                                  false,
                                  ImGuiSelectableFlags_AllowDoubleClick)) {

                if (ImGui::IsMouseDoubleClicked(0)) {
                    m_sink.push_back(entry.path());
                    response = ModalResponse::Close;
                }
            }
        }
        ImGui::EndListBox();
    }

    if (ImGui::Button("Cancel", ImVec2(0, 0))) {
        response = ModalResponse::Close;
    }

    if (next_dir != m_current_dir) {
        m_current_dir = next_dir;
        iterate_dir();
        if (backtrack_path_needs_update()) {
            m_backtrack_path = m_current_dir;
        }
    }

    return response;
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
