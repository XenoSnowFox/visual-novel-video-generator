#pragma once

#include <filesystem>
#include <iostream>
#include <utility>

#if defined(_WIN32)
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

/**
 * @brief Provides an exclusive, blocking lock for a sidecar lock file.
 *
 * The lock file is created beside the target path by appending `.lock`. The
 * lock is advisory, transfers to a move destination, and is released when the
 * owning object is destroyed. A failed acquisition is reported to standard
 * error and leaves the object in an unlocked state. The sidecar file is not
 * removed when the lock is released.
 */
class FileLock {
public:
  /**
   * @brief Attempts to acquire an exclusive lock for a target path.
   *
   * This operation blocks until the platform lock becomes available. It does
   * not lock or open the target path itself.
   *
   * @param arTargetFilePath Path whose adjacent `.lock` file should be locked.
   */
  explicit FileLock(const std::filesystem::path &arTargetFilePath) {
    std::filesystem::path lockPath = arTargetFilePath;
    lockPath += ".lock";

#if defined(_WIN32)
    mHandle = CreateFileW(lockPath.c_str(), GENERIC_READ | GENERIC_WRITE,
                          FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                          OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (mHandle != INVALID_HANDLE_VALUE) {
      OVERLAPPED overlapped = {0};
      if (LockFileEx(mHandle, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD,
                     &overlapped)) {
        mLocked = true;
      } else {
        CloseHandle(mHandle);
        mHandle = INVALID_HANDLE_VALUE;
      }
    }
#else
    mFileDescriptor = open(lockPath.c_str(), O_RDWR | O_CREAT, 0666);
    if (mFileDescriptor != -1) {
      struct flock fl{};
      fl.l_type = F_WRLCK;
      fl.l_whence = SEEK_SET;
      fl.l_start = 0;
      fl.l_len = 0;

      if (fcntl(mFileDescriptor, F_SETLKW, &fl) != -1) {
        mLocked = true;
      } else {
        close(mFileDescriptor);
        mFileDescriptor = -1;
      }
    }
#endif
    if (!mLocked) {
      std::cerr << "[FileLock] Failed to acquire lock for: " << lockPath
                << "\n";
    }
  }

  /**
   * @brief Releases the acquired lock, if any.
   */
  ~FileLock() { Unlock(); }

  /**
   * @brief Copy construction is disabled because a lock has a single owner.
   *
   * @param arOther Source object that cannot be copied.
   */
  FileLock(const FileLock &arOther) = delete;

  /**
   * @brief Copy assignment is disabled because a lock has a single owner.
   *
   * @param arOther Source object that cannot be copied.
   * @return This operation is deleted and never returns.
   */
  FileLock &operator=(const FileLock &arOther) = delete;

  /**
   * @brief Transfers lock ownership from another object.
   *
   * @param arOther Source object. After the move, it no longer reports an
   * acquired lock.
   */
  FileLock(FileLock &&arOther) noexcept { *this = std::move(arOther); }

  /**
   * @brief Releases the current lock and takes ownership from another object.
   *
   * @param arOther Source object. After the move, it no longer reports an
   * acquired lock.
   * @return A reference to this object.
   */
  FileLock &operator=(FileLock &&arOther) noexcept {
    if (this != &arOther) {
      Unlock();
#if defined(_WIN32)
      mHandle = arOther.mHandle;
      arOther.mHandle = INVALID_HANDLE_VALUE;
#else
      mFileDescriptor = arOther.mFileDescriptor;
      arOther.mFileDescriptor = -1;
#endif
      mLocked = arOther.mLocked;
      arOther.mLocked = false;
    }
    return *this;
  }

  /**
   * @brief Reports whether this object currently owns an acquired lock.
   *
   * @return `true` if a lock was acquired and has not yet been released;
   * otherwise `false`.
   */
  [[nodiscard]] bool IsLocked() const noexcept { return mLocked; }

private:
  /**
   * @brief Releases the platform lock and closes its handle or descriptor.
   *
   * This operation has no effect when no lock is currently owned.
   */
  void Unlock() {
    if (!mLocked) {
      return;
    }

#if defined(_WIN32)
    if (mHandle != INVALID_HANDLE_VALUE) {
      OVERLAPPED overlapped = {0};
      UnlockFileEx(mHandle, 0, MAXDWORD, MAXDWORD, &overlapped);
      CloseHandle(mHandle);
      mHandle = INVALID_HANDLE_VALUE;
    }
#else
    if (mFileDescriptor != -1) {
      struct flock fl{};
      fl.l_type = F_UNLCK;
      fl.l_whence = SEEK_SET;
      fl.l_start = 0;
      fl.l_len = 0;
      fcntl(mFileDescriptor, F_SETLK, &fl);
      close(mFileDescriptor);
      mFileDescriptor = -1;
    }
#endif
    mLocked = false;
  }

#if defined(_WIN32)
  /**
   * @brief Handle to the sidecar lock file on Windows.
   */
  HANDLE mHandle = INVALID_HANDLE_VALUE;
#else
  /**
   * @brief File descriptor for the sidecar lock file on non-Windows systems.
   */
  int mFileDescriptor = -1;
#endif
  /**
   * @brief Indicates whether the platform lock is currently owned.
   */
  bool mLocked = false;
};
