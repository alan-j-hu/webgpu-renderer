#include "CreateLayerCommand.h"

CreateLayerCommand::CreateLayerCommand()
{
}

void CreateLayerCommand::up(Project& project)
{
    project.add_layer();
}

void CreateLayerCommand::down(Project& project)
{
    project.remove_layer(project.layer_count() - 1);
}

const char* CreateLayerCommand::name()
{
    return "Create Layer";
}
