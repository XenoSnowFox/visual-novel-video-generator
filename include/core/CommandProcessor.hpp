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
 * The callback receives the remaining command-line tokens after one of the
 * option's keywords is matched and removed. This structure stores parser
 * configuration only; it does not validate or process the queue by itself.
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
   * @brief Processes the next command-line token after a keyword is matched
   * with a supplied argument_name.
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
   */
  CommandProcessor(std::string aName, std::string aSummary);

protected:
  /**
   * @brief Adds an example to the command's generated documentation.
   *
   * @param aExample Example to append. It is moved into the collection in
   * arrival order without validation or deduplication.
   */
  void addExample(const CommandExample aExample);

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
   * @brief Examples registered for the command's generated documentation.
   *
   * Examples are retained in arrival order. The collection does not enforce
   * uniqueness and is populated through addExample().
   */
  std::vector<const CommandExample> mExamples;
};
} // namespace core
