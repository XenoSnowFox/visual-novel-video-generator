#include "core/ZipArchive.hpp"
#include "core/FileLock.hpp"
#include "zip.h"

namespace core {

ZipArchive::ZipArchive(const std::filesystem::path aPath) : mPath(aPath) {}

ZipArchive::~ZipArchive() {}

bool ZipArchive::exists() { return std::filesystem::exists(mPath); }

ZipArchiveError ZipArchive::create(bool aOverwriteExisting) {
  FileLock lock(this->mPath);
  if (!lock.IsLocked()) {
    return ZipArchiveError::LOCK_ACQUISITION_FAILED;
  }

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

ZipArchiveError ZipArchive::writeEntry(const std::string &arEntryKey,
                                       const void *apBuffer,
                                       std::size_t aSize) {
  FileLock lock(this->mPath);
  if (!lock.IsLocked()) {
    return ZipArchiveError::LOCK_ACQUISITION_FAILED;
  }

  struct zip_t *zip =
      zip_open(this->mPath.string().c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL,
               ZipArchiveMode::APPEND);
  if (!zip) {
    return ZipArchiveError::UNKNOWN;
  }

  if (zip_entry_open(zip, arEntryKey.c_str()) < 0) {
    zip_close(zip);
    return ZipArchiveError::ENTRY_WRITE_FAILED;
  }

  if (zip_entry_write(zip, apBuffer, aSize) < 0) {
    zip_entry_close(zip);
    zip_close(zip);
    return ZipArchiveError::ENTRY_WRITE_FAILED;
  }

  if (zip_entry_close(zip) < 0) {
    zip_close(zip);
    return ZipArchiveError::ENTRY_WRITE_FAILED;
  }
  zip_close(zip);
  return ZipArchiveError::NONE;
}

} // namespace core
