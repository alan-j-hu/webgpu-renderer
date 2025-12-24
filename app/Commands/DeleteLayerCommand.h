#ifndef DELETELAYERCOMMAND_H
#define DELETELAYERCOMMAND_H

#include "Command.h"
#include "../Project.h"
#include <memory>
#include <optional>

class DeleteLayerCommand : public Command
{
public:
    DeleteLayerCommand(LayerLocation);

    virtual const char* name() override;

protected:
    virtual Command::Outcome up(Project&) override;
    virtual void down(Project&) override;

private:
    LayerLocation m_loc;
    std::optional<std::unique_ptr<Layer>> m_layer;
};

#endif
