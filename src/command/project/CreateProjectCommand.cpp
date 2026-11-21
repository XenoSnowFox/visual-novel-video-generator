#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

#include "command/project/CreateProjectCommand.hpp"
#include "core/ZipArchive.hpp"

command::project::CreateProjectCommand::CreateProjectCommand(
    ProjectCommand *apParentCommand)
    : CommandProcessor("create", "", apParentCommand) {

  this->addPositionalArgument(
      std::make_shared<core::CommandArgument>(core::CommandArgument{
          .argumentName = "PROJECT_NAME",
          .callback =
              [this](std::optional<std::string> aArg) {
                this->mProjectName = aArg.value();
              },
      }));

  this->addPositionalArgument(
      std::make_shared<core::CommandArgument>(core::CommandArgument{
          .argumentName = "FILENAME",
          .callback =
              [this](std::optional<std::string> aArg) {
                this->mFilename = aArg.value();
              },
      }));
}

int command::project::CreateProjectCommand::executeCommand() const {
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

  std::cout << "Project '" << this->mProjectName.value() << "' was created at '"
            << projectPath.string() << "'.\n";

  return 0;
}
