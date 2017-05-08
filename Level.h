#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <glm/glm.hpp>

#include "Platform.h"
#include "Trigger.h"

class Level {
public:
    std::vector<Platform> platforms;
    std::vector<Trigger> triggers;
    Object3D endObject;
    glm::vec3 start, end;
    float startOrientation;

    void draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
    void update(float delta);
    bool intersects(const glm::vec3 &position, const glm::vec3 &scale);

    static Level fromFile(
        const char *filename, Material *material, Mesh endMesh,
        const std::map<std::string, PlatformType> &platformTypes
    );
};
