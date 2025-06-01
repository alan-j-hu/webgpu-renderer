#include "tilesetpane.h"

TilesetPane::TilesetPane(int flex, AppState& app_state)
    : HorizontalLayout("TilesetPane", flex),
      m_editor("A", 1, app_state),
      m_tile_list("B", 1, app_state)
{
    children.push_back(&m_tile_list);
    children.push_back(&m_editor);
}
