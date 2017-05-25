#include <iostream>
#include <chrono>
#include <memory>

#include <json/json.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <SOIL.h>

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

static int width = 1280, height = 720;
static const char *title = "Gi's Awakening: The Mending of the Sky";
static GLuint screenQuadVAO = 0;
const int AA_SAMPLES = 4;

bool initGLEW();
GLFWwindow *initGLFW();
void drawScreenQuad();
void generateFBO(GLuint &FBO, GLuint* colorBuffers, int numColorAttachments, bool isMultisampled, bool attachDepthRBO);
void blitFramebuffer(GLuint srcFBO, GLuint destFBO, GLbitfield mask, GLenum readBuffer, GLenum drawBuffer,
    GLuint srcWidth, GLuint srcHeight, GLuint destWidth, GLuint destHeight, GLenum filter);

int main(void) {
    GLFWwindow* window = initGLFW();
    if (window == NULL) {
        return 0;
    }

    // Set OpenGL options
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // activate v-sync
    glfwSwapInterval(1);

    if (!initGLEW()) {
        return 0;
    }

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glm::mat4 projectionMatrix = glm::perspectiveFov(
        glm::radians(70.0f),
        static_cast<float>(width),
        static_cast<float>(height),
        0.1f, 100.0f
    );

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    GLuint multisampledFBO;
    GLuint multisampledColorBuffers[2];
    generateFBO(multisampledFBO, multisampledColorBuffers, 2, false, true);

    GLuint FBO;
    GLuint colorBuffers[2];
    generateFBO(FBO, colorBuffers, 2, false, true);

    GLuint blurFBOs[2];
    GLuint blurColorBuffers[2];
    generateFBO(blurFBOs[0], &blurColorBuffers[0], 1, false, false);
    generateFBO(blurFBOs[1], &blurColorBuffers[1], 1, false, false);

    GLuint hudFBO;
    GLuint hudColorBuffer;
    generateFBO(hudFBO, &hudColorBuffer, 1, false, false);

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
        platformTypes.emplace(platformType["name"].get<std::string>(), PlatformType {
            glm::vec3(platformType["size"][0], platformType["size"][1], platformType["size"][2]),
            Mesh::fromFile(("geometry/" + platformType["name"].get<std::string>() + ".vbo").c_str())
        });
    }

    Mesh endMesh = Mesh::fromFile("geometry/End.vbo");
    Level level = Level::fromFile("levels/level1.gil", material.get(), endMesh, platformTypes);
    Game game(level);

    Shader textShader = Shader("shaders/textShader.vert", "shaders/textShader.frag");
    TextRenderer::init(width, height, "fonts/Gidole-Regular.ttf", textShader);
    Shader gaussianBlurShader = Shader("shaders/gaussianBlur.vert", "shaders/gaussianBlur.frag");
    Shader postProcessingShader = Shader("shaders/postProcessing.vert", "shaders/postProcessing.frag");
    ParticleSystem::init();

    glm::vec3 youDiedTextDimensions = TextRenderer::calcDimensions("You died", 1.0f);

    int centerX = width / 2, centerY = height / 2;
    glfwSetCursorPos(window, centerX, centerY);

    double time = glfwGetTime();
    double previousTime = time;

    bool rightMouseButtonPressed = false;
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double delta = currentTime - previousTime;
        previousTime = currentTime;
        
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

        game.update(delta);
        
        // mouse look
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        glfwSetCursorPos(window, centerX, centerY);
        game.cursorMoved(mouseX - centerX, mouseY - centerY);

        // render to multisampled framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, multisampledFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthMask(GL_FALSE);
        skybox.draw(game.camera.getMatrix(), projectionMatrix);
        glDepthMask(GL_TRUE);
        game.draw(projectionMatrix);
        ParticleSystem::draw(delta, game.camera.getMatrix(), projectionMatrix);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // copy multisampled FBO to non-multisampled FBO
        for (GLuint i = 0; i < 2; i++) {
            blitFramebuffer(
                multisampledFBO, FBO, GL_COLOR_BUFFER_BIT, GL_COLOR_ATTACHMENT0 + i, GL_COLOR_ATTACHMENT0 + i,
                width, height, width, height, GL_NEAREST
            );
        }

        // downscale FBOs used for blurring
        const int scaledDownWidth = 256;
        const int scaledDownHeight = 144;
        for (GLuint i = 0; i < 2; i++) {
            blitFramebuffer(
                blurFBOs[i], blurFBOs[i], GL_COLOR_BUFFER_BIT, GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT0,
                width, height, scaledDownWidth, scaledDownHeight, GL_NEAREST
            );
        }

        // copy color attachment 1 (bright parts of the image) to the first blur FBO
        blitFramebuffer(
            FBO, blurFBOs[0], GL_COLOR_BUFFER_BIT, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT0,
            width, height, scaledDownWidth, scaledDownHeight, GL_NEAREST
        );

        // gaussian blur
        gaussianBlurShader.use();
        gaussianBlurShader.setInteger("imageHeight", height);
        gaussianBlurShader.setInteger("imageWidth", width);

        // blur horizontally
        glBindFramebuffer(GL_FRAMEBUFFER, blurFBOs[1]);
        gaussianBlurShader.setInteger("horizontalBlur", 1);
        gaussianBlurShader.setTexture2D("image", GL_TEXTURE0, blurColorBuffers[0], 0);
        drawScreenQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // blur vertically
        glBindFramebuffer(GL_FRAMEBUFFER, blurFBOs[0]);
        gaussianBlurShader.setInteger("horizontalBlur", 0);
        gaussianBlurShader.setTexture2D("image", GL_TEXTURE0, blurColorBuffers[1], 0);
        drawScreenQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // copy blurred FBO to FBO used for drawing to the screen
        blitFramebuffer(
            blurFBOs[0], FBO, GL_COLOR_BUFFER_BIT, GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
            scaledDownWidth, scaledDownHeight, width, height, GL_LINEAR
        );

        // post processing - combine the blurred and the main image
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // victory condition
        if (glm::length(level.end - game.player.position) < 2) {
            glBindFramebuffer(GL_FRAMEBUFFER, hudFBO);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            TextRenderer::renderText(
                "You won",
                width / 2.0f - youDiedTextDimensions.x / 2.0f,
                height / 2.0f - youDiedTextDimensions.y / 2.0f,
                1.0f,
                glm::vec3(0.8f));
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        } else if (game.player.isDead) {
            glBindFramebuffer(GL_FRAMEBUFFER, hudFBO);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            TextRenderer::renderText(
                "You died",
                width / 2.0f - youDiedTextDimensions.x / 2.0f,
                height / 2.0f - youDiedTextDimensions.y / 2.0f,
                1.0f,
                glm::vec3(0.8f));
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, hudFBO);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        postProcessingShader.use();
        postProcessingShader.setTexture2D("mainImage", GL_TEXTURE0, colorBuffers[0], 0);
        postProcessingShader.setTexture2D("brightSpotsBloomImage", GL_TEXTURE1, colorBuffers[1], 1);
        postProcessingShader.setTexture2D("hudTexture", GL_TEXTURE2, hudColorBuffer, 2);
        drawScreenQuad();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void generateFBO(GLuint &FBO, GLuint* colorBuffers, int numColorAttachments, bool isMultisampled, bool attachDepthRBO) {
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
    glfwWindowHint(GLFW_SAMPLES, 4);

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
