#include <doctest/doctest.h>

#include <sstream>
#include <string>

#include "project/ProjectMenifest.hpp"

SCENARIO("project manifests can be serialized and recreated") {
  GIVEN("a project manifest with a name") {
    project::ProjectManifest manifest;
    manifest.set_project_name("Round Trip Project");

    WHEN("the manifest is written to an output stream") {
      std::ostringstream output(std::ios::binary);
      std::ostream &arOutputStream = output;
      arOutputStream << manifest;

      AND_WHEN("a manifest is recreated from the serialized data") {
        std::istringstream input(output.str(), std::ios::binary);
        project::ProjectManifest recreatedManifest(input);

        THEN("the recreated project name matches the original") {
          CHECK(recreatedManifest.get_project_name() ==
                manifest.get_project_name());
        }
      }
    }
  }
}
