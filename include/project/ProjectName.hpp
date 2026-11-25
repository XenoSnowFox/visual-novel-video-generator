#pragma once

#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "String.hpp"

namespace vnvg::project {

/**
 * @brief Reports the result of a ZIP archive operation.
 */
enum ProjectNameError {
  /**
   * @brief The operation completed successfully.
   */
  NONE = 0,

  /**
   * @brief An unknown or unexpected operation error occurred.
   */
  UNKNOWN,

  /**
   * @brief The process failed to gain an exclusive lock on the archive file.
   */
  LENGTH_TOO_LONG,
};

class ProjectName {
public:
  /**
   * @brief Constructs a project name using the string-taking constructor.
   *
   * The argument must be a non-const lvalue. The constructor currently ignores
   * it, so the returned project name retains its default value.
   *
   * @param arProjectName String passed to the constructor.
   * @return A project name with the default value.
   *
   * @par Example
   * @code{.cpp}
   * std::string value = "My Project";
   * ProjectName projectName = ProjectName::of(value);
   * // projectName contains "Untitled Project"
   * @endcode
   */
  static inline ProjectName of(std::string &arProjectName) {
    return ProjectName(arProjectName);
  }

  /**
   * @brief Constructs a project name with the default value.
   *
   * @par Example
   * @code{.cpp}
   * ProjectName projectName;
   * std::string value = projectName;
   * // value == "Untitled Project"
   * @endcode
   */
  ProjectName() = default;

  /**
   * @brief Accepts an existing string while constructing a project name.
   *
   * The current implementation retains the default project name and does not
   * use the supplied string.
   *
   * @param arProjectName String accepted by the constructor but not currently
   * stored.
   *
   * @par Example
   * @code{.cpp}
   * std::string value = "My Project";
   * ProjectName projectName(value);
   * @endcode
   */
  ProjectName(std::string &arProjectName) { this->assign(arProjectName); }

  /**
   * @brief Assigns a project name from an lvalue string.
   *
   * @param arProjectName String copied into the project name.
   * @return A reference to this project name.
   *
   * @par Example
   * @code{.cpp}
   * std::string value = "My Project";
   * ProjectName projectName;
   * projectName = value;
   * @endcode
   */
  ProjectName &operator=(const std::string &arProjectName) {
    return this->assign(arProjectName);
  }

  /**
   * @brief Assigns a project name by moving an rvalue string.
   *
   * @param arProjectName String moved into the project name.
   * @return A reference to this project name.
   *
   * @par Example
   * @code{.cpp}
   * ProjectName projectName;
   * projectName = std::string("My Project");
   * @endcode
   */
  ProjectName &operator=(std::string &&arProjectName) noexcept {
    return this->assign(std::move(arProjectName));
  }

  /**
   * @brief Assigns a project name from a null-terminated C string.
   *
   * @param apProjectName C string copied into the project name. It must not
   * be null.
   * @return A reference to this project name.
   *
   * @par Example
   * @code{.cpp}
   * ProjectName projectName;
   * projectName = "My Project";
   * @endcode
   */
  ProjectName &operator=(const char *apProjectName) {
    return this->assign(apProjectName);
  }

  /**
   * @brief Converts an lvalue project name to a copy of its string value.
   *
   * @return A string containing the current project name.
   *
   * @par Example
   * @code{.cpp}
   * ProjectName projectName;
   * std::string value = projectName;
   * @endcode
   */
  operator std::string() const & { return mProjectName; }

  /**
   * @brief Converts an rvalue project name by moving its string value.
   *
   * The moved-from project name has a valid but unspecified value.
   *
   * @return The moved project-name string.
   *
   * @par Example
   * @code{.cpp}
   * std::string value = ProjectName{};
   * // value == "Untitled Project"
   * @endcode
   */
  operator std::string() && { return std::move(mProjectName); }

  /**
   * @brief Provides a non-owning view of the current project name.
   *
   * The returned view becomes invalid if the project name is modified or
   * destroyed.
   *
   * @return A view of the project-name string.
   *
   * @par Example
   * @code{.cpp}
   * ProjectName projectName;
   * std::string_view view = projectName;
   * @endcode
   */
  operator std::string_view() const noexcept { return mProjectName; }

  /**
   * @brief Writes the project name to an output stream.
   *
   * @param arStream Stream receiving the name.
   * @param arProjectName Project name to write.
   * @return The stream, allowing further output to be chained.
   *
   * @par Example
   * @code{.cpp}
   * ProjectName projectName;
   * projectName = "Yay";
   * std::cout << projectName; // Prints "Yay".
   * @endcode
   */
  friend std::ostream &operator<<(std::ostream &arStream,
                                  const ProjectName &arProjectName) {
    return arStream << arProjectName.mProjectName;
  }

private:
  ProjectName &assign(std::string aProjectName) {
    // trim the string
    vnvg::String s = vnvg::String(aProjectName).trim();

    // check for a max of 200 characters
    if (s.length() > 200) {
      throw std::length_error(
          "Project Name exceeds maximum length of 200 characters.");
    }

    // save it
    mProjectName = s;

    return *this;
  }

  /**
   * @brief Stored project-name text.
   */
  vnvg::String mProjectName = "Untitled Project";
};

} // namespace vnvg::project