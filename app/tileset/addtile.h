#ifndef ADDTILE_H
#define ADDTILE_H

#include "../appstate.h"
#include "../modal.h"
#include "noworry/texture.h"
#include <filesystem>
#include <memory>
#include <vector>

/// Modal for adding a new tile.
class AddTile : public Modal
{
public:
    AddTile(AppState&);

    virtual ModalResponse render() override;

private:
    AppState* m_app_state;
    std::shared_ptr<Texture> m_texture;
    std::vector<std::filesystem::path> m_sink;
};

#endif
