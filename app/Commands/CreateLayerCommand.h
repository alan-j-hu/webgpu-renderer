#ifndef CREATELAYERCOMMAND_H
#define CREATELAYERCOMMAND_H

#include "Command.h"

class CreateLayerCommand : public Command
{
public:
    CreateLayerCommand();

    virtual const char* name() override;

protected:
    virtual void up(Project&) override;
    virtual void down(Project&) override;

private:
};

#endif
