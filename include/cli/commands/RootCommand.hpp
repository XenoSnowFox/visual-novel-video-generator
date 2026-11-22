#pragma once

#include <string>

#include "cli/CommandProcessor.hpp"

namespace vnvg::cli::commands {

/**
 * @brief Root command processor for the application.
 *
 * This processor represents the program itself and therefore has no parent.
 */
class RootCommand : public vnvg::cli::CommandProcessor {

public:
  /**
   * @brief Constructs the application's root command.
   *
   * @param aProgramExectableName Name of the program executable. It is used as
   * the root command name in generated usage documentation and is stored
   * without validation.
   */
  RootCommand(std::string aProgramExectableName);
};

} // namespace vnvg::cli::commands