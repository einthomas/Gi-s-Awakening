#include "Level.h"

#include <algorithm>
#include <fstream>
#include <json/json.hpp>

namespace glm {
    // overloads for json
    // here because json is only used for levels right now
    void from_json(const nlohmann::json &j, vec3 &v) {
        v = vec3(j[0], j[1], j[2]);
    }
}

void Level::draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {
    for (Object3D &object : platforms) {
        object.draw(viewMatrix, projectionMatrix);
    }
    endObject.draw(viewMatrix, projectionMatrix);
}

Level Level::fromFile(const char *filename, Material *material, Mesh endMesh, const std::map<std::string, PlatformType> &platformTypes) {
    // Note: this function will crash if the gil file is malformed.
    Level level;
    nlohmann::json json;
    std::ifstream file(filename);
    file >> json;

    level.start = json["start"];
    level.end = json["end"];
    level.startOrientation = json["startOrientation"];

    auto platforms = json["platforms"];
    for (auto &platform : platforms) {
        level.platforms.push_back(Platform(
            &platformTypes.at(platform["type"]), material, platform["position"]
        ));
    }

    level.endObject = { material, level.end, glm::vec3(1.0f), endMesh };

    return level;
}
