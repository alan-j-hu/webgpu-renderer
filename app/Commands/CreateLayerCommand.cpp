#include "CreateLayerCommand.h"

CreateLayerCommand::CreateLayerCommand(LayerLocation loc)
    : m_loc(loc)
{
}

Command::Outcome CreateLayerCommand::up(Project& project)
{
    project.level_at(m_loc).add_layer();
    return Outcome::COMPLETED;
}

void CreateLayerCommand::down(Project& project)
{
    project
        .level_at(m_loc)
        .remove_layer(project.level_at(m_loc).layer_count() - 1);
}

const char* CreateLayerCommand::name()
{
    return "Create Layer";
}
