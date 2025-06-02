#ifndef ROTATIONDROPDOWN_H
#define ROTATIONDROPDOWN_H

enum class RotationTag
{
    Rotate0,
    Rotate90,
    Rotate180,
    Rotate270
};

/// UI for drawing a combo box for selecting from the four rotations.
class RotationDropdown
{
public:
    RotationDropdown(RotationTag&);

    void render();

private:
    RotationTag* m_rotation;

    int to_int(RotationTag);
};

#endif
