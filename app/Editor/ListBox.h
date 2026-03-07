#ifndef LIST_BOX_H
#define LIST_BOX_H

#include <optional>

class ListBox
{
public:
    enum ItemState {
        NONE,
        HOVERED,
        SELECTED
    };

    virtual int count() = 0;

    virtual ItemState item(int idx) = 0;

    virtual void select(int idx) = 0;

    virtual void request_reorder(int old_idx, int new_idx) = 0;

    void update();

private:
    struct DragDropInfo {
        int source;
        int target;
    };

    std::optional<DragDropInfo> m_drag_drop;
};

#endif
