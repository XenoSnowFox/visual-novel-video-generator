#include <utility>

#include "core/CommandProcessor.hpp"

core::CommandProcessor::CommandProcessor(std::string aName,
                                         std::string aSummary,
                                         CommandProcessor *aParent)
    : mName(std::move(aName)), mSummary(std::move(aSummary)), mParent(aParent) {
}

void core::CommandProcessor::addExample(CommandExample aExample) {
  mExamples.push_back(std::move(aExample));
}