#ifndef REORDER_LAYER_COMMAND_H
#define REORDER_LAYER_COMMAND_H

#include "Command.h"

class ReorderLayerCommand : public Command
{
public:
    ReorderLayerCommand(Level&, int, int);

    virtual const char* name() override;

protected:
    virtual std::expected<Outcome, std::string> up(Project&) override;
    virtual void down(Project&) override;

private:
    Level* m_level;
    int m_idx;
    int m_delta;
};

#endif
