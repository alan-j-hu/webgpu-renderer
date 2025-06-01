#include "tilesetpane.h"

TilesetPane::TilesetPane(int flex, AppState& app_state)
    : HorizontalLayout("TilesetPane", flex),
      m_editor("A", 1, app_state),
      m_editor2("B", 1, app_state)
{
    children.push_back(&m_editor);
    children.push_back(&m_editor2);
}
