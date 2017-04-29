#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <glm/glm.hpp>

#include "Platform.h"

class Level {
public:
    std::vector<Platform> platforms;
    Object3D endObject;
    glm::vec3 start, end;
    float startOrientation;

    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);

    static Level fromFile(
        const char *filename, Material *material, Mesh endMesh,
        const std::map<std::string, PlatformType> &platformTypes
    );
};
