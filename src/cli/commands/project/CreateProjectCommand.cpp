#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>

#include "cli/commands/project/CreateProjectCommand.hpp"
#include "core/ZipArchive.hpp"
#include "project/ProjectMenifest.hpp"

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
  core::ZipArchive projectArchive(projectPath);

  const core::ZipArchiveError error = projectArchive.create();

  if (error == core::ZipArchiveError::ARCHIVE_ALREADY_EXISTS) {
    std::cerr << "Error: file '" << projectPath.string()
              << "' already exists.\n";
    return 1;
  }
  if (error != core::ZipArchiveError::NONE) {
    std::cerr << "Error: could not create project file '"
              << projectPath.string() << "'.\n";
    return 1;
  }

  ::project::ProjectManifest projectManifest;
  projectManifest.set_project_name(this->mProjectName.value());
  std::ostringstream manifestStream(std::ios::binary);
  manifestStream << projectManifest;
  const std::string manifestData = manifestStream.str();

  const core::ZipArchiveError manifestWriteError =
      projectArchive.writeEntry("manifest", manifestData);
  if (manifestWriteError != core::ZipArchiveError::NONE) {
    std::cerr << "Error: could not write the project manifest to '"
              << projectPath.string() << "'.\n";
    return 1;
  }

  std::cout << "Project '" << this->mProjectName.value() << "' was created at '"
            << projectPath.string() << "'.\n";

  return 0;
}
} // namespace vnvg::cli::commands::project
