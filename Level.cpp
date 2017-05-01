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

bool Level::intersects(const glm::vec3 &position, const glm::vec3 &scale) {
    for (Platform levelObject : platforms) {
        if (levelObject.intersects(position, scale)) {
            return true;
        }
    }
    for (Trigger &trigger : triggers) {
        if (trigger.intersects(position, scale)) {
            return true;
        }
    }

    return false;
}

void Level::draw(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {
    for (Platform &platform : platforms) {
        if (platform.isVisible) {
            platform.draw(viewMatrix, projectionMatrix);
        }
    }
    for (Trigger &trigger : triggers) {
        if (trigger.isVisible) {
            trigger.draw(viewMatrix, projectionMatrix);
        }
    }
    endObject.draw(viewMatrix, projectionMatrix);
}

void Level::update(float delta) {
    for (Trigger &trigger : triggers) {
        trigger.update(delta);
    }
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
            &platformTypes.at(platform["type"]), material, platform["position"], platform["name"]
        ));
    }

    auto triggers = json["triggers"];
    for (auto &trigger : triggers) {
        Platform *triggeredPlatform = nullptr;
        bool isTriggered = trigger["isTriggered"].get<int>();
        for (int i = 0; i < level.platforms.size(); i++) {
            if (level.platforms[i].name == trigger["triggers"]) {
                triggeredPlatform = &level.platforms[i];
                triggeredPlatform->isVisible = isTriggered;
                break;
            }
        }

        std::vector<Platform*> triggeredPlatforms;
        triggeredPlatforms.push_back(triggeredPlatform);
        level.triggers.push_back(Trigger(
            &platformTypes.at(trigger["type"]), material, trigger["position"], isTriggered, triggeredPlatforms
        ));
    }

    level.endObject = { material, level.end, glm::vec3(1.0f), endMesh };

    return level;
}
