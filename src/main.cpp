#include <iostream>
#include <queue>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[]) {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
#endif

  // create a new argument queue
  std::queue<std::string> arguments;
  for (int i = 1; i < argc; ++i) {
    arguments.emplace(argv[i]);
  }

  std::cout << "Hello World." << std::endl;
  while (!arguments.empty()) {
    std::cout << arguments.front() << std::endl;
    arguments.pop();
  }

  // done
  return 0;
}