#include <algorithm>
#include <iomanip>
#include <utility>

#include "core/CommandProcessor.hpp"

namespace {
/**
 * @brief Checks whether a command-line keyword requests help.
 *
 * Matching is exact and case-sensitive; only `-h` and `--help` are recognized.
 *
 * @param aKeyword The command-line keyword to inspect.
 * @return `true` when the keyword is `-h` or `--help`; otherwise `false`.
 */
inline bool isHelpKeyword(const std::string &aKeyword) {
  return aKeyword == "-h" || aKeyword == "--help";
}
} // namespace

namespace core {

// -----------------------------------------------------------------------------
// Construction and registration
// -----------------------------------------------------------------------------

CommandProcessor::CommandProcessor(std::string aName, std::string aSummary,
                                   CommandProcessor *aParent)
    : mName(std::move(aName)), mSummary(std::move(aSummary)), mParent(aParent) {
}

void CommandProcessor::addExample(CommandExample aExample) {
  mExamples.push_back(std::move(aExample));
}

void CommandProcessor::addSubcommand(
    std::shared_ptr<CommandProcessor> aSubcommand) {
  mSubcommands.push_back(std::move(aSubcommand));
}

void CommandProcessor::addArgument(std::shared_ptr<CommandArgument> aArgument) {
  mArguments.push_back(std::move(aArgument));
}

// -----------------------------------------------------------------------------
// Command processing
// -----------------------------------------------------------------------------

int CommandProcessor::process(std::queue<std::string> &args) const {
  std::optional<std::string> positional_argument;

  // iterate over the remaining arguments in the queue by popping the nextitem
  while (!args.empty()) {
    // remove the next keyword
    const std::string keyword = args.front();
    args.pop();

    if (this->dispatchHelpDocumentation(keyword, args)) {
      return 0;
    }

    if (this->dispatchArgumentParsers(keyword, args)) {
      continue;
    }

    if (const std::optional<int> result =
            this->dispatchCommandParsers(keyword, args)) {
      return *result;
    }

    std::cerr << "Error: unrecognized command or option '" << keyword
              << "' for '" << this->mName << "'.\n";
    return 1;
  }

  return executeCommand(positional_argument);
};

// -----------------------------------------------------------------------------
// Help documentation
// -----------------------------------------------------------------------------

void CommandProcessor::printHelpDocumentation(std::ostream &oss) const {
  std::vector<std::string> command_path;
  for (const CommandProcessor *processor = this; processor != nullptr;
       processor = processor->mParent) {
    command_path.push_back(processor->mName);
  }
  std::reverse(command_path.begin(), command_path.end());

  std::vector<std::string> option_labels{"-h, --help"};
  std::size_t option_width = option_labels.front().size();
  for (const std::shared_ptr<CommandArgument> &argument : this->mArguments) {
    std::string label;
    for (std::size_t i = 0; i < argument->keywords.size(); ++i) {
      if (i > 0) {
        label += ", ";
      }
      label += argument->keywords[i];
    }
    if (argument->argumentName) {
      label += " <";
      label += *argument->argumentName;
      label += ">";
    }
    if (label.size() > option_width) {
      option_width = label.size();
    }
    option_labels.push_back(std::move(label));
  }

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
  for (std::size_t i = 0; i < this->mArguments.size(); ++i) {
    oss << "  " << std::left << std::setw(static_cast<int>(option_width + 2))
        << option_labels[i + 1] << this->mArguments[i]->summary << '\n';
  }

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

// -----------------------------------------------------------------------------
// Dispatch helpers
// -----------------------------------------------------------------------------

bool CommandProcessor::dispatchHelpDocumentation(
    const std::string &keyword,
    [[maybe_unused]] std::queue<std::string> &arArgs) const {
  if (::isHelpKeyword(keyword)) {
    this->printHelpDocumentation(std::cout);
    return true;
  }
  return false;
}

bool CommandProcessor::dispatchArgumentParsers(
    const std::string &keyword, std::queue<std::string> &args) const {
  // loop over the argument parsers
  for (const std::shared_ptr<CommandArgument> &parser : this->mArguments) {
    // if we don't match on the keywords then move to the next argument
    // parser
    if (std::find(parser->keywords.begin(), parser->keywords.end(), keyword) !=
        parser->keywords.end()) {
      std::optional<std::string> argument;
      if (parser->argumentName && !args.empty()) {
        argument = args.front();
        args.pop();
      }
      parser->callback(std::move(argument));
      return true;
    }
  }

  // we found no argument matches
  return false;
}

std::optional<int>
CommandProcessor::dispatchCommandParsers(const std::string &keyword,
                                         std::queue<std::string> &args) const {

  for (const std::shared_ptr<CommandProcessor> &parser : this->mSubcommands) {
    if (keyword == parser->mName) {
      return parser->process(args);
    }
  }

  // we found no command matches
  return std::nullopt;
}

} // namespace core
