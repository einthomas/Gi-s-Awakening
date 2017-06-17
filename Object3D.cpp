#include "Object3D.h"

#include <glm/gtc/matrix_transform.hpp>

int Object3D::objectDrawCount = 0;
bool Object3D::frustumCullingEnabled = true;
GLuint Object3D::cubeVAO = static_cast<GLuint>(-1);
GLuint Object3D::skyboxCubeVAO = static_cast<GLuint>(-1);

static GLfloat boxVertices[] = {
    // Positions          // Texture Coords // Normals
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,        0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,        0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,        0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,        0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,        0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,        0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,        0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,        0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,        0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,        0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,        0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,        0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,       -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,       -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,       -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,       -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,       -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,       -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,        1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,        1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,        1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,        1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,        1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,        1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,        0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,        0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,        0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,        0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,        0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,        0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,        0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,        0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,        0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,        0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,        0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,        0.0f,  1.0f,  0.0f,
};

static GLfloat skyboxVertices[] = {
    // Positions
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,

     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,

     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,

     0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
};

Object3D::Object3D(
    Material *material, const glm::vec3 &position, const glm::vec3 &scale,
    const glm::vec3 &size, const Mesh &mesh,
    GLfloat lightMapScale, glm::vec2 lightMapPosition
) :
    material(material), position(position), rotation(glm::vec3(0)),
    scale(scale), size(size), mesh(mesh),
    lightMapScale(lightMapScale), lightMapPosition(lightMapPosition)
{
}

Object3D Object3D::makeCube(
    Material *material, const glm::vec3 &position, const glm::vec3 &scale
) {
    if (cubeVAO == static_cast<GLuint>(-1)) {
        // create and bind VAO
        glGenVertexArrays(1, &cubeVAO);
        glBindVertexArray(cubeVAO);

        // create and bind VBO
        GLuint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // copy vertices array to VBO
        glBufferData(
            GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW
        );

        // set vertex attribute pointers
        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE,
            8 * sizeof(GLfloat), static_cast<GLvoid*>(0)
        );
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            1, 3, GL_FLOAT, GL_FALSE,
            8 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(5 * sizeof(GLfloat))
        );
        glEnableVertexAttribArray(1);

        // unbind VAO
        glBindVertexArray(0);
    }

    return Object3D(
        material, position, scale, scale, Mesh(cubeVAO, sizeof(boxVertices) / 8 / 4)
    );
}

Object3D Object3D::makeSkyboxCube(
    Material *material, const glm::vec3 &position, const glm::vec3 &scale
) {
    if (skyboxCubeVAO == static_cast<GLuint>(-1)) {
        // create and bind VAO
        glGenVertexArrays(1, &skyboxCubeVAO);
        glBindVertexArray(skyboxCubeVAO);

        // create and bind VBO
        GLuint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // copy vertices array to VBO
        glBufferData(
            GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices,
            GL_STATIC_DRAW
        );

        // set vertex attribute pointers
        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE,
            3 * sizeof(GLfloat), static_cast<GLvoid*>(0)
        );
        glEnableVertexAttribArray(0);

        // unbind VAO
        glBindVertexArray(0);
    }

    return Object3D(
        material, position, scale, scale,
        Mesh(skyboxCubeVAO, sizeof(skyboxVertices) / 3 / 4)
    );
}

void Object3D::draw(const Shader &shader) {
    shader.setMatrix4("model", calculateModelMatrix());
    mesh.draw();
}

void Object3D::draw(
    const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix
) {
    material->bind(viewMatrix, projectionMatrix, calculateModelMatrix());
    mesh.draw();
}

void Object3D::draw(
    RenderInfo renderInfo, ShadowInfo shadowInfo, GLuint lightMap
) {
    int outside = 0;
    int inside = 0;
    if (Object3D::frustumCullingEnabled) {
        glm::vec3 sizeFactors[8] = {
            glm::vec3(-0.5f, -0.5f, -0.5f),
            glm::vec3(-0.5f, -0.5f,  0.5f),

            glm::vec3(-0.5f,  0.5f, -0.5f),
            glm::vec3(-0.5f,  0.5f,  0.5f),

            glm::vec3(0.5f, -0.5f, -0.5f),
            glm::vec3(0.5f, -0.5f,  0.5f),

            glm::vec3(0.5f,  0.5f, -0.5f),
            glm::vec3(0.5f,  0.5f,  0.5f),
        };

        for (int i = 0; i < 4; i++) {
            outside = 0;
            inside = 0;
            for (int k = 0; k < 8 && (outside == 0 || inside == 0); k++) {
                if (glm::dot(renderInfo.viewFrustumNormals[i], position + size * sizeFactors[k])
                    + renderInfo.ds[i] < 0
                    ) {
                    outside++;
                } else {
                    inside++;
                }
            }

            // object is outside -> return
            if (inside == 0) {
                return;
            }
        }
    }

    if (!Object3D::frustumCullingEnabled || inside != 0) {
        Object3D::objectDrawCount++;
        material->bind(
            renderInfo, shadowInfo, calculateModelMatrix(),
            lightMapScale, lightMapPosition, lightMap
        );
        mesh.draw();
    }
}

glm::mat4 Object3D::calculateModelMatrix() {
    glm::mat4 modelMatrix;
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1, 0, 0));
    modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0, 0, 1));
    modelMatrix = glm::scale(modelMatrix, scale);

    return modelMatrix;
}
