#ifndef CREATELAYERCOMMAND_H
#define CREATELAYERCOMMAND_H

#include "Command.h"
#include "../Project.h"

class CreateLayerCommand : public Command
{
public:
    CreateLayerCommand(LayerLocation);

    virtual const char* name() override;

protected:
    virtual Command::Outcome up(Project&) override;
    virtual void down(Project&) override;

private:
    LayerLocation m_loc;
};

#endif
