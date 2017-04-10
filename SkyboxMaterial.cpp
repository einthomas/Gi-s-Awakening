#include "SkyboxMaterial.h"
#include <SOIL.h>

Shader SkyboxMaterial::shader;

void SkyboxMaterial::init() {
    shader = Shader("shaders/skybox.vert", "shaders/skybox.frag");
}

SkyboxMaterial::SkyboxMaterial(const std::vector<std::string> &textures) :
    cubeMap(loadCubemap(textures))
{
}

void SkyboxMaterial::bind(const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix, const glm::mat4 &modelMatrix) {
    shader.use();
    shader.setMatrix4("model", modelMatrix);
    shader.setMatrix4("view", glm::mat4(glm::mat3(viewMatrix)));
    shader.setMatrix4("projection", projectionMatrix);
    shader.setTextureCubeMap("skybox", GL_TEXTURE0, cubeMap, 0);
}

GLuint SkyboxMaterial::loadCubemap(const std::vector<std::string> &textures) {
    GLuint cubemapTexture;
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    int imageWidth, imageHeight;
    for (int i = 0; i < textures.size(); i++) {
        auto image = SOIL_load_image(textures[i].c_str(), &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        SOIL_free_image_data(image);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return cubemapTexture;
}
