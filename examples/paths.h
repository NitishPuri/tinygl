#pragma once

#include <array>
#include <string>

namespace Paths {
const std::string BASE_DIR = ROOT_DIR;

std::vector<std::string> MODELS = {"african_head", "boogie", "diablo3_pose"};

std::string GetObjPath(std::string model) {
  return BASE_DIR + "obj/" + model + "/" + model + ".obj";
}

std::string GetDiffuseTexture(std::string model) {
  return BASE_DIR + "obj/" + model + "/" + model + "_diffuse.jpg";
}

std::string GetNormalTexture(std::string model) {
  return BASE_DIR + "obj/" + model + "/" + model + "_nm_tangent.jpg";
}

std::string GetOutputPath(const std::string model, const std::string proj,
                          const std::string suffix) {
  return BASE_DIR + "out/" + model + "/" + proj + suffix + ".jpg";
}

}

