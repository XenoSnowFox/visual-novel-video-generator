#include <doctest/doctest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <queue>
#include <sstream>
#include <string>

#include "command/RootCommand.hpp"

SCENARIO("the project file is created when the filename is available") {
  GIVEN("an available filename") {
    const std::filesystem::path projectFile =
        std::filesystem::temp_directory_path() /
        ("visual-novel-video-generator-" +
         std::to_string(std::chrono::steady_clock::now()
                            .time_since_epoch()
                            .count()) +
         ".vnv");

    CAPTURE(projectFile);

    command::RootCommand command("visual-novel-video-generator");
    std::queue<std::string> arguments;
    arguments.push("project");
    arguments.push("create");
    arguments.push("test-project");
    arguments.push(projectFile.string());

    WHEN("the create project command is run") {
      const int result = command.process(arguments);

      THEN("the command succeeds and creates the file") {
        CHECK(result == 0);
        CHECK(std::filesystem::is_regular_file(projectFile));
      }

      std::filesystem::remove(projectFile);
    }
  }
}

SCENARIO("an existing project file is not overridden") {
  GIVEN("an existing file at the requested filename") {
    const std::filesystem::path projectFile =
        std::filesystem::temp_directory_path() /
        ("visual-novel-video-generator-" +
         std::to_string(std::chrono::steady_clock::now()
                            .time_since_epoch()
                            .count()) +
         ".vnv");
    const std::string originalContent = "existing project content";
    {
      std::ofstream existingFile(projectFile, std::ios::binary);
      existingFile << originalContent;
    }

    command::RootCommand command("visual-novel-video-generator");
    std::queue<std::string> arguments;
    arguments.push("project");
    arguments.push("create");
    arguments.push("test-project");
    arguments.push(projectFile.string());

    CAPTURE(projectFile);

    WHEN("the create project command is run") {
      std::ostringstream errorOutput;
      std::streambuf *const originalBuffer =
          std::cerr.rdbuf(errorOutput.rdbuf());
      const int result = command.process(arguments);
      std::cerr.rdbuf(originalBuffer);

      std::string actualContent;
      {
        std::ifstream existingFile(projectFile, std::ios::binary);
        actualContent.assign(std::istreambuf_iterator<char>(existingFile),
                             std::istreambuf_iterator<char>());
      }

      THEN("the command fails and preserves the existing contents") {
        CHECK(result == 1);
        CHECK(actualContent == originalContent);
      }

      std::filesystem::remove(projectFile);
    }
  }
}
