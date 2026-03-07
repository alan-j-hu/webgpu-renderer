#ifndef LAYER_LIST_H
#define LAYER_LIST_H

#include "../ListBox.h"

class AppState;
class Editor;
class Level;

class LayerList : public ListBox
{
public:
    LayerList(AppState&, Editor&);

    virtual int count() override;

    virtual ItemState item(int idx) override;

    virtual void select(int idx) override;

    virtual void request_reorder(int idx, int delta) override;

    void draw();

private:
    AppState* m_app_state;
    Editor* m_editor;

    void context_menu(Level& level, int idx);
};

#endif
