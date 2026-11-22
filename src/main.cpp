#include <filesystem>
#include <queue>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include "cli/commands/RootCommand.hpp"

int main(int argc, char *argv[]) {
#ifdef _WIN32
  // Enable UTF-8 Console output of windows
  SetConsoleOutputCP(CP_UTF8);
#endif

  // create a new argument queue
  std::queue<std::string> arguments;
  for (int i = 1; i < argc; ++i) {
    arguments.emplace(argv[i]);
  }

  // create a new root command processor
  const std::string programExecutableName =
      std::filesystem::path(argv[0]).filename().string();
  vnvg::cli::commands::RootCommand rootCommand(programExecutableName);

  // execute the command
  return rootCommand.process(arguments);
}