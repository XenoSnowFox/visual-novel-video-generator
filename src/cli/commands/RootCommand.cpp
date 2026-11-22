#include <memory>

#include "cli/commands/ProjectCommand.hpp"
#include "cli/commands/RootCommand.hpp"

namespace vnvg::cli::commands {

RootCommand::RootCommand(std::string aProgramExectableName)
    : CommandProcessor(aProgramExectableName, "", nullptr) {
  // Register the PROJECT command
  this->addSubcommand(std::make_shared<ProjectCommand>(this));
}

} // namespace vnvg::cli::commands
