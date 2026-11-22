#include <doctest/doctest.h>

#include <sstream>
#include <string>

#include "project/ProjectManifest.hpp"

SCENARIO("project manifests can be serialized and recreated") {
  GIVEN("a project manifest with a name") {
    vnvg::project::ProjectManifest manifest;
    manifest.setProjectName("Round Trip Project");

    WHEN("the manifest is written to an output stream") {
      std::ostringstream output(std::ios::binary);
      std::ostream &arOutputStream = output;
      arOutputStream << manifest;

      AND_WHEN("a manifest is recreated from the serialized data") {
        std::istringstream input(output.str(), std::ios::binary);
        vnvg::project::ProjectManifest recreatedManifest(input);

        THEN("the recreated project name matches the original") {
          CHECK(recreatedManifest.getProjectName() ==
                manifest.getProjectName());
        }
      }
    }
  }
}
