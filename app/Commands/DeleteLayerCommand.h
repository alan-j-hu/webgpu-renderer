#ifndef DELETELAYERCOMMAND_H
#define DELETELAYERCOMMAND_H

#include "Command.h"
#include <memory>
#include <optional>

class DeleteLayerCommand : public Command
{
public:
    DeleteLayerCommand(int index);

    virtual const char* name() override;

protected:
    virtual Command::Outcome up(Project&) override;
    virtual void down(Project&) override;

private:
    int m_index;
    std::optional<std::unique_ptr<Layer>> m_layer;
};

#endif
