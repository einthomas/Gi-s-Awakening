#include <iostream>
#include <chrono>
#include <memory>
#include <stdlib.h>
#include <time.h>

#include <irrKlang.h>

#include <json/json.hpp>

#include <SOIL.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Camera.h"
#include "Level.h"
#include "Mesh.h"
#include "BlinnMaterial.h"
#include "Player.h"
#include "TextRenderer.h"
#include "Skybox.h"
#include "SkyboxMaterial.h"
#include "ParticleSystem.h"
#include "Game.h"
#include "PlatformMaterial.h"
#include "SoundEngine.h"
#include "Textures.h"

static int width = 1280, height = 720;
static const char *title = "Gi's Awakening: The Mending of the Sky";
static GLuint screenQuadVAO = 0;
const int AA_SAMPLES = 4;
const int DEPTH_TEXTURE_WIDTH = 1024;
const int DEPTH_TEXTURE_HEIGHT = 1024;
const int BLOOM_BLUR_WIDTH = 512;
const int BLOOM_BLUR_HEIGHT = 288;
const int DEPTH_TEXTURE_BLUR_WIDTH = 1024;
const int DEPTH_TEXTURE_BLUR_HEIGHT = 1024;
const int NUM_SHADOW_MAPS = 3;
static GLfloat fLargest;

enum class TextureSamplingQuality {
    NEAREST_NEIGHBOR,
    BILINEAR
};
TextureSamplingQuality textureSamplingQuality = TextureSamplingQuality::BILINEAR;

enum class MipMappingQuality {
    OFF,
    NEAREST_NEIGHBOR,
    LINEAR
};
MipMappingQuality mipMappingQuality = MipMappingQuality::LINEAR;

bool initGLEW();
GLFWwindow *initGLFW();
void drawScreenQuad();
void generateFBO(
    GLuint &FBO, GLuint* colorBuffers, int width, int height, int numColorAttachments,
    bool isMultisampled, bool attachDepthRBO
);
void blitFramebuffer(
    GLuint srcFBO, GLuint destFBO, GLbitfield mask, GLenum readBuffer, GLenum drawBuffer,
    GLuint srcWidth, GLuint srcHeight, GLuint destWidth, GLuint destHeight, GLenum filter
);
GLuint blur(
    int width, int height, GLuint blurFBO0, GLuint blurFBO1, GLuint blurBuffer0,
    GLuint blurBuffer1, GLuint inputBuffer, Shader &shader
);
void calculateSplitFrustumCornersWorldSpace(
    glm::vec3 frustumCornersWorldSpace[], const float *cascadeEnds,
    const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix
);
void calculateShadowMappingProjectionMatrices(
    glm::mat4 shadowMappingProjectionMatrices[], glm::vec3 frustumCornersWorldSpace[],
    const glm::mat4 &shadowMappingViewMatrix
);
GLuint loadTexture(const char* filename, GLfloat fLargest);
void loadPlatformTypesTextures(
    std::map<std::string, PlatformType> &platformTypes,
    const nlohmann::json &platformTypesJson,
    Level &level
);

int main(void) {
    srand(time(NULL));

    GLFWwindow* window = initGLFW();
    if (window == NULL) {
        return 0;
    }

    // activate v-sync
    glfwSwapInterval(0);

    if (!initGLEW()) {
        return 0;
    }

    glm::mat4 shadowMappingViewMatrix = glm::lookAt(
        glm::vec3(-3.5f, -5.3f, 7.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    glm::mat4 ndcUVMappingMatrix = glm::mat4(
        glm::vec4(0.5f, 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
        glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)
    );

    glViewport(0, 0, width, height);
    // Set OpenGL options
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);

    const float FOV = glm::radians(70.0f);
    const float CAMERA_NEAR = 0.01f;
    const float CAMERA_FAR = 100.0f;
    glm::mat4 projectionMatrix = glm::perspectiveFov(
        FOV,
        static_cast<float>(width),
        static_cast<float>(height),
        CAMERA_NEAR,
        CAMERA_FAR
    );

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    GLuint multisampledFBO;
    GLuint multisampledColorBuffers[2];
    generateFBO(multisampledFBO, multisampledColorBuffers, width, height, 2, true, true);

    GLuint FBO;
    GLuint colorBuffers[2];
    generateFBO(FBO, colorBuffers, width, height, 2, false, true);

    GLuint blurFBOs[2];
    GLuint blurColorBuffers[2];
    generateFBO(blurFBOs[0], &blurColorBuffers[0], BLOOM_BLUR_WIDTH, BLOOM_BLUR_HEIGHT, 1, false, false);
    generateFBO(blurFBOs[1], &blurColorBuffers[1], BLOOM_BLUR_WIDTH, BLOOM_BLUR_HEIGHT, 1, false, false);

    GLuint blurShadowMapFBOs[2];
    GLuint blurShadowMapBuffers[2];
    generateFBO(blurShadowMapFBOs[0], &blurShadowMapBuffers[0], DEPTH_TEXTURE_BLUR_WIDTH, DEPTH_TEXTURE_BLUR_HEIGHT, 1, false, false);
    generateFBO(blurShadowMapFBOs[1], &blurShadowMapBuffers[1], DEPTH_TEXTURE_BLUR_WIDTH, DEPTH_TEXTURE_BLUR_HEIGHT, 1, false, false);

    GLuint hudFBO;
    GLuint hudColorBuffer;
    generateFBO(hudFBO, &hudColorBuffer, width, height, 1, false, false);

    const float cascadeEnds[4] = {
        CAMERA_NEAR - 2.0f,
        6.0f,
        15.0f,
        30.0f
    };

    GLuint depthFBO;
    glGenFramebuffers(1, &depthFBO);

    GLuint shadowMaps[NUM_SHADOW_MAPS];
    glGenTextures(NUM_SHADOW_MAPS, shadowMaps);
    for (int i = 0; i < NUM_SHADOW_MAPS; i++) {
        glBindTexture(GL_TEXTURE_2D, shadowMaps[i]);
        glTexImage2D(
            GL_TEXTURE_2D, 0,
            GL_DEPTH_COMPONENT24, DEPTH_TEXTURE_WIDTH, DEPTH_TEXTURE_HEIGHT,
            0, GL_DEPTH_COMPONENT, GL_FLOAT, 0
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMaps[0], 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::vector<std::string> textures;
    textures.push_back("textures/skybox/right.jpg");
    textures.push_back("textures/skybox/left.jpg");
    textures.push_back("textures/skybox/front.jpg");
    textures.push_back("textures/skybox/back.jpg");
    textures.push_back("textures/skybox/top.jpg");
    textures.push_back("textures/skybox/bottom.jpg");
    Skybox skybox(textures);

    BlinnMaterial::init();
    SkyboxMaterial::init();
    std::unique_ptr<BlinnMaterial> material(new BlinnMaterial(glm::vec3(1.0f), glm::vec3(1.0f, 1.0f, 0.5f), 64.0f));

    std::map<std::string, PlatformType> platformTypes;
    nlohmann::json platformTypesJson;
    std::ifstream platformTypesFile("geometry/set1.gib");
    platformTypesFile >> platformTypesJson;

    for (auto &platformType : platformTypesJson) {
        platformTypes.emplace(
            platformType["name"].get<std::string>(), PlatformType {
                glm::vec3(
                    platformType["size"][0],
                    platformType["size"][1],
                    platformType["size"][2]
                ),
                Mesh::fromFile((
                    "geometry/" +
                    platformType["name"].get<std::string>() + ".vbo"
                ).c_str()),
                loadTexture((
                    "textures/" +
                    platformType.value<std::string>("colorTexture", "")
                ).c_str(), fLargest),
                loadTexture((
                    "textures/" +
                    platformType.value<std::string>("linesTexture", "")
                ).c_str(), fLargest)
            }
        );
    }

    Mesh endMesh = Mesh::fromFile("geometry/End.vbo");

    Level level = Level::fromFile(
        "levels/level1.gil",
        material.get(),
        endMesh, platformTypes
    );
    Game game(level);

    Shader shadowMappingDepthShader = Shader("shaders/shadowMappingDepth.vert", "shaders/shadowMappingDepth.frag");
    Shader textShader = Shader("shaders/textShader.vert", "shaders/textShader.frag");
    TextRenderer::init(width, height, "fonts/Gidole-Regular.ttf", textShader);
    Shader gaussianBlurShader4 = Shader("shaders/gaussianBlur.vert", "shaders/gaussianBlur4.frag");
    Shader gaussianBlurShader7 = Shader("shaders/gaussianBlur.vert", "shaders/gaussianBlur7.frag");
    Shader postProcessingShader = Shader("shaders/postProcessing.vert", "shaders/postProcessing.frag");
    ParticleSystem::init();
    SoundEngine::init();

    glm::vec3 youDiedTextDimensions = TextRenderer::calcDimensions("You died", 1.0f);

    int centerX = width / 2, centerY = height / 2;
    glfwSetCursorPos(window, centerX, centerY);

    bool displayPerformanceStats = false;
    bool drawAsWireframe = false;
    bool bloomActivated = true;
    bool shadowsActivated = true;
    bool displayHelp = false;
    int currentShadowMap = 0;   // TODO: DEBUG, REMOVE!!

    int fKeyStates[10] = { 0 };

    int softFps = 0;
    float softFrameTime = 0.0f;
    double time = glfwGetTime();
    double previousTime = time;
    SoundEngine::soundEndTime = time;
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double delta = currentTime - previousTime;
        previousTime = currentTime;

        SoundEngine::update(delta, currentTime);

        // movement
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            game.forwardPressed();
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            game.leftPressed();
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            game.backwardsPressed();
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            game.rightPressed();
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            game.confirmPressed();
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            game.confirmReleased();
        }

        // shooting
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            game.primaryActionPressed();
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
            game.primaryActionReleased();
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            game.secondaryActionPressed();
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
            game.secondaryActionReleased();
        }

        // f-keys
        if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS && fKeyStates[0] == GLFW_RELEASE) {
            // toggle help display
            displayHelp = !displayHelp;
            std::cout << "Help display toggled " << (drawAsWireframe ? "on" : "off") << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS && fKeyStates[1] == GLFW_RELEASE) {
            // toggle performance stats
            displayPerformanceStats = !displayPerformanceStats;
            std::cout << "Performance stats toggled " << (displayPerformanceStats ? "on" : "off")
                << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS && fKeyStates[2] == GLFW_RELEASE) {
            // toggle draw wireframe
            drawAsWireframe = !drawAsWireframe;
            std::cout << "Wireframe toggled " << (drawAsWireframe ? "on" : "off") << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS && fKeyStates[3] == GLFW_RELEASE) {
            // texture sampling quality Nearest Neighbor/Bilinear
            std::cout << "Texture sampling quality set to ";
            if (textureSamplingQuality == TextureSamplingQuality::BILINEAR) {
                textureSamplingQuality = TextureSamplingQuality::NEAREST_NEIGHBOR;
                std::cout << " nearest neighbor";
            } else {
                textureSamplingQuality = TextureSamplingQuality::BILINEAR;
                std::cout << " bilinear";
            }
            std::cout << std::endl;
            loadPlatformTypesTextures(platformTypes, platformTypesJson, level);

            //currentShadowMap++;    // TODO: DEBUG, REMOVE!!
            //currentShadowMap %= NUM_SHADOW_MAPS;
        }
        if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS && fKeyStates[4] == GLFW_RELEASE) {
            // toggle mip mapping quality Off/Nearest Neighbor/Linear
            if (mipMappingQuality == MipMappingQuality::OFF) {
                mipMappingQuality = MipMappingQuality::NEAREST_NEIGHBOR;
                std::cout << "Mip mapping quality set to nearest neighbor" << std::endl;
            } else if (mipMappingQuality == MipMappingQuality::NEAREST_NEIGHBOR) {
                mipMappingQuality = MipMappingQuality::LINEAR;
                std::cout << "Mip mapping quality set to linear" << std::endl;
            } else {
                mipMappingQuality = MipMappingQuality::OFF;
                std::cout << "Mip mapping disabled" << std::endl;
            }
            loadPlatformTypesTextures(platformTypes, platformTypesJson, level);
        }
        if (glfwGetKey(window, GLFW_KEY_F6) == GLFW_PRESS && fKeyStates[5] == GLFW_RELEASE) {
            // toggle bloom
            bloomActivated = !bloomActivated;
            postProcessingShader.use();
            postProcessingShader.setInteger("bloomActivated", bloomActivated);
            std::cout << "Bloom toggled " << (bloomActivated ? "on" : "off") << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_F7) == GLFW_PRESS && fKeyStates[6] == GLFW_RELEASE) {
            // toggle shadows
            shadowsActivated = !shadowsActivated;
            std::cout << "Shadows toggled " << (shadowsActivated ? "on" : "off") << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_F8) == GLFW_PRESS && fKeyStates[7] == GLFW_RELEASE) {
            // toggle view frustum culling
            Object3D::frustumCullingEnabled = !Object3D::frustumCullingEnabled;
            std::cout << "View frustum culling toggled " <<
                (Object3D::frustumCullingEnabled ? "on" : "off") << std::endl;
        }
        if (glfwGetKey(window, GLFW_KEY_F9) == GLFW_PRESS && fKeyStates[8] == GLFW_RELEASE) {
            // toggle blending
            std::cout << "Blending toggled " <<
                (!glIsEnabled(GL_BLEND) ? "on" : "off") << std::endl;
            if (glIsEnabled(GL_BLEND)) {
                glDisable(GL_BLEND);
            } else {
                glEnable(GL_BLEND);
            }
        }
        if (glfwGetKey(window, GLFW_KEY_F10) == GLFW_PRESS && fKeyStates[9] == GLFW_RELEASE) {
            BlinnMaterial::shader.reload();   // TODO: DEBUG, REMOVE!!
        }
        for (int i = 0; i < 10; i++) {
            fKeyStates[i] = glfwGetKey(window, GLFW_KEY_F1 + i);
        }

        game.update(delta);

        // mouse look
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        glfwSetCursorPos(window, centerX, centerY);
        game.cursorMoved(mouseX - centerX, mouseY - centerY);

        if (drawAsWireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        glm::vec3 splitFrustumCornersWorldSpace[4 * (NUM_SHADOW_MAPS + 1)];
        calculateSplitFrustumCornersWorldSpace(
            splitFrustumCornersWorldSpace, cascadeEnds, game.camera.getMatrix(), projectionMatrix
        );
        glm::mat4 shadowProjectionMatrices[NUM_SHADOW_MAPS];
        calculateShadowMappingProjectionMatrices(
            shadowProjectionMatrices, splitFrustumCornersWorldSpace, shadowMappingViewMatrix
        );

        const glm::vec3 viewFrustumNormals[4] = {
            // left plane normal (near left top, near left bottom, far left bottom)
            glm::normalize(glm::cross(
                splitFrustumCornersWorldSpace[0] - splitFrustumCornersWorldSpace[3],    // near left top - near left bottom
                splitFrustumCornersWorldSpace[7] - splitFrustumCornersWorldSpace[3]     // far left bottom - near left bottom
            )),

            // right plane normal (near right bottom, near right top, far right top)
            glm::normalize(glm::cross(
                splitFrustumCornersWorldSpace[2] - splitFrustumCornersWorldSpace[1],    // near right bottom - near right top
                splitFrustumCornersWorldSpace[5] - splitFrustumCornersWorldSpace[1]     // far right top - near right top
            )),

            // top plane normal (near right top, near left top, far left top)
            glm::normalize(glm::cross(
                splitFrustumCornersWorldSpace[1] - splitFrustumCornersWorldSpace[0],    // near right top - near left top
                splitFrustumCornersWorldSpace[4] - splitFrustumCornersWorldSpace[0]     // far left top - near left top
            )),

            // bottom plane normal (near left bottom, near right bottom, far right bottom)
            glm::normalize(glm::cross(
                splitFrustumCornersWorldSpace[3] - splitFrustumCornersWorldSpace[2],    // near left bottom - near right bottom
                splitFrustumCornersWorldSpace[6] - splitFrustumCornersWorldSpace[2]     // far right bottom - near right bottom
            ))
        };

        const float ds[4] = {
            -glm::dot(viewFrustumNormals[0], splitFrustumCornersWorldSpace[3]),
            -glm::dot(viewFrustumNormals[1], splitFrustumCornersWorldSpace[1]),
            -glm::dot(viewFrustumNormals[2], splitFrustumCornersWorldSpace[0]),
            -glm::dot(viewFrustumNormals[3], splitFrustumCornersWorldSpace[2]),
        };

        float cascadeEndsClipSpace[NUM_SHADOW_MAPS];
        glm::mat4 lightSpaceMatrices[NUM_SHADOW_MAPS];
        GLuint blurredShadowMaps[NUM_SHADOW_MAPS];
        if (shadowsActivated) {
            for (int i = 0; i < NUM_SHADOW_MAPS; i++) {
                cascadeEndsClipSpace[i] = (projectionMatrix * glm::vec4(0.0f, cascadeEnds[i + 1], 0.0f, 1.0f)).y;
            }
            for (int i = 0; i < NUM_SHADOW_MAPS; i++) {
                glPolygonOffset(4.0f, 0.0f);
                glViewport(0, 0, DEPTH_TEXTURE_WIDTH, DEPTH_TEXTURE_HEIGHT);

                glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMaps[i], 0);

                glClear(GL_DEPTH_BUFFER_BIT);
                glm::mat4 lightSpaceMatrix = shadowProjectionMatrices[i] * shadowMappingViewMatrix;
                lightSpaceMatrices[i] = ndcUVMappingMatrix * lightSpaceMatrix;
                shadowMappingDepthShader.use();
                shadowMappingDepthShader.setMatrix4("lightSpaceMatrix", lightSpaceMatrix);
                game.draw(shadowMappingDepthShader);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glDisable(GL_POLYGON_OFFSET_FILL);

                // blur just the closest shadow map
                if (i == 0) {
                    blurredShadowMaps[i] = blur(
                        DEPTH_TEXTURE_BLUR_WIDTH, DEPTH_TEXTURE_BLUR_HEIGHT, blurShadowMapFBOs[0], blurShadowMapFBOs[1],
                        blurShadowMapBuffers[0], blurShadowMapBuffers[1], shadowMaps[i], gaussianBlurShader4
                    );
                } else {
                    blurredShadowMaps[i] = shadowMaps[i];
                }
            }
        }

        glViewport(0, 0, width, height);
        // render to multisampled framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, multisampledFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthMask(GL_FALSE);
        skybox.draw(game.camera.getMatrix(), projectionMatrix);
        glDepthMask(GL_TRUE);
        Object3D::objectDrawCount = 0;
        game.draw(projectionMatrix, viewFrustumNormals, ds, ShadowInfo(
            lightSpaceMatrices, blurredShadowMaps, shadowsActivated ? NUM_SHADOW_MAPS : 0,
            cascadeEndsClipSpace
        ));
        ParticleSystem::draw(game.camera.getMatrix(), projectionMatrix);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // copy multisampled FBO to non-multisampled FBO
        for (GLuint i = 0; i < 2; i++) {
            blitFramebuffer(
                multisampledFBO, FBO, GL_COLOR_BUFFER_BIT, GL_COLOR_ATTACHMENT0 + i, GL_COLOR_ATTACHMENT0 + i,
                width, height, width, height, GL_NEAREST
            );
        }

        GLuint bloomColorBuffer;
        if (bloomActivated) {
            bloomColorBuffer = blur(
                BLOOM_BLUR_WIDTH, BLOOM_BLUR_HEIGHT, blurFBOs[0], blurFBOs[1],
                blurColorBuffers[0], blurColorBuffers[1], colorBuffers[1], gaussianBlurShader7
            );
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, width, height);

        // victory condition
        glBindFramebuffer(GL_FRAMEBUFFER, hudFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (glm::length(level.end - game.player.position) < 2) {
            TextRenderer::renderText(
                "You won",
                width / 2.0f - youDiedTextDimensions.x / 2.0f,
                height / 2.0f - youDiedTextDimensions.y / 2.0f,
                1.0f,
                glm::vec3(0.8f));
        } else if (game.player.isDead) {
            TextRenderer::renderText(
                "You died",
                width / 2.0f - youDiedTextDimensions.x / 2.0f,
                height / 2.0f - youDiedTextDimensions.y / 2.0f,
                1.0f,
                glm::vec3(0.8f));
        }

        if (displayPerformanceStats) {
            softFps = (softFps * 9 + (1.0f / delta)) / 10;
            softFrameTime = (softFrameTime * 9 + round(delta * 100000) / 100) / 10;
            TextRenderer::renderText(
                "fps: " + std::to_string(softFps),
                0.0f,
                height - 20.0f,
                0.1f,
                glm::vec3(0.8f));
            TextRenderer::renderText(
                "frametime (ms): " + std::to_string(softFrameTime),
                0.0f,
                height - 50.0f,
                0.1f,
                glm::vec3(0.8f));
            TextRenderer::renderText(
                "Objects drawn: " + std::to_string(Object3D::objectDrawCount) + "/" + std::to_string(level.getTotalObjectCount()),
                0.0f,
                height - 80.0f,
                0.1f,
                glm::vec3(0.8f));
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        postProcessingShader.use();
        postProcessingShader.setTexture2D("mainImage", GL_TEXTURE0, colorBuffers[0], 0);
        if (bloomActivated) {
            postProcessingShader.setTexture2D("brightSpotsBloomImage", GL_TEXTURE1, bloomColorBuffer, 1);
        }
        postProcessingShader.setTexture2D("hudTexture", GL_TEXTURE2, hudColorBuffer, 2);
        drawScreenQuad();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void calculateSplitFrustumCornersWorldSpace(
    glm::vec3 frustumCornersWorldSpace[], const float *cascadeEnds,
    const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix
) {
    for (int k = 0; k < NUM_SHADOW_MAPS + 1; k++) {
        // cascade near and far plane values from view to clip space
        glm::vec4 tmp = projectionMatrix * glm::vec4(0.0f, 0.0f, -cascadeEnds[k], 1.0f);
        float clipSpaceNear = tmp.z / tmp.w;

        glm::vec3 frustumCornersClipSpace[4] = {
            // near plane
            glm::vec3(-1.0f, 1.0f, clipSpaceNear),      // left top
            glm::vec3(1.0f, 1.0f, clipSpaceNear),       // right top
            glm::vec3(1.0f, -1.0f, clipSpaceNear),      // right bottom
            glm::vec3(-1.0f, -1.0f, clipSpaceNear)      // left bottom
        };

        for (int i = 0; i < 4; i++) {
            glm::vec4 frustumCorner = glm::inverse(projectionMatrix * viewMatrix) 
                * glm::vec4(frustumCornersClipSpace[i], 1.0f);
            frustumCornersWorldSpace[i + 4 * k] = glm::vec3(frustumCorner / frustumCorner.w);
        }
    }
}

void calculateShadowMappingProjectionMatrices(
    glm::mat4 shadowMappingProjectionMatrices[], glm::vec3 frustumCornersWorldSpace[],
    const glm::mat4 &shadowMappingViewMatrix
) {
    for (int k = 0; k < NUM_SHADOW_MAPS; k++) {
        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::min();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::min();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::min();

        for (int i = 0; i < 8; i++) {
            glm::vec3 frustumCornerShadowSpace = shadowMappingViewMatrix * glm::vec4(frustumCornersWorldSpace[i + k * 4], 1.0f);
            minX = std::min(minX, frustumCornerShadowSpace.x);
            maxX = std::max(maxX, frustumCornerShadowSpace.x);
            minY = std::min(minY, frustumCornerShadowSpace.y);
            maxY = std::max(maxY, frustumCornerShadowSpace.y);
            minZ = std::min(minZ, frustumCornerShadowSpace.z);
            maxZ = std::max(maxZ, frustumCornerShadowSpace.z);
        }

        shadowMappingProjectionMatrices[k] = glm::ortho(minX, maxX, minY, maxY, -10.0f, 20.0f);
    }
}

GLuint blur(
    int width, int height, GLuint blurFBO0, GLuint blurFBO1, GLuint blurBuffer0,
    GLuint blurBuffer1, GLuint inputBuffer, Shader &shader
) {
    // gaussian blur
    shader.use();
    shader.setInteger("imageHeight", height);
    shader.setInteger("imageWidth", width);

    // blur horizontally
    glBindFramebuffer(GL_FRAMEBUFFER, blurFBO1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);
    shader.setInteger("horizontalBlur", 1);
    shader.setTexture2D("image", GL_TEXTURE0, inputBuffer, 0);     // colorBuffers[1]
    drawScreenQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // blur vertically
    glBindFramebuffer(GL_FRAMEBUFFER, blurFBO0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);
    shader.setInteger("horizontalBlur", 0);
    shader.setTexture2D("image", GL_TEXTURE0, blurBuffer1, 0);     // blurColorBuffers[1]
    drawScreenQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return blurBuffer0;
}

void generateFBO(GLuint &FBO, GLuint* colorBuffers, int width, int height, int numColorAttachments, bool isMultisampled, bool attachDepthRBO) {
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glGenTextures(numColorAttachments, colorBuffers);
    GLuint *colorAttachments = new GLuint[numColorAttachments];
    for (GLuint i = 0; i < numColorAttachments; i++) {
        if (isMultisampled) {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorBuffers[i]);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, AA_SAMPLES, GL_RGB8, width, height, GL_TRUE);
        } else {
            glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        if (isMultisampled) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, colorBuffers[i], 0);
        } else {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
        }

        colorAttachments[i] = GL_COLOR_ATTACHMENT0 + i;
    }

    if (attachDepthRBO) {
        GLuint depthRBO;
        glGenRenderbuffers(1, &depthRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        if (isMultisampled) {
            glRenderbufferStorageMultisample(
                GL_RENDERBUFFER, AA_SAMPLES, GL_DEPTH_COMPONENT, width, height
            );
        } else {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        }
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);
    }

    // tell OpenGL to draw to all color buffers
    glDrawBuffers(numColorAttachments, colorAttachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void blitFramebuffer(GLuint srcFBO, GLuint destFBO,  GLbitfield mask, GLenum readBuffer, GLenum drawBuffer,
    GLuint srcWidth, GLuint srcHeight, GLuint destWidth, GLuint destHeight, GLenum filter)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destFBO);
    glDrawBuffer(drawBuffer);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFBO);
    glReadBuffer(readBuffer);
    glBlitFramebuffer(0, 0, srcWidth, srcHeight, 0, 0, destWidth, destHeight, mask, filter);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint loadTexture(const char* filename, GLfloat fLargest) {
    GLint textureQuality;
    if (textureSamplingQuality == TextureSamplingQuality::NEAREST_NEIGHBOR) {
        if (mipMappingQuality == MipMappingQuality::OFF) {
            textureQuality = GL_NEAREST;
        } else if (mipMappingQuality == MipMappingQuality::NEAREST_NEIGHBOR) {
            textureQuality = GL_NEAREST_MIPMAP_NEAREST;
        } else if (mipMappingQuality == MipMappingQuality::LINEAR) {
            textureQuality = GL_NEAREST_MIPMAP_LINEAR;
        }
    } else {
        if (mipMappingQuality == MipMappingQuality::OFF) {
            textureQuality = GL_LINEAR;
        } else if (mipMappingQuality == MipMappingQuality::NEAREST_NEIGHBOR) {
            textureQuality = GL_LINEAR_MIPMAP_NEAREST;
        } else if (mipMappingQuality == MipMappingQuality::LINEAR) {
            textureQuality = GL_LINEAR_MIPMAP_LINEAR;
        }
    }

    GLuint texture;
    glGenTextures(1, &texture);

    int width, height;
    auto image = SOIL_load_image(
        filename,
        &width, &height, nullptr, SOIL_LOAD_RGB
    );
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, width, height,
        0, GL_RGB, GL_UNSIGNED_BYTE, image
    );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureQuality);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);

    SOIL_free_image_data(image);

    return texture;
}

void drawScreenQuad() {
    if (screenQuadVAO == 0)     {
        GLfloat quadVertices[] = {
            // Positions            // Texture Coords
            -1.0f,  1.0f, 0.0f,     0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,
             1.0f,  1.0f, 0.0f,     1.0f, 1.0f,
             1.0f, -1.0f, 0.0f,     1.0f, 0.0f,
        };

        // create and bind VAO
        glGenVertexArrays(1, &screenQuadVAO);
        glBindVertexArray(screenQuadVAO);

        // create and bind VBO
        GLuint quadVBO;
        glGenBuffers(1, &quadVBO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

        // copy vertices array to VBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        // set vertex attribute pointers
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

        // unbind VAO
        glBindVertexArray(0);
    }

    glBindVertexArray(screenQuadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

GLFWwindow *initGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    //glfwWindowHint(GLFW_SAMPLES, 4);

    //GLFWwindow *window = glfwCreateWindow(width, height, title, glfwGetPrimaryMonitor(), nullptr);
    GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    glfwMakeContextCurrent(window);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return window;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return window;
}

bool initGLEW() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    return true;
}

void loadPlatformTypesTextures(
    std::map<std::string, PlatformType> &platformTypes,
    const nlohmann::json &platformTypesJson,
    Level &level
) {
    for (auto &platformType : platformTypesJson) {
        platformTypes.at(platformType["name"].get<std::string>()).colorTexture =
            loadTexture((
                "textures/" +
                platformType.value<std::string>("colorTexture", "")
                ).c_str(), fLargest);

        platformTypes.at(platformType["name"].get<std::string>()).linesTexture =
            loadTexture((
                "textures/" +
                platformType.value<std::string>("linesTexture", "")
                ).c_str(), fLargest);

        platformTypes.at(platformType["name"].get<std::string>()).size.x = 100.0f;
    }

    for (int i = 0; i < level.platforms.size(); i++) {
        level.platforms.at(i).reloadTexture();
    }
}
