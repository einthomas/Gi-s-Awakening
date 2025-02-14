#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <glm/glm.hpp>

#include "Platform.h"
#include "Trigger.h"
#include "PressurePlate.h"

class Level {
public:
    std::vector<Platform> platforms;
    std::vector<Trigger> triggers;
    std::vector<PressurePlate> pressurePlates;
    Platform endObject;
    glm::vec3 start, end;
    float startOrientation;
    GLuint lightMap;
    float time;

    Level(
        const Platform &endObject, const glm::vec3 &start,
        const glm::vec3 &end, float startOrientation, GLuint lightMap = 0
    );

    void draw(const Shader &shader);
    void draw(RenderInfo renderInfo, ShadowInfo shadowInfo);
    void update(float delta);
    bool intersects(const glm::vec3 &position, const glm::vec3 &scale);
    int getTotalObjectCount();

    static Level fromFile(const char *filename, Material *material, Mesh endMesh,
        const std::map<std::string, PlatformType> &platformTypes);
};
