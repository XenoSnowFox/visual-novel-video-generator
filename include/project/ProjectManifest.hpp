#pragma once

#include <iostream>
#include <string>

namespace vnvg::project {
/**
 * @brief Stores the metadata encoded in a project manifest.
 *
 * Project names are limited to 200 bytes so they fit within the manifest's
 * one-byte length field and satisfy the project name limit.
 */
class ProjectManifest {
public:
  /**
   * @brief Constructs an empty project manifest.
   */
  ProjectManifest();

  /**
   * @brief Reads a project manifest from a binary input stream.
   *
   * @param arInputStream Stream positioned at the start of a manifest.
   * @throws std::runtime_error When the signature, version, or required
   * manifest data is invalid.
   */
  ProjectManifest(std::istream &arInputStream);

  /**
   * @brief Sets the name stored in the manifest.
   *
   * @param aProjectName Project name containing at most 200 bytes.
   * @throws std::length_error When the name cannot fit in the manifest length
   * field.
   */
  void set_project_name(std::string aProjectName);

  /**
   * @brief Gets the name stored in the manifest.
   *
   * This function is defined inline so its body can remain in the header
   * without producing multiple-definition linker errors. The inline keyword
   * does not guarantee that the compiler will inline the function call.
   *
   * @return A read-only reference to the project name.
   */
  inline const std::string &get_project_name() const {
    return this->mProjectName;
  }

  /**
   * @brief Writes a project manifest to a binary output stream.
   *
   * @param arOutputStream Destination stream.
   * @param arManifest Manifest to serialize.
   * @return The destination stream.
   */
  friend std::ostream &operator<<(std::ostream &arOutputStream,
                                  const ProjectManifest &arManifest);

private:
  /**
   * @brief Name of the project represented by this manifest.
   *
   * The encoded form permits no more than 200 bytes.
   */
  std::string mProjectName = "Untitled Project";
};
} // namespace vnvg::project