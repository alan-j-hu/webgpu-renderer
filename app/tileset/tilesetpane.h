#ifndef TILESET_PANE_H
#define TILESET_PANE_H

#include "../pane.h"
#include "modelinspector.h"

class TilesetPane : public HorizontalLayout
{
public:
    TilesetPane(int flex, ModalStack&, Renderer&);

    ModelInspector& editor() { return m_editor; }

private:
    ModelInspector m_editor;
    ModelInspector m_editor2;
};

#endif
