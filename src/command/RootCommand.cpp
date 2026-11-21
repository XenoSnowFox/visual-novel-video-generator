#include <memory>

#include "command/RootCommand.hpp"

command::RootCommand::RootCommand(std::string aProgramExectableName)
    : CommandProcessor(aProgramExectableName, "", nullptr) {}
