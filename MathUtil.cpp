#include "MathUtil.h"

glm::vec3 MathUtil::limit(glm::vec3 vector, float magnitude) {
    if (glm::length(vector) > magnitude) {
        return glm::normalize(vector) * magnitude;
    }

    return vector;
}
