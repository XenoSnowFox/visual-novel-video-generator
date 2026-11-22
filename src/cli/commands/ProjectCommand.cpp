#include <memory>

#include "cli/commands/ProjectCommand.hpp"
#include "cli/commands/project/CreateProjectCommand.hpp"

namespace vnvg::cli::commands {

ProjectCommand::ProjectCommand(RootCommand *apParentCommand)
    : CommandProcessor("project", "", apParentCommand) {

  this->addSubcommand(std::make_shared<project::CreateProjectCommand>(this));
}
} // namespace vnvg::cli::commands
