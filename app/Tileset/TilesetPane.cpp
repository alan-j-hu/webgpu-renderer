#include "TilesetPane.h"

TilesetPane::TilesetPane(int flex, AppState& app_state)
    : HorizontalLayout("TilesetPane", flex),
      m_tile_list("B", 1, app_state)
{
    children.push_back(&m_tile_list);
}
