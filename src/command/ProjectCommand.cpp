#include <memory>

#include "command/ProjectCommand.hpp"
#include "command/project/CreateProjectCommand.hpp"

command::ProjectCommand::ProjectCommand(RootCommand *apParentCommand)
    : CommandProcessor("project", "", apParentCommand) {

  this->addSubcommand(std::make_shared<project::CreateProjectCommand>(this));
}
