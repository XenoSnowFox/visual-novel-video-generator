#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>

#include "cli/commands/project/CreateProjectCommand.hpp"
#include "project/ProjectManifest.hpp"
#include "storage/ZipArchive.hpp"

namespace vnvg::cli::commands::project {

CreateProjectCommand::CreateProjectCommand(ProjectCommand *apParentCommand)
    : CommandProcessor("create", "", apParentCommand) {

  this->addPositionalArgument(std::make_shared<CommandArgument>(CommandArgument{
      .argumentName = "PROJECT_NAME",
      .callback =
          [this](std::optional<std::string> aArg) {
            this->mProjectName = aArg.value();
          },
  }));

  this->addPositionalArgument(std::make_shared<CommandArgument>(CommandArgument{
      .argumentName = "FILENAME",
      .callback =
          [this](std::optional<std::string> aArg) {
            this->mFilename = aArg.value();
          },
  }));
}

int CreateProjectCommand::executeCommand() const {
  const std::filesystem::path projectPath(this->mFilename.value());
  storage::ZipArchive projectArchive(projectPath);

  const storage::ZipArchiveError error = projectArchive.create();

  if (error == storage::ZipArchiveError::ARCHIVE_ALREADY_EXISTS) {
    std::cerr << "Error: file '" << projectPath.string()
              << "' already exists.\n";
    return 1;
  }
  if (error != storage::ZipArchiveError::NONE) {
    std::cerr << "Error: could not create project file '"
              << projectPath.string() << "'.\n";
    return 1;
  }

  vnvg::project::ProjectManifest projectManifest;
  projectManifest.setProjectName(this->mProjectName.value());
  std::ostringstream manifestStream(std::ios::binary);
  manifestStream << projectManifest;
  const std::string manifestData = manifestStream.str();

  const storage::ZipArchiveError manifestWriteError =
      projectArchive.writeEntry("manifest", manifestData);
  if (manifestWriteError != storage::ZipArchiveError::NONE) {
    std::cerr << "Error: could not write the project manifest to '"
              << projectPath.string() << "'.\n";
    return 1;
  }

  std::cout << "Project '" << this->mProjectName.value() << "' was created at '"
            << projectPath.string() << "'.\n";

  return 0;
}
} // namespace vnvg::cli::commands::project
