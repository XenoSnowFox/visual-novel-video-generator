#pragma once

#include <cstddef>
#include <filesystem>
#include <string>

namespace vnvg::storage {

/**
 * @brief Tracks the access state of a ZIP archive.
 */
enum ZipArchiveState {
  /**
   * @brief No archive access is currently active.
   */
  CLOSED = 0,

  /**
   * @brief The archive is available for read-only access.
   */
  READ_ONLY = 1,

  /**
   * @brief The archive is available for read and write access.
   */
  READ_WRITE = 2
};

/**
 * @brief Reports the result of a ZIP archive operation.
 */
enum ZipArchiveError {
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
  LOCK_ACQUISITION_FAILED,

  /**
   * @brief Creation was requested without overwriting an existing archive.
   */
  ARCHIVE_ALREADY_EXISTS,

  /**
   * @brief The process failed to write an entry into the archive.
   */
  ENTRY_WRITE_FAILED,
};

/**
 * @brief Maps archive access modes to the values expected by kuba--/zip.
 */
enum ZipArchiveMode : char {
  /**
   * @brief Opens an archive for reading.
   */
  READ = 'r',

  /**
   * @brief Opens an archive for writing.
   */
  WRITE = 'w',

  /**
   * @brief Opens an archive for appending.
   */
  APPEND = 'a'
};

/**
 * @brief Provides a small abstraction for a ZIP archive path and lifecycle.
 *
 * The current implementation supports checking for, creating, and adding
 * entries to archives. It does not persist an open archive handle between
 * operations.
 */
class ZipArchive {
public:
  /**
   * @brief Stores a ZIP archive path without opening or validating it.
   *
   * @param aPath Filesystem path for the archive.
   */
  ZipArchive(const std::filesystem::path aPath);

  /**
   * @brief Releases archive resources.
   *
   * The current implementation owns no persistent open handle.
   */
  ~ZipArchive();

  /**
   * @brief Determines whether the archive path currently exists.
   *
   * @return `true` when any filesystem entry exists at the archive path;
   * otherwise `false`.
   */
  bool exists();

  /**
   * @brief Creates an empty archive without overwriting an existing path.
   *
   * @return `NONE` after successful creation, `ARCHIVE_ALREADY_EXISTS` when
   * the path is occupied, `LOCK_ACQUISITION_FAILED` when its sidecar lock
   * cannot be obtained, or `UNKNOWN` if archive creation fails.
   */
  ZipArchiveError create() { return this->create(false); }

  /**
   * @brief Creates an empty archive with optional replacement of an existing
   * archive.
   *
   * @param aOverwriteExisting When `true`, permits writing over an existing
   * path; when `false`, an occupied path returns `ARCHIVE_ALREADY_EXISTS`.
   * @return `NONE` after successful creation, or the applicable
   * `ZipArchiveError`. Lock acquisition uses a sidecar file.
   */
  ZipArchiveError create(bool aOverwriteExisting);

  /**
   * @brief Appends a binary entry to the archive.
   *
   * The archive must already exist and be readable as a ZIP archive. The
   * entire buffer is written in one operation while holding the archive's
   * sidecar lock.
   *
   * @param arEntryKey Archive-relative key for the new entry.
   * @param apBuffer Buffer containing the entry data.
   * @param aSize Number of bytes to write from the buffer.
   * @return `NONE` after writing and closing the entry, or the applicable
   * `ZipArchiveError`.
   */
  ZipArchiveError writeEntry(const std::string &arEntryKey,
                             const void *apBuffer, std::size_t aSize);

  /**
   * @brief Appends a string entry to the archive.
   *
   * This overload copies no data beyond the lifetime of the method call and
   * writes the complete byte sequence held by the string.
   *
   * @param arEntryKey Archive-relative key for the new entry.
   * @param arEntryValue Bytes to store in the entry.
   * @return `NONE` after writing and closing the entry, or the applicable
   * `ZipArchiveError`.
   */
  ZipArchiveError writeEntry(const std::string &arEntryKey,
                             const std::string &arEntryValue) {
    return this->writeEntry(arEntryKey, arEntryValue.data(),
                            arEntryValue.size());
  }

private:
  /**
   * @brief Filesystem path associated with this archive.
   *
   * The file is not owned or modified by this member.
   */
  const std::filesystem::path mPath;

  /**
   * @brief Current logical access state of the archive.
   *
   * The current implementation does not transition this value because
   * operations open and close their archive handles immediately.
   */
  ZipArchiveState mState = ZipArchiveState::CLOSED;
};
} // namespace vnvg::storage