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
    /**
       The logic for reordering list elements is confusing, so it's worth
       writing it out in a comment.

       We can conceptualize of sequence of "cursors" numbering the gaps between
       elements, and at the beginning and end.

       Cursor: 0     1     2     3     4
                -----------------------
       Index:  |  0  |  1  |  2  |  3  |
                -----------------------

       When the user drags a list element E, a cursor *between* elements will
       highlight, indicating between which two elements that E will be moved.
       For example, if we have the sequence ABCD and want to move B, and have
       highlighted cursor 3, we want to move B between C and D. Therefore, the
       updated sequence becomes ACBD.

       In particular, the cursor immediately before and after an element
       indicate that the element will not be moved.

       A reorder specification consists of an index (of the element to move)
       and a delta indicating how many steps to take, either forwards or
       backwards. The delta is created by counting cursor indices. For example,
       a reorder command { idx = 1, delta = 2 } will permute the sequence
       ABCD into ACDB. In other words, the element at index 1 (B) gets moved
       two steps forward.

       To carry out a reorder, a pair (idx, cursor) must be translated into a
       pair (idx, delta). If the destination cursor is before idx, we can
       think of the source cursor as the one right before idx. If the
       destination cursor is after idx, we can think of the source cursor as the
       one right after idx.
     */
    struct DragDropInfo {
        int idx;
        int cursor;
    };

    std::optional<DragDropInfo> m_drag_drop;
};

#endif
