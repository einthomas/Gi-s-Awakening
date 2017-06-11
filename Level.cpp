#include "Level.h"

#include <algorithm>
#include <fstream>
#include <json/json.hpp>
#include "AbilityType.h"

namespace glm {
    // overloads for json
    // here because json is only used for levels right now
    void from_json(const nlohmann::json &j, vec3 &v) {
        v = vec3(j[0], j[1], j[2]);
    }
}

Level::Level(const Object3D &endObject, const glm::vec3 &start, const glm::vec3 &end, float startOrientation) : 
    endObject(endObject), start(start), end(end), startOrientation(startOrientation)
{
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

void Level::draw(const Shader& shader) {
    for (Platform &platform : platforms) {
        if (platform.isVisible) {
            platform.draw(shader);
        }
    }
    for (Trigger &trigger : triggers) {
        if (trigger.isVisible) {
            trigger.draw(shader);
        }
    }
    for (PressurePlate &pressurePlate : pressurePlates) {
        if (pressurePlate.isVisible) {
            pressurePlate.draw(shader);
        }
    }
    endObject.draw(shader);
}

void Level::draw(RenderInfo renderInfo, ShadowInfo shadowInfo) {
    for (Platform &platform : platforms) {
        if (platform.isVisible) {
            platform.draw(renderInfo, shadowInfo);
        }
    }
    for (Trigger &trigger : triggers) {
        if (trigger.isVisible) {
            trigger.draw(renderInfo, shadowInfo);
        }
    }
    for (PressurePlate &pressurePlate : pressurePlates) {
        if (pressurePlate.isVisible) {
            pressurePlate.draw(renderInfo, shadowInfo);
        }
    }
    endObject.draw(renderInfo, shadowInfo);
}

void Level::update(float delta) {
    for (Trigger &trigger : triggers) {
        trigger.update(delta);
    }
}

Level Level::fromFile(const char *filename, Material *material, Mesh endMesh, const std::map<std::string, PlatformType> &platformTypes) {
    // Note: this function will crash if the gil file is malformed.
    nlohmann::json json;
    std::ifstream file(filename);
    file >> json;

    Level level(
        Object3D(material, json["end"], glm::vec3(1.0f), endMesh),
        json["start"],
        json["end"],
        json["startOrientation"]
    );

    auto platformsJson = json["platforms"];
    for (auto &platformJson : platformsJson) {
        level.platforms.push_back(Platform(
            &platformTypes.at(platformJson["type"]), material, platformJson["position"], platformJson["name"]
        ));
    }

    auto pressurePlatesJson = json["pressurePlates"];
    for (auto &pressurePlateJson : pressurePlatesJson) {
        level.pressurePlates.push_back(PressurePlate(
            &platformTypes.at(pressurePlateJson["type"]),
            BlinnMaterial(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.0f),
            pressurePlateJson["position"],
            static_cast<AbilityType>(pressurePlateJson["givesAbility"].get<int>())
        ));
    }

    std::vector<Platform*> triggeredPlatforms;
    auto triggersJson = json["triggers"];
    for (auto &triggerJson : triggersJson) {
        bool isTriggered = triggerJson["isTriggered"].get<int>();
        for (int i = 0; i < level.platforms.size(); i++) {
            if (level.platforms[i].name == triggerJson["triggers"]) {
                Platform *triggeredPlatform = &level.platforms[i];
                triggeredPlatform->isVisible = isTriggered;
                triggeredPlatforms.push_back(triggeredPlatform);
                break;
            }
        }

        level.triggers.push_back(Trigger(
            &platformTypes.at(triggerJson["type"]),
            BlinnMaterial(glm::vec3(1.0f), glm::vec3(0.0f), 0.0f),
            triggerJson["position"],
            isTriggered,
            triggeredPlatforms
        ));
    }

    return level;
}
