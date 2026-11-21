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
 * @brief Describes a command-line option and how to process it.
 *
 * Instances store parser configuration only; they do not match keywords,
 * consume command-line tokens, or invoke the callback by themselves.
 */
struct CommandArgument {
  /**
   * @brief Keywords that identify the option, such as `-s` or `--something`.
   *
   * Keywords are stored exactly as provided. The structure does not enforce
   * uniqueness or a required prefix.
   */
  const std::vector<std::string> keywords;

  /**
   * @brief Short description shown in help and generated documentation.
   *
   * The structure does not impose a length or formatting requirement.
   */
  const std::string summary;

  /**
   * @brief Name of the value expected after the option.
   *
   * A disengaged optional indicates that the option does not take a value.
   */
  const std::optional<std::string> argument_name;

  /**
   * @brief Callback invoked by a consumer after a keyword is matched.
   *
   * The optional value is expected to contain the token parsed as the
   * option's argument. It is disengaged when argument_name is disengaged or
   * when the consumer has no argument to provide. The structure does not
   * enforce either convention.
   */
  const std::function<void(std::optional<std::string>)> callback;
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
 * Parsing behavior is not implemented by this class at present; it currently
 * only retains the supplied command metadata.
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
   * @param args Queue of command-line tokens to consume.
   * @return The command's execution result, `0` when help is shown, or `1` for
   * an unrecognized token.
   */
  virtual int process(std::queue<std::string> &args) const;

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
   * without validation or deduplication. The command does not invoke the
   * option's callback; consumers are responsible for parsing and processing
   * registered arguments.
   */
  void addArgument(std::shared_ptr<CommandArgument> aArgument);

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
   * @brief Executes this command after options and positional arguments parse.
   * @param aPositionalArgument Parsed positional argument, if provided.
   * @return The command result; by default, delegates to executeCommand().
   */
  virtual int
  executeCommand(const std::optional<std::string> &aPositionalArgument) const {
    static_cast<void>(aPositionalArgument);
    return executeCommand();
  }

  /**
   * @brief Executes this command after its options have been parsed.
   *
   * Override this method in a derived parser to provide command behavior.
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
  std::vector<std::shared_ptr<CommandArgument>> mArguments;

  /**
   * @brief Processors for subcommands nested under this command.
   */
  std::vector<std::shared_ptr<CommandProcessor>> mSubcommands;

  /**
   * @brief Examples registered for the command's generated documentation.
   *
   * Examples are retained in arrival order. The collection does not enforce
   * uniqueness and is populated through addExample().
   */
  std::vector<CommandExample> mExamples;

  /**
   * @brief Handles a help keyword by printing help for this parser.
   * @param keyword Token being checked for a help option.
   * @param arArgs Remaining tokens; unused because parsing stops after help is
   * printed.
   * @return `true` when help was printed; otherwise `false`.
   */
  bool dispatchHelpDocumentation(
      const std::string &keyword,
      [[maybe_unused]] std::queue<std::string> &arArgs) const;

  /**
   * @brief Dispatches a keyword to a matching argument callback.
   * @param keyword Keyword to match against the registered arguments.
   * @param args Remaining command-line tokens passed to the callback.
   * @return `true` if a matching argument callback was invoked; otherwise
   * `false`.
   */
  bool dispatchArgumentParsers(const std::string &keyword,
                               std::queue<std::string> &args) const;

  /**
   * @brief Dispatches a command name to its matching subcommand parser.
   * @param keyword Command name to match against the registered subcommands.
   * @param args Remaining command-line tokens passed to the subcommand parser.
   * @return The subcommand's result code if matched; otherwise `std::nullopt`.
   */
  std::optional<int>
  dispatchCommandParsers(const std::string &keyword,
                         std::queue<std::string> &args) const;
};
} // namespace core
