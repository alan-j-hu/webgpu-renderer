#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include "modal.h"
#include <filesystem>
#include <vector>

class FileDialog : public Modal
{
public:
    FileDialog(std::filesystem::path path,
               std::vector<std::filesystem::path>& sink);

    virtual bool render() override;

private:
    std::filesystem::path m_current_dir;
    std::filesystem::path m_backtrack_path;
    std::vector<std::filesystem::directory_entry> m_subdirs;
    std::vector<std::filesystem::directory_entry> m_files;
    std::vector<std::filesystem::path>& m_sink;

    void iterate_dir();
    bool backtrack_path_needs_update();
};

#endif
