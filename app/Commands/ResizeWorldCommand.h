#ifndef RESIZE_WORLD_COMMAND_H
#define RESIZE_WORLD_COMMAND_H

#include "Command.h"

class ResizeWorldCommand : public Command
{
public:
    ResizeWorldCommand(World&, int top, int bottom, int left, int right);

    virtual const char* name() override;

protected:
    virtual std::expected<Outcome, std::string> up(Project&) override;
    virtual void down(Project&) override;

private:
    World* m_world;
    int m_top;
    int m_bottom;
    int m_left;
    int m_right;
};

#endif
