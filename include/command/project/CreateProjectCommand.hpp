#pragma once

#include <optional>
#include <string>

#include "command/ProjectCommand.hpp"
#include "core/CommandProcessor.hpp"

namespace command::project {

/**
 * @brief Creates a project archive containing its manifest at a requested path.
 *
 * The command accepts a project name and destination filename. Existing
 * destination files are preserved. The only project entry currently written
 * is the serialized manifest.
 */
class CreateProjectCommand : public core::CommandProcessor {

public:
  /**
   * @brief Constructs the create-project command and its positional arguments.
   *
   * @param apParentCommand Non-owning parent command. The parent must outlive
   * this command.
   */
  CreateProjectCommand(ProjectCommand *apParentCommand);

protected:
  /**
   * @brief Creates the project archive requested by the parsed arguments.
   *
   * @return `0` after creating the archive and writing its manifest, or `1`
   * when the destination exists or either operation cannot be completed.
   */
  int executeCommand() const override;

private:
  /**
   * @brief Parsed project name used in the success message.
   *
   * The value is populated through the registered positional-argument
   * callback before command execution.
   */
  std::optional<std::string> mProjectName;

  /**
   * @brief Parsed filesystem path for the new project archive.
   *
   * The value is populated through the registered positional-argument
   * callback before command execution.
   */
  std::optional<std::string> mFilename;
};

} // namespace command::project