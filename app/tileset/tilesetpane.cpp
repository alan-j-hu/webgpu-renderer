#include "tilesetpane.h"

TilesetPane::TilesetPane(int flex, ModalStack& stack, Renderer& renderer)
    : HorizontalLayout("TilesetPane", flex),
      m_editor("A", 1, stack, renderer),
      m_editor2("B", 1, stack, renderer)
{
    children.push_back(&m_editor);
    children.push_back(&m_editor2);
}
