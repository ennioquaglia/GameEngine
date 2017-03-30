#include "VAO.h"
#include "Debug.h"

namespace Engine
{
	VAO::VAO(std::vector<StaticMeshVertexData> &data, std::vector<GLuint> &indices) :indices_id(0), vbo_id(0), vao_id(0), numIndices(0)
	{

		glGenVertexArrays(1, &vao_id); // Create our Vertex Array Object
		glBindVertexArray(vao_id); // Bind our Vertex Array Object so we can use it


		glGenBuffers(1, &indices_id);
		numIndices = indices.size();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);


		glGenBuffers(1, &vbo_id);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
		glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(StaticMeshVertexData), &data[0], GL_STATIC_DRAW);


		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, sizeof(StaticMeshVertexData), 0); // Set up our vertex attributes pointer
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_TRUE, sizeof(StaticMeshVertexData), (void*)(3 * sizeof(GLfloat))); // Normal
		glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, sizeof(StaticMeshVertexData), (void*)(6 * sizeof(GLfloat))); // Texture
		glVertexAttribPointer((GLuint)3, 3, GL_FLOAT, GL_FALSE, sizeof(StaticMeshVertexData), (void*)(8 * sizeof(GLfloat))); // Tangent

		glEnableVertexAttribArray(0); // Disable our Vertex Array Object
        glBindVertexArray(0); // Disable our Vertex Buffer Object

        glDebug();
        QENGINE_INFO("VAO generated, index="+vao_id);
	}



	VAO::VAO(std::vector<SkeletalMeshVertexData> &data, std::vector<GLuint> &indices) :indices_id(0), vbo_id(0), vao_id(0), numIndices(0)
	{
		glGenVertexArrays(1, &vao_id); // Create our Vertex Array Object
		glBindVertexArray(vao_id); // Bind our Vertex Array Object so we can use it


		glGenBuffers(1, &indices_id);
		numIndices = indices.size();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);


		glGenBuffers(1, &vbo_id);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
		glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(SkeletalMeshVertexData), &data[0], GL_STATIC_DRAW);


		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);

		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, sizeof(SkeletalMeshVertexData), 0); // Set up our vertex attributes pointer
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_TRUE, sizeof(SkeletalMeshVertexData), (void*)(3 * sizeof(GLfloat))); // Normal
		glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, sizeof(SkeletalMeshVertexData), (void*)(6 * sizeof(GLfloat))); // Texture
		glVertexAttribPointer((GLuint)3, 3, GL_FLOAT, GL_FALSE, sizeof(SkeletalMeshVertexData), (void*)(8 * sizeof(GLfloat))); // Tangent
		glVertexAttribPointer((GLuint)4, 4, GL_FLOAT, GL_FALSE, sizeof(SkeletalMeshVertexData), (void*)(11 * sizeof(GLfloat))); // boneWeights
		glVertexAttribPointer((GLuint)5, 4, GL_FLOAT, GL_FALSE, sizeof(SkeletalMeshVertexData), (void*)(15 * sizeof(GLfloat))); // boneIDs

		glEnableVertexAttribArray(0); // Disable our Vertex Array Object
		glBindVertexArray(0); // Disable our Vertex Buffer Object
	}




}
