#ifndef DELETE_LEVEL_COMMAND_H
#define DELETE_LEVEL_COMMAND_H

#include "Command.h"
#include "../Project.h"
#include <memory>
#include <optional>

class DeleteLevelCommand : public Command
{
public:
    DeleteLevelCommand(World& world, Level& level);

    virtual const char* name() override;

protected:
    virtual std::expected<Outcome, std::string> up(Project&) override;
    virtual void down(Project&) override;

private:
    World& m_world;
    // Other commands in the undo/redo stack can reference the level.
    Level& m_level_ref;
    std::unique_ptr<Level> m_level_ptr;
};

#endif
