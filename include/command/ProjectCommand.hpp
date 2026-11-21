#pragma once

#include "command/RootCommand.hpp"
#include "core/CommandProcessor.hpp"

namespace command {

/**
 * @brief Dispatches project-related subcommands.
 *
 * The command has no behavior of its own; it owns the processors registered
 * for operations performed on project files.
 */
class ProjectCommand : public core::CommandProcessor {

public:
  /**
   * @brief Constructs the project command and registers its subcommands.
   *
   * @param apParentCommand Non-owning parent command. The parent must outlive
   * this command.
   */
  ProjectCommand(RootCommand *apParentCommand);
};

} // namespace command