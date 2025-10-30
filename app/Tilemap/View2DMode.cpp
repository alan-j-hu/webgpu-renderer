#include "View2DMode.h"

View2DMode::View2DMode(AppState& app_state, TilemapEditor& editor)
    : m_app_state(&app_state),
      m_editor(&editor)
{
}

AppState& View2DMode::app_state()
{
    return *m_app_state;
}

TilemapEditor& View2DMode::editor()
{
    return *m_editor;
}
