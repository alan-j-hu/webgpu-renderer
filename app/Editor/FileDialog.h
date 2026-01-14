#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <filesystem>
#include <optional>
#include <vector>

/// File selector modal.
class FileDialog
{
public:
    enum Mode
    {
        CLOSED = 0,
        CREATE = 1,
        READ = 2
    };

    FileDialog(std::string name, std::filesystem::path path);

    const std::string& name() const;

    void open(Mode mode = READ);

    std::optional<std::filesystem::path> update();

private:
    std::string m_name;
    std::string m_new_file;
    std::filesystem::path m_current_dir;
    std::filesystem::path m_backtrack_path;
    std::vector<std::filesystem::directory_entry> m_subdirs;
    std::vector<std::filesystem::directory_entry> m_files;
    std::filesystem::directory_entry* m_selected;

    // 0 represents closed, non-zero indicates the Mode enum.
    //
    // Keeping the open/close state is necessary because ImGui::OpenPopup does
    // not work if Imgui::BeginPopup is in a different ID stack scope. This
    // problem often occurs when attempting to open a popup from MenuItem from
    // within BeginMenu, from within BeginMenuBar.
    //
    // See https://github.com/ocornut/imgui/issues/5468#issuecomment-1184017711
    Mode m_state;
    std::optional<std::filesystem::path> m_path_to_confirm;

    void iterate_dir();
    bool backtrack_path_needs_update();
};

#endif
