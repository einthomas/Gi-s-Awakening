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

static int width = 1280, height = 720;
static const char *title = "Gi's Awakening: The Mending of the Sky";
GLuint screenQuadVAO = 0;
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
    generateFBO(multisampledFBO, multisampledColorBuffers, 2, true, true);

    GLuint FBO;
    GLuint colorBuffers[2];
    generateFBO(FBO, colorBuffers, 2, false, true);

    GLuint blurFBOs[2];
    GLuint blurColorBuffers[2];
    generateFBO(blurFBOs[0], &blurColorBuffers[0], 1, false, false);
    generateFBO(blurFBOs[1], &blurColorBuffers[1], 1, false, false);

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
    std::unique_ptr<BlinnMaterial> material(new BlinnMaterial(glm::vec3(1.0f), glm::vec3(0.0f), 0.0f));

    std::map<std::string, PlatformType> platformTypes; // TODO
    nlohmann::json platformTypesJson;
    std::ifstream platformTypesFile("geometry/set1.gib");
    platformTypesFile >> platformTypesJson;

    for (auto &platformType : platformTypesJson) {
        platformTypes.emplace(platformType["name"].get<std::string>(), PlatformType {
            glm::vec3(platformType["size"][0], platformType["size"][1], platformType["size"][2]),
            Mesh::fromFile(("geometry/" + platformType["name"].get<std::string>() + ".vbo").c_str())
        });
    }

    Level level = Level::fromFile("levels/level0.gil", material.get(), platformTypes);
    Player player(level.start + glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.5f, 0.5f, 2.0f));
    Camera camera(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(glm::radians(90.0f), 0.0f, level.startOrientation));
    Shader textShader = Shader("shaders/textShader.vert", "shaders/textShader.frag");
    TextRenderer::init(width, height, "fonts/Gidole-Regular.ttf", textShader);
    Shader gaussianBlurShader = Shader("shaders/gaussianBlur.vert", "shaders/gaussianBlur.frag");
    Shader postProcessingShader = Shader("shaders/postProcessing.vert", "shaders/postProcessing.frag");

    int centerX = width / 2, centerY = height / 2;
    glfwSetCursorPos(window, centerX, centerY);

    float gravity = 12.0f;
    float rotationSpeed = glm::radians(14.0f);
    float projectileSpeed = 12.0f;

    double time = glfwGetTime();
    double previousTime = time;

    bool mousePressed = false;
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double delta = currentTime - previousTime;
        previousTime = currentTime;

        // movement
        glm::vec2 movement(0);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            movement.x -= std::sin(camera.rotation.z);
            movement.y += std::cos(camera.rotation.z);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            movement.x -= std::cos(camera.rotation.z);
            movement.y -= std::sin(camera.rotation.z);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            movement.x += std::sin(camera.rotation.z);
            movement.y -= std::cos(camera.rotation.z);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            movement.x += std::cos(camera.rotation.z);
            movement.y += std::sin(camera.rotation.z);
        }

        // shooting
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            mousePressed = true;
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
            if (mousePressed) {
                mousePressed = false;
                glm::vec3 cameraDirection = camera.getDirection();
                player.shoot(Projectile(
                    material.get(),
                    player.position + player.size / 4.0f + cameraDirection * 0.5f,
                    cameraDirection * projectileSpeed
                ));
            }
        }

        // jumping
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            player.jumpPressed(delta);
        } else {
            player.jumpReleased();
        }

        player.update(delta, gravity, movement, level);

        camera.position = player.position + glm::vec3(0.f, 0.f, 0.5f);
        
        // mouse look
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        glfwSetCursorPos(window, centerX, centerY);

        camera.rotation.z -= (mouseX - centerX) * rotationSpeed * delta;
        camera.rotation.x -= (mouseY - centerY) * rotationSpeed * delta;
        camera.rotation.x = glm::clamp(camera.rotation.x, 0.f, glm::pi<float>());

        // render to multisampled framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, multisampledFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthMask(GL_FALSE);
        skybox.draw(camera.getMatrix(), projectionMatrix);
        glDepthMask(GL_TRUE);
        player.draw(camera.getMatrix(), projectionMatrix);
        level.draw(camera.getMatrix(), projectionMatrix);
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
        postProcessingShader.use();
        postProcessingShader.setTexture2D("mainImage", GL_TEXTURE0, colorBuffers[0], 0);
        postProcessingShader.setTexture2D("brightSpotsBloomImage", GL_TEXTURE1, colorBuffers[1], 1);
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
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, AA_SAMPLES, GL_RGBA8, width, height, GL_TRUE);
        } else {
            glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
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
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, AA_SAMPLES, GL_DEPTH_COMPONENT, width, height);
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
