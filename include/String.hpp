#pragma once;

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace vnvg {
class String {
private:
  std::string data;

public:
  // ----------------------------------------------------
  // Constructors & Conversions
  // ----------------------------------------------------
  String() = default;
  String(const char *str) : data(str ? str : "") {}
  String(std::string str) : data(std::move(str)) {}
  String(std::string_view sv) : data(sv) {}

  // Implicit conversions for smooth STL integration
  operator const std::string &() const & noexcept { return data; }
  operator std::string() && noexcept { return std::move(data); }
  operator std::string_view() const noexcept { return data; }

  // Direct accessors
  [[nodiscard]] const std::string &str() const & noexcept { return data; }
  [[nodiscard]] const char *c_str() const noexcept { return data.c_str(); }
  [[nodiscard]] size_t length() const noexcept { return data.length(); }
  [[nodiscard]] bool empty() const noexcept { return data.empty(); }

  // ----------------------------------------------------
  // Chained Mutating Operations (lvalue & rvalue overloads)
  // ----------------------------------------------------

  // Trims leading and trailing whitespace in-place
  String &trim() & {
    auto is_space = [](unsigned char ch) { return std::isspace(ch); };

    // Trim left
    auto start = std::find_if_not(data.begin(), data.end(), is_space);
    data.erase(data.begin(), start);

    // Trim right
    auto end = std::find_if_not(data.rbegin(), data.rend(), is_space).base();
    data.erase(end, data.end());

    return *this;
  }

  String &&trim() && { return std::move(trim()); }

  // Replaces all occurrences of 'target' with 'replacement'
  String &replace(std::string_view target, std::string_view replacement) & {
    if (target.empty())
      return *this;

    size_t pos = 0;
    while ((pos = data.find(target, pos)) != std::string::npos) {
      data.replace(pos, target.length(), replacement);
      pos += replacement.length();
    }
    return *this;
  }

  String &&replace(std::string_view target, std::string_view replacement) && {
    return std::move(replace(target, replacement));
  }

  // Converts string to lowercase in-place
  String &to_lower() & {
    std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c) {
      return static_cast<char>(std::tolower(c));
    });
    return *this;
  }

  String &&to_lower() && { return std::move(to_lower()); }

  // ----------------------------------------------------
  // Terminal / Non-Mutating Operations
  // ----------------------------------------------------

  // Splits string by a delimiter into a vector of String wrappers
  [[nodiscard]] std::vector<String> split(std::string_view delimiter) const {
    std::vector<String> tokens;
    if (delimiter.empty()) {
      tokens.push_back(*this);
      return tokens;
    }

    size_t start = 0;
    size_t end = data.find(delimiter);

    while (end != std::string::npos) {
      tokens.emplace_back(data.substr(start, end - start));
      start = end + delimiter.length();
      end = data.find(delimiter, start);
    }
    tokens.emplace_back(data.substr(start));

    return tokens;
  }

  friend std::ostream &operator<<(std::ostream &arStream,
                                  const vnvg::String &arString) {
    return arStream << arString.data;
  }
};

} // namespace vnvg