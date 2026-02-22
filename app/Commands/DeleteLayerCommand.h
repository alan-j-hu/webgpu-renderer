#ifndef DELETELAYERCOMMAND_H
#define DELETELAYERCOMMAND_H

#include "Command.h"
#include "../Project.h"
#include <memory>
#include <optional>

class DeleteLayerCommand : public Command
{
public:
    DeleteLayerCommand(Level&, int idx);

    virtual const char* name() override;

protected:
    virtual std::expected<Outcome, std::string> up(Project&) override;
    virtual void down(Project&) override;

private:
    // SAFETY: m_level is a live reference because the underlying object
    // will either be owned by the project state or another command in the
    // undo/redo stack.
    Level& m_level;
    // Other commands in the undo/redo stack can reference the layer.
    std::unique_ptr<Layer> m_layer;
    int m_idx;
};

#endif
