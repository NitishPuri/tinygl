#pragma once

#include <array>
#include <string>

const std::string BASE_DIR = "D:/tree/rendering/tinyrenderer/";

std::vector<std::string> MODELS = {"african_head", "boogie", "diablo3_pose"};

std::string GetObjPath(std::string model) {
  return BASE_DIR + "obj/" + model + "/" + model + ".obj";
}

std::string GetDiffuseTexture(std::string model) {
  return BASE_DIR + "obj/" + model + "/" + model + "_diffuse.tga";
}

std::string GetOutputPath(const std::string model, const std::string proj,
                          const std::string suffix) {
  return BASE_DIR + "out/" + model + "/" + proj + suffix + ".tga";
}
