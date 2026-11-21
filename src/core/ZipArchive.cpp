
#include "core/ZipArchive.hpp"
#include "zip.h"

namespace core {

ZipArchive::ZipArchive(const std::filesystem::path aPath) : mPath(aPath) {}

ZipArchive::~ZipArchive() {}

bool ZipArchive::exists() { return std::filesystem::exists(mPath); }

ZipArchiveError ZipArchive::create(bool aOverwriteExisting) {
  if (!aOverwriteExisting && this->exists()) {
    return ZipArchiveError::ARCHIVE_ALREADY_EXISTS;
  }
  struct zip_t *zip =
      zip_open(this->mPath.string().c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL,
               ZipArchiveMode::WRITE);
  if (!zip) {
    return ZipArchiveError::UNKNOWN;
  }
  zip_close(zip);

  return ZipArchiveError::NONE;
}

} // namespace core