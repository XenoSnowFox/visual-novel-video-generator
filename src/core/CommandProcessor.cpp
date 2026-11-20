#include <iomanip>
#include <iostream>
#include <utility>

#include "core/CommandProcessor.hpp"

core::CommandProcessor::CommandProcessor(std::string aName,
                                         std::string aSummary,
                                         CommandProcessor *aParent)
    : mName(std::move(aName)), mSummary(std::move(aSummary)), mParent(aParent) {
}

void core::CommandProcessor::addExample(CommandExample aExample) {
  mExamples.push_back(std::move(aExample));
}

void core::CommandProcessor::addSubcommand(
    std::shared_ptr<CommandProcessor> aSubcommand) {
  mSubcommands.push_back(std::move(aSubcommand));
}

/**
 * @brief Prints this parser's help text.
 *
 * The output includes this command's usage, options, and subcommands, using
 * the metadata registered on this parser.
 */
void core::CommandProcessor::printHelpDocumentation(std::ostream &oss) const {
  std::vector<std::string> command_path;
  for (const CommandProcessor *processor = this; processor != nullptr;
       processor = processor->mParent) {
    command_path.push_back(processor->mName);
  }
  std::reverse(command_path.begin(), command_path.end());

  std::vector<std::string> option_labels{"-h, --help"};
  std::size_t option_width = option_labels.front().size();

  oss << "Usage:";
  for (const std::string &command_name : command_path) {
    oss << ' ' << command_name;
  }
  oss << " [OPTIONS]";
  if (!this->mSubcommands.empty()) {
    oss << " <COMMAND>";
  }
  oss << "\n\n" << this->mSummary << "\n\nOptions:\n";

  oss << "  " << std::left << std::setw(static_cast<int>(option_width + 2))
      << option_labels.front() << "Show this help message and exit\n";

  if (!this->mSubcommands.empty()) {
    std::size_t command_width = 0;
    for (const std::shared_ptr<CommandProcessor> &commandProcessor :
         this->mSubcommands) {
      if (commandProcessor->mName.size() > command_width) {
        command_width = commandProcessor->mName.size();
      }
    }

    oss << "\nCommands:\n";
    for (const std::shared_ptr<CommandProcessor> &commandProcessor :
         this->mSubcommands) {
      oss << "  " << std::left << std::setw(static_cast<int>(command_width + 2))
          << commandProcessor->mName << commandProcessor->mSummary << '\n';
    }
  }
}