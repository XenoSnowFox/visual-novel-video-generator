#include "project/ProjectManifest.hpp"

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace {
constexpr std::string_view kManifestSignature = "VNVG";
constexpr std::uint16_t kManifestVersion = 1;
constexpr std::size_t kMaximumProjectNameLength = 200;
} // namespace

namespace vnvg::project {

ProjectManifest::ProjectManifest() = default;

ProjectManifest::ProjectManifest(std::istream &arInputStream) {
  char signature[kManifestSignature.size()];
  if (!arInputStream.read(
          signature, static_cast<std::streamsize>(kManifestSignature.size())) ||
      std::memcmp(signature, kManifestSignature.data(),
                  kManifestSignature.size()) != 0) {
    throw std::runtime_error("invalid project manifest signature");
  }

  std::uint16_t version = 0;
  if (!arInputStream.read(reinterpret_cast<char *>(&version),
                          static_cast<std::streamsize>(sizeof(version))) ||
      version != kManifestVersion) {
    throw std::runtime_error("unsupported project manifest version");
  }

  unsigned char projectNameLength = 0;
  if (!arInputStream.read(
          reinterpret_cast<char *>(&projectNameLength),
          static_cast<std::streamsize>(sizeof(projectNameLength)))) {
    throw std::runtime_error("project manifest name length is missing");
  }

  std::string projectName(projectNameLength, '\0');
  if (projectNameLength > 0 &&
      !arInputStream.read(projectName.data(),
                          static_cast<std::streamsize>(projectNameLength))) {
    throw std::runtime_error("project manifest name is truncated");
  }

  this->setProjectName(std::move(projectName));
}

void ProjectManifest::setProjectName(std::string aProjectName) {
  if (aProjectName.size() > kMaximumProjectNameLength) {
    throw std::length_error("project name exceeds 200 bytes");
  }

  this->mProjectName = std::move(aProjectName);
}

std::ostream &operator<<(std::ostream &arOutputStream,
                         const ProjectManifest &arManifest) {
  const auto projectNameLength =
      static_cast<unsigned char>(arManifest.mProjectName.size());

  arOutputStream.write(kManifestSignature.data(),
                       static_cast<std::streamsize>(kManifestSignature.size()));
  arOutputStream.write(reinterpret_cast<const char *>(&kManifestVersion),
                       static_cast<std::streamsize>(sizeof(kManifestVersion)));
  arOutputStream.write(reinterpret_cast<const char *>(&projectNameLength),
                       static_cast<std::streamsize>(sizeof(projectNameLength)));
  arOutputStream.write(arManifest.mProjectName.data(),
                       static_cast<std::streamsize>(projectNameLength));

  return arOutputStream;
}
} // namespace vnvg::project
