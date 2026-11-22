#pragma once

#include "cli/CommandProcessor.hpp"
#include "cli/commands/RootCommand.hpp"

namespace vnvg::cli::commands {

/**
 * @brief Dispatches project-related subcommands.
 *
 * The command has no behavior of its own; it owns the processors registered
 * for operations performed on project files.
 */
class ProjectCommand : public vnvg::cli::CommandProcessor {

public:
  /**
   * @brief Constructs the project command and registers its subcommands.
   *
   * @param apParentCommand Non-owning parent command. The parent must outlive
   * this command.
   */
  ProjectCommand(RootCommand *apParentCommand);
};

} // namespace vnvg::cli::commands