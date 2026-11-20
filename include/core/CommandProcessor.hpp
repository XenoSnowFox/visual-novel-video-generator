#pragma once

#include <functional>
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
struct ArgumentParser {
  /**
   * @brief Keywords that identify the option, such as `-s` or `--something`.
   *
   * Keywords are stored exactly as provided. The structure does not enforce
   * uniqueness or a required prefix.
   */
  const std::vector<std::string> keywords;

  /**
   * @brief Name of the value expected after the option.
   *
   * A disengaged optional indicates that the option does not take a value.
   */
  const std::optional<std::string> argument_name;

  /**
   * @brief Short description shown in help and generated documentation.
   *
   * The structure does not impose a length or formatting requirement.
   */
  const std::string summary;

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

protected:
  /**
   * @brief Adds an example to the command's generated documentation.
   *
   * @param aExample Example to append. It is moved into the collection in
   * arrival order without validation or deduplication.
   */
  void addExample(CommandExample aExample);

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
   * @brief Examples registered for the command's generated documentation.
   *
   * Examples are retained in arrival order. The collection does not enforce
   * uniqueness and is populated through addExample().
   */
  std::vector<CommandExample> mExamples;
};
} // namespace core
