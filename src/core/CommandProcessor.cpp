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

void CommandProcessor::addArgument(
    std::shared_ptr<CommandOptionArgument> aArgument) {
  mArguments.push_back(std::move(aArgument));
}

void CommandProcessor::addPositionalArgument(
    std::shared_ptr<CommandArgument> aArgument) {
  mPositionalArguments.push_back(std::move(aArgument));
}

// -----------------------------------------------------------------------------
// Command processing
// -----------------------------------------------------------------------------

int CommandProcessor::process(std::queue<std::string> &arArgs) const {
  std::optional<std::vector<std::string>> positionalArguments;

  while (!arArgs.empty()) {
    const std::string keyword = arArgs.front();
    arArgs.pop();

    if (this->dispatchHelpDocumentation(keyword, arArgs)) {
      return 0;
    }

    if (this->dispatchArgumentParsers(keyword, arArgs)) {
      continue;
    }

    for (const std::shared_ptr<CommandProcessor> &parser : this->mSubcommands) {
      if (keyword == parser->mName) {
        if (positionalArguments &&
            positionalArguments->size() != mPositionalArguments.size()) {
          this->printMissingArguments(positionalArguments->size());
          return 1;
        }
        if (positionalArguments) {
          const int result = this->executeCommand();
          if (result != 0) {
            return result;
          }
        }
        return parser->process(arArgs);
      }
    }

    if (!positionalArguments) {
      positionalArguments.emplace();
    }
    if (positionalArguments->size() >= mPositionalArguments.size()) {
      std::cerr << "Error: unrecognized command or option '" << keyword
                << "' for '" << this->mName << "'.\n";
      return 1;
    }

    mPositionalArguments[positionalArguments->size()]->callback(
        std::optional<std::string>(keyword));
    positionalArguments->push_back(keyword);
  }

  if (positionalArguments &&
      positionalArguments->size() != mPositionalArguments.size()) {
    this->printMissingArguments(positionalArguments->size());
    return 1;
  }
  if (!positionalArguments && !mPositionalArguments.empty()) {
    this->printMissingArguments(0);
    return 1;
  }

  return executeCommand();
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
  for (const std::shared_ptr<CommandOptionArgument> &argument :
       this->mArguments) {
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
  for (const std::shared_ptr<CommandArgument> &argument :
       this->mPositionalArguments) {
    oss << " <" << *argument->argumentName << ">";
  }
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

  if (!this->mPositionalArguments.empty()) {
    std::size_t argument_width = 0;
    for (const std::shared_ptr<CommandArgument> &argument :
         this->mPositionalArguments) {
      if (argument->argumentName->size() > argument_width) {
        argument_width = argument->argumentName->size();
      }
    }

    oss << "\nArguments:\n";
    for (const std::shared_ptr<CommandArgument> &argument :
         this->mPositionalArguments) {
      oss << "  " << std::left
          << std::setw(static_cast<int>(argument_width + 2))
          << *argument->argumentName << argument->summary << '\n';
    }
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
    const std::string &aKeyword,
    [[maybe_unused]] std::queue<std::string> &arArgs) const {
  if (::isHelpKeyword(aKeyword)) {
    this->printHelpDocumentation(std::cout);
    return true;
  }
  return false;
}

bool CommandProcessor::dispatchArgumentParsers(
    const std::string &aKeyword, std::queue<std::string> &arArgs) const {
  // loop over the argument parsers
  for (const std::shared_ptr<CommandOptionArgument> &parser :
       this->mArguments) {
    // if we don't match on the keywords then move to the next argument
    // parser
    if (std::find(parser->keywords.begin(), parser->keywords.end(), aKeyword) !=
        parser->keywords.end()) {
      std::optional<std::string> argument;
      if (parser->argumentName && !arArgs.empty()) {
        argument = arArgs.front();
        arArgs.pop();
      }
      parser->callback(std::move(argument));
      return true;
    }
  }

  // we found no argument matches
  return false;
}

void CommandProcessor::printMissingArguments(
    const std::size_t aParsedArgumentCount) const {
  std::cerr << "Error: missing arguments for '" << this->mName << "':\n";
  for (std::size_t i = aParsedArgumentCount; i < mPositionalArguments.size();
       ++i) {
    const std::shared_ptr<CommandArgument> &argument =
        mPositionalArguments[i];
    std::cerr << "  <" << *argument->argumentName << "> - " << argument->summary
              << '\n';
  }
}

} // namespace core
