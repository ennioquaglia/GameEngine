#include "StaticMesh.h"
#include "../common/utility.h"
#include "Shader.h"

using namespace Engine;

void StaticMesh_330::draw(MaterialList m, Shader *s, uint flag)
{
	glDebug();
//	s->Use();

	if(flag == RenderFlag::Default) {
		

		if(m.empty()) {
			Material *mat = Material::getDefaultMaterial();
			if(s) {
				//mat->updateShader(s);
				mat->setShader(s, 1);
				glDebug();
			}
			//mat->bind();
			m.push_back(mat);
		}
		MaterialList::iterator mit = m.begin();
		
		glDebug();
		for(glm::uint i = 0; i < VAOs.size(); i++) {
			if(mit != m.end()) {
				if(s){
					(*mit)->updateShader(s);
					glDebug(flag);   	
				}
				(*mit)->bind();
				glDebug(flag);
				mit++;
			}
			VAOs[i].draw();
		}
		glDebug();
		glBindVertexArray(0); // Unbind our Vertex Array Object
		glDebug();
	} else if(flag == RenderFlag::Shadow) {
		glDebug();
		for(glm::uint i = 0; i < VAOs.size(); i++) {
			VAOs[i].draw();
		}
		glDebug();
	}
	glBindVertexArray(0); // Unbind our Vertex Array Object
}

bool StaticMesh_330::Import(std::string filename)
{
	//Debug("330 static mesh importing...");
	const aiScene* MeshScene = aiImportFile(filename.data(), aiProcessPreset_TargetRealtime_Quality | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);

	if(!MeshScene) {
		//std::cout << "cannot load the file at "<<filename<<"\n";//importer.GetErrorString() << std::endl;

		Engine::error(Engine::EString() + "cannot load the file at " + filename);
		Engine::consolePause();
	} else {
		Engine::debug(Engine::EString() + MeshScene->mNumMeshes + " models loaded!");
	}

	
	for(GLuint i = 0; i < MeshScene->mNumMeshes; i++) {

		GLuint vaoId;

		glDebug("StaticMesh::import opengl error");

		std::vector<GLuint> indices;
		const struct aiMesh* mesh;
		mesh = MeshScene->mMeshes[i];
		for(GLuint j = 0; j < mesh->mNumFaces; j++) {
			const struct aiFace* face = &mesh->mFaces[j];
			indices.push_back(face->mIndices[0]);
			indices.push_back(face->mIndices[1]);
			indices.push_back(face->mIndices[2]);

		}

		std::vector<StaticMeshVertexData> vertex_data;
		vertex_data.resize(3 * mesh->mNumFaces);
		memset(&vertex_data[0], 0, vertex_data.size()*sizeof(StaticMeshVertexData));

		if(mesh->HasPositions()) {
			for(GLuint j = 0; j < mesh->mNumVertices; j++) {
				vertex_data[j].position[0] = mesh->mVertices[j].x;
				vertex_data[j].position[1] = mesh->mVertices[j].y;
				vertex_data[j].position[2] = mesh->mVertices[j].z;
			}
		}

		if(mesh->HasNormals()) {
			for(GLuint j = 0; j < mesh->mNumVertices; j++) {
				vertex_data[j].normal[0] = mesh->mNormals[j].x;
				vertex_data[j].normal[1] = mesh->mNormals[j].y;
				vertex_data[j].normal[2] = mesh->mNormals[j].z;
			}
		}

		if(mesh->HasTextureCoords(0)) {
			for(GLuint j = 0; j < mesh->mNumVertices; j++) {
				vertex_data[j].textureCoord[0] = mesh->mTextureCoords[0][j].x;
				vertex_data[j].textureCoord[1] = mesh->mTextureCoords[0][j].y;
			}
		}

		if(mesh->HasTangentsAndBitangents()) {
			for(GLuint j = 0; j < mesh->mNumVertices; j++) {
				vertex_data[j].tangent[0] = mesh->mTangents[j][0];
				vertex_data[j].tangent[1] = mesh->mTangents[j][1];
				vertex_data[j].tangent[2] = mesh->mTangents[j][2];
			}
		}
		VAOs.push_back(VAO(vertex_data,indices)); 

	}

}


void StaticMesh_330_old::draw(MaterialList m, Shader *s, uint flag)
{

	if(flag == RenderFlag::Default) {
		

		if(m.empty()) {
			Material *mat = Material::getDefaultMaterial();
			if(s) {
				//mat->updateShader(s);
				mat->setShader(s, 1);
			}
			//mat->bind();
			m.push_back(mat);
		}
		MaterialList::iterator mit = m.begin();
		
		glDebug();
		for(glm::uint i = 0; i < vao.size(); i++) {
			if(mit != m.end()) {
				if(s){
					(*mit)->updateShader(s);
					glDebug(flag);   	
				}
				(*mit)->bind();
				glDebug(flag);
				mit++;
			}
			glDebug(flag);
			glBindVertexArray(vao[i]); // Bind our Vertex Array Object
			glDebug();
			glDrawElements(GL_TRIANGLES, num_indices[i], GL_UNSIGNED_INT, 0);
			glDebug();

		}
		glDebug();
		glBindVertexArray(0); // Unbind our Vertex Array Object
		glDebug();
	} else if(flag == RenderFlag::Shadow) {
		glDebug();
		for(glm::uint i = 0; i < vao.size(); i++) {

			glBindVertexArray(vao[i]); // Bind our Vertex Array Object
			glDebug();
			glDrawElements(GL_TRIANGLES, num_indices[i], GL_UNSIGNED_INT, 0);
			glDebug();
		}
		glDebug();
	}
	glBindVertexArray(0); // Unbind our Vertex Array Object
}
 
bool StaticMesh_330_old::Import(std::string filename)
{
	//Debug("330 static mesh importing...");
	const aiScene* MeshScene = aiImportFile(filename.data(), aiProcessPreset_TargetRealtime_Quality | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);

	if(!MeshScene) {
		//std::cout << "cannot load the file at "<<filename<<"\n";//importer.GetErrorString() << std::endl;

		Engine::error(Engine::EString() + "cannot load the file at " + filename);
		Engine::consolePause();
		exit(1);
	} else {
		Engine::debug(Engine::EString() + MeshScene->mNumMeshes + " models loaded!");
	}

	//vao.resize(MeshScene->mNumMeshes);
	for(GLuint i = 0; i < MeshScene->mNumMeshes; i++) {

		GLuint vaoId;

		int error = glGetError();
		if(error)Debug("StaticMesh::import opengl error:" + error);

		glGenVertexArrays(1, &vaoId);

		glBindVertexArray(vaoId);
		vao.push_back(vaoId);

		std::vector<GLuint> indices;
		const struct aiMesh* mesh;
		mesh = MeshScene->mMeshes[i];
		for(GLuint j = 0; j < mesh->mNumFaces; j++) {
			const struct aiFace* face = &mesh->mFaces[j];
			indices.push_back(face->mIndices[0]);
			indices.push_back(face->mIndices[1]);
			indices.push_back(face->mIndices[2]);

		}
		num_indices.push_back(indices.size());

		GLuint vertex_indices;
		glGenBuffers(1, &vertex_indices);
		index_buffers.push_back(vertex_indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

		std::vector<VertexData> vertex_data;
		vertex_data.resize(3 * mesh->mNumFaces);
		memset(&vertex_data[0], 0, vertex_data.size()*sizeof(VertexData));

		if(mesh->HasPositions()) {
			for(GLuint j = 0; j < mesh->mNumVertices; j++) {
				vertex_data[j].position[0] = mesh->mVertices[j].x;
				vertex_data[j].position[1] = mesh->mVertices[j].y;
				vertex_data[j].position[2] = mesh->mVertices[j].z;
			}
		}

		if(mesh->HasNormals()) {
			for(GLuint j = 0; j < mesh->mNumVertices; j++) {
				vertex_data[j].normal[0] = mesh->mNormals[j].x;
				vertex_data[j].normal[1] = mesh->mNormals[j].y;
				vertex_data[j].normal[2] = mesh->mNormals[j].z;
			}
		}

		if(mesh->HasTextureCoords(0)) {
			for(GLuint j = 0; j < mesh->mNumVertices; j++) {
				vertex_data[j].textureCoord[0] = mesh->mTextureCoords[0][j].x;
				vertex_data[j].textureCoord[1] = mesh->mTextureCoords[0][j].y;
			}
		}

		if(mesh->HasTangentsAndBitangents()) {
			for(GLuint j = 0; j < mesh->mNumVertices; j++) {
				vertex_data[j].tangent[0] = mesh->mTangents[j][0];
				vertex_data[j].tangent[1] = mesh->mTangents[j][1];
				vertex_data[j].tangent[2] = mesh->mTangents[j][2];
			}
		}


		GLuint vertex_buffer;
		glGenBuffers(1, &vertex_buffer);
		vbo_buffers.push_back(vertex_buffer);
		//vertex_data_buffers.push_back(vertex_buffer);
		mesh_materials.push_back(mesh->mMaterialIndex);

		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, vertex_data.size()*sizeof(VertexData), &vertex_data[0], GL_STATIC_DRAW);


		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 0); // Set up our vertex attributes pointer
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_TRUE, sizeof(VertexData), (void*)(3 * sizeof(GLfloat))); // Normal
		glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(6 * sizeof(GLfloat))); // Texture
		glVertexAttribPointer((GLuint)3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(8 * sizeof(GLfloat))); // Tangent

		glBindVertexArray(0);

	}
	glEnableVertexAttribArray(0); // Disable our Vertex Array Object
	//glBindVertexArray(0);
	//aiReleaseImport(MeshScene);
}



void StaticMesh_120::draw(MaterialList m, Shader *s, uint flag)
{

	MaterialList::iterator mit = m.begin();

	for(glm::uint i = 0; i < Indices.size(); i++) {
		//glDebug();
		int error = glGetError();

		//if(error)Debug("StaticMesh::render pre opengl error:"+error);
		if(mit != m.end()) {
			(*mit)->bind();
			mit++;
		}

		if(listId[i]) {
			glCallList(listId[i]);

		} else {
			listId[i] = glGenLists(1);
			glNewList(listId[i], GL_COMPILE);
			glBegin(GL_TRIANGLES);
			for(int c = 0; c < Indices[i].size(); c++) {
				glColor3fv(&vertex_datas[i][Indices[i][c]].tangent[0]);
				glNormal3fv(&vertex_datas[i][Indices[i][c]].normal[0]);
				glTexCoord2fv(&vertex_datas[i][Indices[i][c]].textureCoord[0]);
				glVertex3fv(&vertex_datas[i][Indices[i][c]].position[0]);


			}
			glEnd();
			glEndList();
		}
	}

	//glBindVertexArray(0); // Unbind our Vertex Array Object
}

bool StaticMesh_120::Import(std::string filename)
{

	const aiScene* MeshScene = aiImportFile(filename.data(), aiProcessPreset_TargetRealtime_Quality | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);

	if(!MeshScene) {
		//std::cout << "cannot load the file at "<<filename<<"\n";//importer.GetErrorString() << std::endl;

		Engine::error(Engine::EString() + "cannot load the file at " + filename);
		Engine::consolePause();
		exit(1);
	} else {
		Engine::debug(Engine::EString() + MeshScene->mNumMeshes + " models loaded!");
	}

	//vao.resize(MeshScene->mNumMeshes);
	/*for(GLuint i = 0; i < MeshScene->mNumMeshes; i++) {

	        GLuint vaoId;

	        int error=glGetError();
			if(error)Debug("StaticMesh::import opengl error:"+error);

	        glGenVertexArrays(1,&vaoId);

	        glBindVertexArray(vaoId);
	        vao.push_back(vaoId);

	        std::vector<GLuint> indices;
	        const struct aiMesh* mesh;
	        mesh = MeshScene->mMeshes[i];
	        for(GLuint j = 0; j < mesh->mNumFaces; j++) {
	                const struct aiFace* face = &mesh->mFaces[j];
	                indices.push_back(face->mIndices[0]);
	                indices.push_back(face->mIndices[1]);
	                indices.push_back(face->mIndices[2]);

	            }
	        num_indices.push_back(indices.size());

	        GLuint vertex_indices;
	        glGenBuffers(1, &vertex_indices);
	        index_buffers.push_back(vertex_indices);
	        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_indices);
	        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

			error=glGetError();
			if(error)Debug("StaticMesh::import vao opengl error:"+error);

	        //std::vector<VertexData> vertex_data;
	        //vertex_data.resize(3 * mesh->mNumFaces);
	        //memset(&vertex_data[0], 0, vertex_data.size()*sizeof(VertexData));

	        if(mesh->HasPositions()) {
	        		vector<vec3> positions(mesh->mNumVertices);
	                for(GLuint j = 0; j < mesh->mNumVertices; j++) {
	                        positions[j][0] = mesh->mVertices[j].x;
	                        positions[j][1] = mesh->mVertices[j].y;
	                        positions[j][2] = mesh->mVertices[j].z;
	                    }
					GLuint buffer;
					glGenBuffers(1, &buffer);
			        std::cout <<"new position buffer " << buffer << " ("<<sizeof(GLfloat)*3*mesh->mNumVertices<<" bytes)\n";
			        glBindBuffer(GL_ARRAY_BUFFER, buffer);
			        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*mesh->mNumVertices, &positions[0][0], GL_STATIC_DRAW);
			        glEnableVertexAttribArray(0);
			        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			        int error=glGetError();
					if(error)Debug("StaticMesh::import  pos opengl error:"+error);
	            }
	        if(mesh->HasNormals()) {
	        		vector<vec3> normals(mesh->mNumVertices);
	                for(GLuint j = 0; j < mesh->mNumVertices; j++) {
	                        normals[j][0] = mesh->mNormals[j].x;
	                        normals[j][1] = mesh->mNormals[j].y;
	                        normals[j][2] = mesh->mNormals[j].z;
	                    }
					GLuint buffer;
					glGenBuffers(1, &buffer);
			        std::cout <<"new position buffer " << buffer << " ("<<sizeof(GLfloat)*3*mesh->mNumVertices<<" bytes)\n";
			        glBindBuffer(GL_ARRAY_BUFFER, buffer);
			        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*mesh->mNumVertices, &normals[0][0], GL_STATIC_DRAW);
			        glEnableVertexAttribArray(1);
			        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			        int error=glGetError();
					if(error)Debug("StaticMesh::import normal opengl error:"+error);
	            }

	        if(mesh->HasTextureCoords(0)) {
	        		vector<vec2> texCoord(mesh->mNumVertices);
	                for(GLuint j = 0; j < mesh->mNumVertices; j++) {
	                        texCoord[j][0] = mesh->mTextureCoords[0][j].x;
	                        texCoord[j][1] = mesh->mTextureCoords[0][j].y;

	                    }
					GLuint buffer;
					glGenBuffers(1, &buffer);
			        std::cout <<"new position buffer " << buffer << " ("<<sizeof(GLfloat)*2*mesh->mNumVertices<<" bytes)\n";
			        glBindBuffer(GL_ARRAY_BUFFER, buffer);
			        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*2*mesh->mNumVertices, &texCoord[0][0], GL_STATIC_DRAW);
			        glEnableVertexAttribArray(2);
			        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
			        int error=glGetError();
					if(error)Debug("StaticMesh::import texture opengl error:"+error);
	            }



	        glBindVertexArray(0);

	    }
	glEnableVertexAttribArray(0); // Disable our Vertex Array Object
	//glBindVertexArray(0);
	//aiReleaseImport(MeshScene);
	*/

	for(GLuint i = 0; i < MeshScene->mNumMeshes; i++) {

		GLuint vaoId;

		int error = glGetError();
		if(error)Debug("StaticMesh::import opengl error:" + error);


		std::vector<GLuint> indices;
		const struct aiMesh* mesh;
		mesh = MeshScene->mMeshes[i];
		for(GLuint j = 0; j < mesh->mNumFaces; j++) {
			const struct aiFace* face = &mesh->mFaces[j];
			indices.push_back(face->mIndices[0]);
			indices.push_back(face->mIndices[1]);
			indices.push_back(face->mIndices[2]);

		}
		num_indices.push_back(indices.size());
		Indices.push_back(indices);

		std::vector<VertexData> vertex_data;
		vertex_data.resize(3 * mesh->mNumFaces);
		memset(&vertex_data[0], 0, vertex_data.size()*sizeof(VertexData));

		if(mesh->HasPositions()) {
			for(GLuint j = 0; j < mesh->mNumVertices; j++) {
				vertex_data[j].position[0] = mesh->mVertices[j].x;
				vertex_data[j].position[1] = mesh->mVertices[j].y;
				vertex_data[j].position[2] = mesh->mVertices[j].z;
			}
		}

		if(mesh->HasNormals()) {
			for(GLuint j = 0; j < mesh->mNumVertices; j++) {
				vertex_data[j].normal[0] = mesh->mNormals[j].x;
				vertex_data[j].normal[1] = mesh->mNormals[j].y;
				vertex_data[j].normal[2] = mesh->mNormals[j].z;
			}
		}

		if(mesh->HasTextureCoords(0)) {
			for(GLuint j = 0; j < mesh->mNumVertices; j++) {
				vertex_data[j].textureCoord[0] = mesh->mTextureCoords[0][j].x;
				vertex_data[j].textureCoord[1] = mesh->mTextureCoords[0][j].y;
			}
		}

		if(mesh->HasTangentsAndBitangents()) {
			for(GLuint j = 0; j < mesh->mNumVertices; j++) {
				vertex_data[j].tangent[0] = mesh->mTangents[j][0];
				vertex_data[j].tangent[1] = mesh->mTangents[j][1];
				vertex_data[j].tangent[2] = mesh->mTangents[j][2];
			}
		}



		mesh_materials.push_back(mesh->mMaterialIndex);
		vertex_datas.push_back(vertex_data);


	}
	listId.resize(Indices.size());
	for(int i = 0; i < listId.size(); i++)listId[i] = 0;
	// glEnableVertexAttribArray(0); // Disable our Vertex Array Object

}

/*


void StaticMesh_120::draw(MaterialList m)
{

    MaterialList::iterator mit=m.begin();

    for(glm::uint i=0; i<vao.size(); i++) {
    		//glDebug();
			int error=glGetError();

			//if(error)Debug("StaticMesh::render pre opengl error:"+error);
            if(mit!=m.end()) {
                    (*mit)->bind();
                    mit++;
                }
			error=glGetError();
		//	if(error)Debug("StaticMesh::render pre2 opengl error:"+error);
            glBindVertexArray(vao[i]); // Bind our Vertex Array Object
			error=glGetError();
		//	if(error)Debug("StaticMesh::render vao opengl error:"+error);
            glDrawElements(GL_TRIANGLES, num_indices[i], GL_UNSIGNED_INT, 0);
			error=glGetError();
		//	if(error)Debug("StaticMesh::render draw opengl error:"+error);
        }

    glBindVertexArray(0); // Unbind our Vertex Array Object
}

bool StaticMesh_120::Import(std::string filename,Shader *s)
{

    const aiScene* MeshScene = aiImportFile(filename.data(), aiProcessPreset_TargetRealtime_Quality|aiProcess_CalcTangentSpace|aiProcess_GenSmoothNormals);

    if(!MeshScene) {
            //std::cout << "cannot load the file at "<<filename<<"\n";//importer.GetErrorString() << std::endl;

            Engine::error(Engine::EString()+"cannot load the file at "+filename);
            Engine::consolePause();
            exit(1);
        }
    else {
            Engine::debug(Engine::EString()+MeshScene->mNumMeshes + " models loaded!");
        }

    //vao.resize(MeshScene->mNumMeshes);

    	GLint posLoc=0,normLoc=1,texLoc=2,tanLoc=3;

    	if(s){
    		GLuint p=s->getProgramId();
			posLoc=glGetAttribLocation(p,"vertex_position_modelspace");
			normLoc=glGetAttribLocation(p,"normal_modelspace");
			texLoc=glGetAttribLocation(p,"tex_coords");
			tanLoc=glGetAttribLocation(p,"tangent");
		}

        for(GLuint i = 0; i < MeshScene->mNumMeshes; i++) {

            GLuint vaoId;

			int error=glGetError();
			if(error)Debug("StaticMesh::import opengl error:"+error);

            glGenVertexArrays(1,&vaoId);

            glBindVertexArray(vaoId);
            vao.push_back(vaoId);

            std::vector<GLuint> indices;
            const struct aiMesh* mesh;
            mesh = MeshScene->mMeshes[i];
            for(GLuint j = 0; j < mesh->mNumFaces; j++) {
                    const struct aiFace* face = &mesh->mFaces[j];
                    indices.push_back(face->mIndices[0]);
                    indices.push_back(face->mIndices[1]);
                    indices.push_back(face->mIndices[2]);

                }
            num_indices.push_back(indices.size());

            GLuint vertex_indices;
            glGenBuffers(1, &vertex_indices);
            index_buffers.push_back(vertex_indices);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_indices);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

            std::vector<VertexData> vertex_data;
            vertex_data.resize(3 * mesh->mNumFaces);
            memset(&vertex_data[0], 0, vertex_data.size()*sizeof(VertexData));

            if(mesh->HasPositions()) {
                    for(GLuint j = 0; j < mesh->mNumVertices; j++) {
                            vertex_data[j].position[0] = mesh->mVertices[j].x;
                            vertex_data[j].position[1] = mesh->mVertices[j].y;
                            vertex_data[j].position[2] = mesh->mVertices[j].z;
                        }
                }

            if(mesh->HasNormals()) {
                    for(GLuint j = 0; j < mesh->mNumVertices; j++) {
                            vertex_data[j].normal[0] = mesh->mNormals[j].x;
                            vertex_data[j].normal[1] = mesh->mNormals[j].y;
                            vertex_data[j].normal[2] = mesh->mNormals[j].z;
                        }
                }

            if(mesh->HasTextureCoords(0)) {
                    for(GLuint j = 0; j < mesh->mNumVertices; j++) {
                            vertex_data[j].textureCoord[0] = mesh->mTextureCoords[0][j].x;
                            vertex_data[j].textureCoord[1] = mesh->mTextureCoords[0][j].y;
                        }
                }

            if(mesh->HasTangentsAndBitangents()) {
                    for(GLuint j = 0; j < mesh->mNumVertices; j++) {
                            vertex_data[j].tangent[0] = mesh->mTangents[j][0];
                            vertex_data[j].tangent[1] = mesh->mTangents[j][1];
                            vertex_data[j].tangent[2] = mesh->mTangents[j][2];
                        }
                }


            GLuint vertex_buffer;
            glGenBuffers(1, &vertex_buffer);
            vbo_buffers.push_back(vertex_buffer);
            //vertex_data_buffers.push_back(vertex_buffer);
            mesh_materials.push_back(mesh->mMaterialIndex);

            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
            glBufferData(GL_ARRAY_BUFFER, vertex_data.size()*sizeof(VertexData), &vertex_data[0], GL_STATIC_DRAW);


            glEnableVertexAttribArray(posLoc);
            glEnableVertexAttribArray(normLoc);
            glEnableVertexAttribArray(texLoc);
            glEnableVertexAttribArray(tanLoc);

            glVertexAttribPointer((GLuint)posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 0); // Set up our vertex attributes pointer
            glVertexAttribPointer((GLuint)normLoc, 3, GL_FLOAT, GL_TRUE, sizeof(VertexData), (void*)(3*sizeof(GLfloat))); // Normal
            glVertexAttribPointer((GLuint)texLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(6*sizeof(GLfloat))); // Texture
            glVertexAttribPointer((GLuint)tanLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(8*sizeof(GLfloat))); // Tangent

            glBindVertexArray(0);

        }
    glEnableVertexAttribArray(0); // Disable our Vertex Array Object

}
*/
