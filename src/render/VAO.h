#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>

namespace Engine
{
    struct StaticMeshVertexData {
        GLfloat position[3];
        GLfloat normal[3];
        GLfloat textureCoord[2];
        GLfloat tangent[3];
    };
    struct SkeletalMeshVertexData {
        GLfloat position[3];
        GLfloat normal[3];
        GLfloat textureCoord[2];
        GLfloat tangent[3];
        GLfloat boneWeights[4];
        GLfloat boneIDs[4];
    };

    struct VAO {

        VAO(std::vector<StaticMeshVertexData> &data, std::vector<GLuint> &indices);
        VAO(std::vector<SkeletalMeshVertexData> &data, std::vector<GLuint> &indices);
        ~VAO() {}

        inline void draw() {
            glBindVertexArray(vao_id); // Bind our Vertex Array Object
            glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
        }



        GLuint indices_id;
        GLuint vbo_id, vao_id;

        GLuint numIndices;
    };
}
