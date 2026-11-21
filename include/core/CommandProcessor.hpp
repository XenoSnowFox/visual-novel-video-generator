#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <vector>

namespace core {

/**
 * @brief Describes a command-line value and how to process it.
 *
 * Instances store parser configuration only and can describe either a
 * positional argument or the value accepted by an option.
 */
struct CommandArgument {
  /**
   * @brief Short description shown in help and generated documentation.
   *
   * The structure does not impose a length or formatting requirement.
   */
  const std::string summary;

  /**
   * @brief Name displayed for the accepted value.
   *
   * A disengaged optional indicates that an option does not take a value.
   * Positional arguments require an engaged value name.
   */
  const std::optional<std::string> argumentName;

  /**
   * @brief Callback invoked after an option or positional argument is parsed.
   *
   * The optional value is expected to contain the token parsed as the
   * option's argument. It is disengaged when argumentName is disengaged or
   * when the consumer has no argument to provide. The structure does not
   * enforce either convention.
   */
  const std::function<void(std::optional<std::string>)> callback;
};

/**
 * @brief Describes a keyword option and the value it accepts.
 *
 * The keywords identify the option while the inherited metadata describes its
 * accepted value and callback.
 */
struct CommandOptionArgument : public CommandArgument {
  /**
   * @brief Keywords that identify the option, such as `-s` or `--something`.
   *
   * Keywords are stored exactly as provided. The structure does not enforce
   * uniqueness or a required prefix.
   */
  const std::vector<std::string> keywords;
};

/**
 * @brief Describes a command-line example for generated documentation.
 */
struct CommandExample {
  /**
   * @brief Explanation displayed with the example.
   *
   * The description is stored without formatting or length validation.
   */
  const std::string description;

  /**
   * @brief Command line shown as the example.
   *
   * This text is documentation only and is not parsed or executed by
   * CommandExample.
   */
  const std::string command;
};

/**
 * @brief Stores the identity and help summary for a command-line command.
 *
 * The processor also dispatches registered options and subcommands, retaining
 * unmatched tokens as positional arguments.
 */
class CommandProcessor {
public:
  /**
   * @brief Constructs command metadata from a name and summary.
   *
   * @param aName Name used to identify the command. It is stored as provided,
   * without validation.
   * @param aSummary Short description of the command. It is stored as
   * provided, without formatting or length validation.
   * @param aParent Non-owning parent command, or `nullptr` when this command
   * is the root. The pointed-to command must outlive this command; the
   * constructor does not take ownership of it or validate the hierarchy.
   */
  CommandProcessor(std::string aName, std::string aSummary,
                   CommandProcessor *aParent);

  /**
   * @brief Parses queued tokens and dispatches options or subcommands.
   *
   * Matched tokens are removed from the queue. Option callbacks receive their
   * optional value; if no subcommand is selected, this command's execution
   * hook runs after parsing completes. Help options print help and return `0`.
   * Positional arguments are assigned to their registered definitions in
   * arrival order. Tokens are still matched as options and subcommands first.
   * If a subcommand follows positional arguments, the current command executes
   * before the subcommand.
   *
   * @param arArgs Queue of command-line tokens to consume.
   * @return The command's execution result, or `0` when help is shown.
   */
  virtual int process(std::queue<std::string> &arArgs) const;

  /**
   * @brief Prints this commands's help text.
   *
   * The output includes this command's usage, options, and subcommands, using
   * the metadata registered on this parser.
   */
  virtual void printHelpDocumentation(std::ostream &oss) const;

protected:
  /**
   * @brief Adds an example to the command's generated documentation.
   *
   * @param aExample Example to append. It is moved into the collection in
   * arrival order without validation or deduplication.
   */
  void addExample(CommandExample aExample);

  /**
   * @brief Registers an option with the command.
   *
   * @param aArgument Option to append. Ownership is retained through the shared
   * pointer, and the option is moved into the collection in arrival order
   * without validation or deduplication.
   */
  void addArgument(std::shared_ptr<CommandOptionArgument> aArgument);

  /**
   * @brief Registers a subcommand.
   *
   * @param aSubcommand Processor to append in arrival order. Ownership is
   * retained through the shared pointer, and the processor is moved into the
   * collection without validation or deduplication. This method does not
   * establish or modify the subcommand's parent relationship.
   */
  void addSubcommand(std::shared_ptr<CommandProcessor> aSubcommand);

  /**
   * @brief Registers a required positional argument for this command.
   *
   * Positional arguments are consumed in registration order and each adds one
   * expected command-line token.
   *
   * @param aArgument Positional argument to append. Ownership is retained
   * through the shared pointer, without validation or deduplication.
   */
  void addPositionalArgument(std::shared_ptr<CommandArgument> aArgument);

  /**
   * @brief Executes this command after its options and positionals are parsed.
   *
   * Override this method in a derived parser to provide command behavior.
   * Option and positional values are available to the derived parser through
   * their registered callbacks.
   * @return Command result code; the default implementation returns `1`.
   */
  virtual int executeCommand() const {
    std::cerr << "Error: no action is defined for command '" << this->mName
              << "'.\n";
    return 1;
  }

private:
  /**
   * @brief Command name recognized by this parser.
   *
   * This value is fixed after construction and is not exposed by an accessor.
   */
  const std::string mName;

  /**
   * @brief Short description used in help and generated documentation.
   *
   * This value is fixed after construction and is not exposed by an accessor.
   */
  const std::string mSummary;

  /**
   * @brief Non-owning pointer to the parent parser, or `nullptr` for the root.
   */
  CommandProcessor *const mParent;

  /**
   * @brief Options recognized by this command.
   */
  std::vector<std::shared_ptr<CommandOptionArgument>> mArguments;

  /**
   * @brief Processors for subcommands nested under this command.
   */
  std::vector<std::shared_ptr<CommandProcessor>> mSubcommands;

  /**
   * @brief Required positional arguments in command-line order.
   */
  std::vector<std::shared_ptr<CommandArgument>> mPositionalArguments;

  /**
   * @brief Examples registered for the command's generated documentation.
   *
   * Examples are retained in arrival order. The collection does not enforce
   * uniqueness and is populated through addExample().
   */
  std::vector<CommandExample> mExamples;

  /**
   * @brief Handles a help keyword by printing help for this parser.
   * @param aKeyword Token being checked for a help option.
   * @param arArgs Remaining tokens; unused because parsing stops after help is
   * printed.
   * @return `true` when help was printed; otherwise `false`.
   */
  bool dispatchHelpDocumentation(
      const std::string &aKeyword,
      [[maybe_unused]] std::queue<std::string> &arArgs) const;

  /**
   * @brief Dispatches a keyword to a matching argument callback.
   * @param aKeyword Keyword to match against the registered arguments.
   * @param arArgs Remaining command-line tokens passed to the callback.
   * @return `true` if a matching argument callback was invoked; otherwise
   * `false`.
   */
  bool dispatchArgumentParsers(const std::string &aKeyword,
                               std::queue<std::string> &arArgs) const;

  /**
   * @brief Prints all unparsed positional arguments and their descriptions.
   *
   * Reporting starts at the first positional definition that has not consumed
   * a command-line token.
   *
   * @param aParsedArgumentCount Number of positional arguments already parsed.
   * @return Nothing.
   */
  void printMissingArguments(std::size_t aParsedArgumentCount) const;

};
} // namespace core
