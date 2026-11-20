#include <filesystem>
#include <iostream>
#include <queue>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include "command/RootCommand.hpp"

int main(int argc, char *argv[]) {
#ifdef _WIN32
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
  command::RootCommand rootCommand(programExecutableName);
  rootCommand.printHelpDocumentation(std::cout);

  // done
  return 0;
}