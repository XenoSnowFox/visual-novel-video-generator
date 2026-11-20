#include <utility>

#include "core/CommandProcessor.hpp"

core::CommandProcessor::CommandProcessor(std::string aName,
                                         std::string aSummary)
    : mName(std::move(aName)), mSummary(std::move(aSummary)) {}

void core::CommandProcessor::addExample(const CommandExample aExample) {
  mExamples.push_back(std::move(aExample));
}