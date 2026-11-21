#pragma once

#include <filesystem>

namespace core {

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
   * @brief Creation was requested without overwriting an existing archive.
   */
  ARCHIVE_ALREADY_EXISTS,
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
 * The current implementation supports checking for and creating empty
 * archives. It does not persist an open archive handle between operations.
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
   * the path is occupied, or `UNKNOWN` if creation fails.
   */
  ZipArchiveError create() { return this->create(false); }

  /**
   * @brief Creates an empty archive with optional replacement of an existing
   * archive.
   *
   * @param aOverwriteExisting When `true`, permits writing over an existing
   * path; when `false`, an occupied path returns `ARCHIVE_ALREADY_EXISTS`.
   * @return `NONE` after successful creation, or the applicable
   * `ZipArchiveError`.
   */
  ZipArchiveError create(bool aOverwriteExisting);

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
} // namespace core