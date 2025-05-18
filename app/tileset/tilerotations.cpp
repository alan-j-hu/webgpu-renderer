#include "tilerotations.h"
#include "tileseteditor.h"
#include <utility>

TileRotations::TileRotations(
    TilesetEditor& editor,
    std::string name,
    std::vector<Vertex> vertices,
    std::vector<std::uint16_t> indices)
    : m_name(std::move(name)),
      m_original(editor, std::move(vertices), std::move(indices)),
      m_rotate90(m_original.rotate90(editor, 1, 1)),
      m_rotate180(m_original.rotate180(editor, 1, 1)),
      m_rotate270(m_original.rotate270(editor, 1, 1))
{
}
