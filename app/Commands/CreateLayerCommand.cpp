#include "CreateLayerCommand.h"

CreateLayerCommand::CreateLayerCommand(LayerLocation loc)
    : m_loc(loc)
{
}

Command::Outcome CreateLayerCommand::up(Project& project)
{
    project.level().add_layer();
    return Outcome::COMPLETED;
}

void CreateLayerCommand::down(Project& project)
{
    project.level().remove_layer(project.level().layer_count() - 1);
}

const char* CreateLayerCommand::name()
{
    return "Create Layer";
}
