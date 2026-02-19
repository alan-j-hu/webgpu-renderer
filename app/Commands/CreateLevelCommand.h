#ifndef CREATE_LEVEL_COMMAND_H
#define CREATE_LEVEL_COMMAND_H

#include "Command.h"
#include "../Project.h"
#include <memory>

class CreateLevelCommand : public Command
{
public:
    CreateLevelCommand(World&, int, int);

    virtual const char* name() override;

protected:
    virtual std::expected<Outcome, std::string> up(Project&) override;
    virtual void down(Project&) override;

private:
    World* m_world;
    std::unique_ptr<Level> m_level;
    int m_x;
    int m_y;
};

#endif
