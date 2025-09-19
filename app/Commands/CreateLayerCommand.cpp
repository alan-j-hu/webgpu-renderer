#include "CreateLayerCommand.h"

CreateLayerCommand::CreateLayerCommand()
{
}

Command::Outcome CreateLayerCommand::up(Project& project)
{
    project.add_layer();
    return Outcome::COMPLETED;
}

void CreateLayerCommand::down(Project& project)
{
    project.remove_layer(project.layer_count() - 1);
}

const char* CreateLayerCommand::name()
{
    return "Create Layer";
}
