#include <memory>

#include "command/ProjectCommand.hpp"
#include "command/RootCommand.hpp"

command::RootCommand::RootCommand(std::string aProgramExectableName)
    : CommandProcessor(aProgramExectableName, "", nullptr) {
  // Register the PROJECT command
  this->addSubcommand(std::make_shared<command::ProjectCommand>(this));
}
