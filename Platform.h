#pragma once

#include "Object3D.h"
#include "PlatformType.h"

class Platform : public Object3D {
    PlatformType *type;

public:
    Platform(PlatformType *type, glm::vec3 position);
};

#endif // PLATFORM_H
