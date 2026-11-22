#include <doctest/doctest.h>

#include <queue>
#include <sstream>
#include <string>

#include "cli/CommandProcessor.hpp"

namespace {
/**
 * @brief Command processor test fixture which records command execution.
 */
class HelpTestCommand final : public vnvg::cli::CommandProcessor {
public:
  /**
   * @brief Constructs a command with a stable name and summary.
   */
  HelpTestCommand()
      : CommandProcessor("help-test", "Test help output", nullptr) {}

  /**
   * @brief Number of times executeCommand() was invoked.
   */
  mutable int mExecutionCount = 0;

protected:
  /**
   * @brief Records that command execution occurred.
   *
   * @return Always returns `0`.
   */
  int executeCommand() const override {
    this->mExecutionCount++;
    return 0;
  }
};
} // namespace

SCENARIO("help arguments render documentation without executing the command") {
  GIVEN("a command processor") {
    HelpTestCommand command;

    WHEN("the command is executed with -h") {
      std::queue<std::string> arguments;
      arguments.push("-h");
      std::ostringstream output;
      std::streambuf *const originalBuffer = std::cout.rdbuf(output.rdbuf());
      const int result = command.process(arguments);
      std::cout.rdbuf(originalBuffer);

      THEN("the help documentation is rendered") {
        CHECK(output.str().find("Usage: help-test [OPTIONS]") !=
              std::string::npos);
      }

      THEN("the command exits successfully without executing") {
        CHECK(result == 0);
        CHECK(command.mExecutionCount == 0);
      }
    }

    WHEN("the command is executed with --help") {
      std::queue<std::string> arguments;
      arguments.push("--help");
      std::ostringstream output;
      std::streambuf *const originalBuffer = std::cout.rdbuf(output.rdbuf());
      const int result = command.process(arguments);
      std::cout.rdbuf(originalBuffer);

      THEN("the help documentation is rendered") {
        CHECK(output.str().find("Usage: help-test [OPTIONS]") !=
              std::string::npos);
      }

      THEN("the command exits successfully without executing") {
        CHECK(result == 0);
        CHECK(command.mExecutionCount == 0);
      }
    }
  }
}
