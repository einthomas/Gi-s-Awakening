#include "Level.h"

#include <algorithm>
#include <fstream>
#include <string>
#include <json/json.hpp>
#include <SOIL.h>
#include "AbilityType.h"

namespace glm {
    // overloads for json
    // here because json is only used for levels right now
    void from_json(const nlohmann::json &j, vec3 &v) {
        v = vec3(j[0], j[1], j[2]);
    }
}

Level::Level(
    const Object3D &endObject, const glm::vec3 &start, const glm::vec3 &end,
    float startOrientation, GLuint lightMap
) :
    platforms(), triggers(), pressurePlates(),
    endObject(endObject), start(start), end(end),
    startOrientation(startOrientation), lightMap(lightMap)
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

int Level::getTotalObjectCount() {
    return platforms.size() + triggers.size() + pressurePlates.size() + 1;  // + 1 ... the end object
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
            platform.draw(renderInfo, shadowInfo, lightMap);
        }
    }
    for (Trigger &trigger : triggers) {
        if (trigger.isVisible) {
            trigger.draw(renderInfo, shadowInfo, lightMap);
        }
    }
    for (PressurePlate &pressurePlate : pressurePlates) {
        if (pressurePlate.isVisible) {
            pressurePlate.draw(renderInfo, shadowInfo, lightMap);
        }
    }
    endObject.draw(renderInfo, shadowInfo, lightMap);
}

void Level::update(float delta) {
    for (Trigger &trigger : triggers) {
        trigger.update(delta);
    }
}

Level Level::fromFile(
    const char *filename, Material *material, Mesh endMesh,
    const std::map<std::string, PlatformType> &platformTypes
) {
    // Note: this function will crash if the gil file is malformed.
    nlohmann::json json;
    std::ifstream file(filename);
    file >> json;

    int lightMapSize = json["lightMapSize"];

    GLuint lightMap;
    glGenTextures(1, &lightMap);
    glBindTexture(GL_TEXTURE_2D, lightMap);

    int lightMapWidth, lightMapHeight;
    auto lightMapImage = SOIL_load_image(
        ("levels/" + json["lightMapPath"].get<std::string>()).c_str(),
        &lightMapWidth, &lightMapHeight, nullptr, SOIL_LOAD_RGB
    );
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, lightMapWidth, lightMapHeight,
        0, GL_RGB, GL_UNSIGNED_BYTE, lightMapImage
    );
    SOIL_free_image_data(lightMapImage);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    const PlatformType& end = platformTypes.at("End");

    Level level(
        Object3D(
            new PlatformMaterial(
                end.colorTexture, 0
            ), json["end"], glm::vec3(1.0f),
            end.size, endMesh
        ),
        json["start"],
        json["end"],
        json["startOrientation"],
        lightMap
    );

    auto platformsJson = json["platforms"];
    for (auto &platformJson : platformsJson) {
        level.platforms.push_back(Platform(
            &platformTypes.at(platformJson["type"]),
            platformJson["position"], platformJson["name"],
            lightMapSize,
            platformJson["lightMapIndex"]
        ));
    }

    auto pressurePlatesJson = json["pressurePlates"];
    for (auto &pressurePlateJson : pressurePlatesJson) {
        const PlatformType *type = &platformTypes.at(pressurePlateJson["type"]);
        level.pressurePlates.push_back(PressurePlate(
            type,
            PlatformMaterial(type->colorTexture, 0),
            lightMapSize, pressurePlateJson["lightMapIndex"],
            pressurePlateJson["position"],
            static_cast<AbilityType>(
                pressurePlateJson["givesAbility"].get<int>()
            )
        ));
    }

    auto triggersJson = json["triggers"];
    for (auto &triggerJson : triggersJson) {
        const PlatformType *type = &platformTypes.at(triggerJson["type"]);
        std::vector<Platform*> triggeredPlatforms;
        bool isTriggered = triggerJson["isTriggered"].get<int>();
        for (auto &triggeredPlatformName : triggerJson["triggers"]) {
            for (int i = 0; i < level.platforms.size(); i++) {
                if (level.platforms[i].name == triggeredPlatformName.get<std::string>()) {
                    Platform *triggeredPlatform = &level.platforms[i];
                    triggeredPlatform->isVisible = isTriggered;
                    triggeredPlatforms.push_back(triggeredPlatform);
                    break;
                }
            }
        }

        level.triggers.push_back(Trigger(
            &platformTypes.at(triggerJson["type"]),
            PlatformMaterial(type->colorTexture, 0),
            lightMapSize, triggerJson["lightMapIndex"],
            triggerJson["position"],
            isTriggered,
            triggeredPlatforms
        ));
    }

    return level;
}
