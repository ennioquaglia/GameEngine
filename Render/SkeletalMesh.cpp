#include "../common/AssetManager.h"
#include "SkeletalMesh.h"
#include "../common/utility.h"
#include "Shader.h"
using namespace Engine;



aiQuaternion lerp(aiQuaternion a, aiQuaternion b, float t)
{
	aiQuaternion c;
	c.w = a.w + (b.w - a.w) * t;
	c.x = a.x + (b.x - a.x) * t;
	c.y = a.y + (b.y - a.y) * t;
	c.z = a.z + (b.z - a.z) * t;
	return c;
}
aiVector3D lerp(aiVector3D a, aiVector3D b, float t)
{
	aiVector3D c;
	c.x = a.x + (b.x - a.x) * t;
	c.y = a.y + (b.y - a.y) * t;
	c.z = a.z + (b.z - a.z) * t;
	return c;
}
inline void copyAiMatrixToGLM(const aiMatrix4x4 *from, glm::mat4 &to)
{
	to[0][0] = (GLfloat)from->a1;
	to[1][0] = (GLfloat)from->a2;
	to[2][0] = (GLfloat)from->a3;
	to[3][0] = (GLfloat)from->a4;
	to[0][1] = (GLfloat)from->b1;
	to[1][1] = (GLfloat)from->b2;
	to[2][1] = (GLfloat)from->b3;
	to[3][1] = (GLfloat)from->b4;
	to[0][2] = (GLfloat)from->c1;
	to[1][2] = (GLfloat)from->c2;
	to[2][2] = (GLfloat)from->c3;
	to[3][2] = (GLfloat)from->c4;
	to[0][3] = (GLfloat)from->d1;
	to[1][3] = (GLfloat)from->d2;
	to[2][3] = (GLfloat)from->d3;
	to[3][3] = (GLfloat)from->d4;
}

void buildNodeHierarchy(const aiNode *node, std::map<std::string, int>& bone_name_hash, const glm::mat4 &T, std::vector<glm::mat4> &bone_transforms)
{
	glm::mat4 current_node_transform = T;
	if(&(node->mName) != NULL) {
		std::map<std::string, int>::iterator it = bone_name_hash.find(node->mName.data);
		if(it != bone_name_hash.end() && it->second > 0) {
			current_node_transform = current_node_transform * bone_transforms[it->second];
			bone_transforms[it->second] = current_node_transform;
		}
	}
	for(unsigned int i = 0; i < node->mNumChildren; ++i)
		buildNodeHierarchy(node->mChildren[i], bone_name_hash, current_node_transform, bone_transforms);
}




void SkeletalMesh::draw(MaterialList m, Shader *s, unsigned int flag)
{
	s->Use();
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
		for(glm::uint i = 0; i < VAOs.size(); i++) {
			if(mit != m.end()) {
				if(s) {
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

bool SkeletalMesh::Import(std::string filename)
{

	MeshScene = AssetManager::addAssimpScene//) aiImportFile
				( filename.data(), aiProcess_CalcTangentSpace                              |
				  aiProcess_GenSmoothNormals                              |
				  aiProcess_JoinIdenticalVertices                 |
				  aiProcess_ImproveCacheLocality                  |
				  aiProcess_LimitBoneWeights                              |
				  aiProcess_RemoveRedundantMaterials      |
				  aiProcess_SplitLargeMeshes                              |
				  aiProcess_Triangulate                                   |
				  aiProcess_GenUVCoords                   |
				  aiProcess_SortByPType                    |
				  aiProcess_FindDegenerates
				  //aiProcess_FindInvalidData
				);

	//aiImportFile(filename.data(), aiProcessPreset_TargetRealtime_Quality|aiProcess_CalcTangentSpace|aiProcess_GenSmoothNormals);

	if(!MeshScene) {
		//std::cout << "cannot load the file at "<<filename<<"\n";//importer.GetErrorString() << std::endl;

		Engine::error(Engine::EString() + "cannot load the file at " + filename);
		Engine::consolePause();
		exit(1);
	} else {
		Engine::debug(Engine::EString() + MeshScene->mNumMeshes + " models loaded!");
	}


	int bonesNumber = 0;
	for(GLuint i = 0; i < MeshScene->mNumMeshes; i++) {
		const struct aiMesh* mesh;
		mesh = MeshScene->mMeshes[i];
		if(mesh)bonesNumber += mesh->mNumBones;
	}
	bonesNumber = 60;
	bone_transforms.resize(bonesNumber);
	offset_matrix_array.resize(bonesNumber);

	//memset(&offset_matrix_array[0], 0, offset_matrix_array.size()*sizeof(glm::mat4));
	for(GLuint i = 0; i < offset_matrix_array.size(); i++)offset_matrix_array[i] = mat4(1);



	//vao.resize(MeshScene->mNumMeshes);
	for(GLuint i = 0; i < MeshScene->mNumMeshes; i++) {



		std::vector<GLuint> indices;
		const struct aiMesh* mesh;
		mesh = MeshScene->mMeshes[i];
		for(GLuint j = 0; j < mesh->mNumFaces; j++) {
			const struct aiFace* face = &mesh->mFaces[j];
			indices.push_back(face->mIndices[0]);
			indices.push_back(face->mIndices[1]);
			indices.push_back(face->mIndices[2]);

		}


		std::vector<SkeletalMeshVertexData> vertex_data;
		vertex_data.resize(3 * mesh->mNumFaces);
		memset(&vertex_data[0], 0, vertex_data.size()*sizeof(SkeletalMeshVertexData));

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

		if(mesh->HasBones()) {
			//std::cout << "Mesh [" << i << "]" << " have " << mesh->mNumBones << " bones" << std::endl;

			std::map<int, int> vertex_bone_use_counter;
			for(uint j = 0; j < mesh->mNumBones; j++) {
				aiBone* bone = mesh->mBones[j];
				std::string bone_name(bone->mName.data);
				//std::cout << "Bone name: " << bone_name << std::endl;
				if(bone_name_hash.find(bone_name) == bone_name_hash.end()) {
					bone_name_hash[bone_name] = bone_name_hash.size() + 1;
					//std::cout << "ID = " << bone_name_hash[bone_name] << std::endl;
				} else {
					//std::cout << "Already hashed!" << std::endl;
				}

				int current_bone_id = bone_name_hash[bone_name];
				if(current_bone_id < 0) {
					//Debug(bone_name.data());
				} else {
					//std::cout << "mNumWeights: " << bone->mNumWeights << std::endl;

					for(uint k = 0; k < bone->mNumWeights; k++) {
						aiVertexWeight *weight = &(bone->mWeights[k]);
						int vertex_index = 0;
						if(vertex_bone_use_counter.find(weight->mVertexId) == vertex_bone_use_counter.end()) {
							vertex_bone_use_counter[weight->mVertexId] = 1;
						} else {
							vertex_index = vertex_bone_use_counter[weight->mVertexId];
							vertex_bone_use_counter[weight->mVertexId]++;
						}
						//if (vertex_index > 4) std::cout << "DEU PAU!" << std::endl;
						vertex_data[weight->mVertexId].boneIDs[vertex_index] = current_bone_id;

						vertex_data[weight->mVertexId].boneWeights[vertex_index] = weight->mWeight;
					}
					copyAiMatrixToGLM(&bone->mOffsetMatrix, offset_matrix_array[current_bone_id]);
				}
			}
		}
		VAOs.push_back(VAO(vertex_data, indices));
	}

	//glBindVertexArray(0);
	//aiReleaseImport(MeshScene);
	Debug("import ok! (bone=" + bone_transforms.size() + ")");
	return 1;
}

void SkeletalMesh::setAnimTime(float time)
{

	if(!MeshScene)return;

	for(uint i = 0; i < bone_transforms.size(); i++) {
		bone_transforms[i] = glm::mat4(1.0f);
	}

	if(MeshScene->HasAnimations()) {
		//static float preTime;
		/*float time=glfwGetTime();
		double ticks_per_sec  = scene->mAnimations[0]->mTicksPerSecond;
		double time_in_tics   = time * ticks_per_sec;*/
		float animation_time = fmod(time, (float) MeshScene->mAnimations[0]->mDuration); //time; ;//= fmod(time_in_tics, scene->mAnimations[0]->mDuration);

		// animation_time=(1/(float)fps)*30;
		//animation_time=preTime-time;
		//preTime=time;
		//animation_time = CurrentAnim->update(0.01);
		//animation_time = currentAnimation->firstFrame + fmod(animation_time, currentAnimation->lastFrame - currentAnimation->firstFrame);
		//
		// updateAnim(animation_time);
		//

		for (uint i = 0; i < MeshScene->mAnimations[0]->mNumChannels; ++i) {
			const aiNodeAnim *node = MeshScene->mAnimations[0]->mChannels[i];
			std::map<std::string, int>::iterator it = bone_name_hash.find(node->mNodeName.data);
			if (it == bone_name_hash.end()) {
				continue;
			}
			if(it->second < 0)continue;



			int rotationKey = 0;
			while (animation_time >= node->mRotationKeys[rotationKey + 1].mTime) ++rotationKey;
			rotationKey = rotationKey % node->mNumRotationKeys;
			aiQuaternion ai_rotation = node->mRotationKeys[rotationKey].mValue;
			{
				int nextRotationKey = rotationKey + 1;
				aiQuaternion ai_rotation_next = node->mRotationKeys[nextRotationKey].mValue;
				float delta, alpha;
				delta = node->mRotationKeys[nextRotationKey].mTime - node->mRotationKeys[rotationKey].mTime;
				alpha = fabs((node->mRotationKeys[nextRotationKey].mTime - animation_time) / delta);
				ai_rotation = lerp(ai_rotation_next, ai_rotation, alpha);

			}

			int positionKey = 0;
			while (animation_time >= node->mPositionKeys[positionKey + 1].mTime) ++positionKey;
			positionKey = positionKey % node->mNumPositionKeys;
			aiVector3D ai_translation = node->mPositionKeys[positionKey].mValue;

			{

				int nextPositionKey = positionKey + 1;
				aiVector3D ai_translation_next = node->mPositionKeys[nextPositionKey].mValue;

				glm::vec3 a1(ai_translation.x, ai_translation.y, ai_translation.z);
				glm::vec3 b1(ai_translation_next.x, ai_translation_next.y, ai_translation_next.z);

				float delta = node->mPositionKeys[nextPositionKey].mTime - node->mPositionKeys[positionKey].mTime;
				float alpha = fabs((animation_time - node->mRotationKeys[rotationKey].mTime) / delta);
				for(unsigned int d = 0; d < 3; d++)ai_translation[d] = ai_translation[d] + (ai_translation_next[d] - ai_translation[d]) * alpha;
			}

			uint scaleIndex = 0;
			while( 1 ) {
				if( scaleIndex + 1 >= node->mNumScalingKeys )
					break;
				if( node->mScalingKeys[scaleIndex++].mTime > animation_time )
					break;
			}
			aiVector3D curScaling = node->mScalingKeys[scaleIndex].mValue;


			glm::quat rotation((GLfloat)ai_rotation.w, (GLfloat)ai_rotation.x, (GLfloat)ai_rotation.y, (GLfloat)ai_rotation.z);
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3((GLfloat)ai_translation.x, (GLfloat)ai_translation.y, (GLfloat)ai_translation.z));
			glm::mat4 M = translation * glm::mat4_cast(glm::normalize(rotation));
			//M[0][0] *= curScaling.x;
			//M[1][1] *= curScaling.y;
			//M[2][2] *= curScaling.z;

			bone_transforms[it->second] = M;
		}
	}
	buildNodeHierarchy(MeshScene->mRootNode, bone_name_hash, glm::mat4(1.0f), bone_transforms);
	for(uint i = 0; i < bone_transforms.size(); i++) {
		bone_transforms[i] = bone_transforms[i] * offset_matrix_array[i];
	}

	//glUseProgram(programId);
	//int loc=glGetUniformLocation(programId, "bone_transforms");
	//if(loc>-1)
	//glUniformMatrix4fv(loc, bone_transforms.size(), GL_FALSE, glm::value_ptr(bone_transforms[0]));
	//else printf("Skeletal mesh MatrixUniform error ");
	//
	//if(programIdShadow){
	//	glUseProgram(programIdShadow);
	//	int loc=glGetUniformLocation(programIdShadow, "bone_transforms");
	//	if(loc>-1)
	//	glUniformMatrix4fv(loc, bone_transforms.size(), GL_FALSE, glm::value_ptr(bone_transforms[0]));
	//	glUseProgram(programId);
	//}

}












void SkeletalMesh_old::draw(MaterialList m, Shader *s, unsigned int flag)
{

	MaterialList::iterator mit = m.begin();

	if(m.empty()) {
		Material *mat = Material::getDefaultMaterial();
		if(s) {
			//mat->updateShader(s);
			mat->setShader(s, 1);
		}
		mat->bind();
	}

	for(glm::uint i = 0; i < vao.size(); i++) {
		if(mit != m.end()) {
			if(s)
				(*mit)->updateShader(s);
			(*mit)->bind();
			mit++;
		}

		glBindVertexArray(vao[i]); // Bind our Vertex Array Object

		glDrawElements(GL_TRIANGLES, num_indices[i], GL_UNSIGNED_INT, 0);

	}

	glBindVertexArray(0); // Unbind our Vertex Array Object
}

mat4 matMix(mat4 a,mat4 b,float alpha){
	mat4 out;
	for(int x=0;x<4;x++){
		for(int y=0;y<4;y++){
			out[x][y]=glm::mix(a[x][y],b[x][y],alpha);
		}
	}
	return out;
}


bool SkeletalMesh_old::Import(std::string filename)
{

	MeshScene = AssetManager::addAssimpScene//) aiImportFile
				( filename.data(), aiProcess_CalcTangentSpace                              |
				  aiProcess_GenSmoothNormals                              |
				  aiProcess_JoinIdenticalVertices                 |
				  aiProcess_ImproveCacheLocality                  |
				  aiProcess_LimitBoneWeights                              |
				  aiProcess_RemoveRedundantMaterials      |
				  aiProcess_SplitLargeMeshes                              |
				  aiProcess_Triangulate                                   |
				  aiProcess_GenUVCoords                   |
				  aiProcess_SortByPType                    |
				  aiProcess_FindDegenerates
				  //aiProcess_FindInvalidData
				);

	//aiImportFile(filename.data(), aiProcessPreset_TargetRealtime_Quality|aiProcess_CalcTangentSpace|aiProcess_GenSmoothNormals);

	if(!MeshScene) {
		//std::cout << "cannot load the file at "<<filename<<"\n";//importer.GetErrorString() << std::endl;

		Engine::error(Engine::EString() + "cannot load the file at " + filename);
		Engine::consolePause();
		exit(1);
	} else {
		Engine::debug(Engine::EString() + MeshScene->mNumMeshes + " models loaded!");
	}


	int bonesNumber = 0;
	for(GLuint i = 0; i < MeshScene->mNumMeshes; i++) {
		const struct aiMesh* mesh;
		mesh = MeshScene->mMeshes[i];
		if(mesh)bonesNumber += mesh->mNumBones;
	}
	bonesNumber = 60;
	bone_transforms.resize(bonesNumber);
	offset_matrix_array.resize(bonesNumber);

	//memset(&offset_matrix_array[0], 0, offset_matrix_array.size()*sizeof(glm::mat4));
	for(GLuint i = 0; i < offset_matrix_array.size(); i++)offset_matrix_array[i] = mat4(1);



	//vao.resize(MeshScene->mNumMeshes);
	for(GLuint i = 0; i < MeshScene->mNumMeshes; i++) {

		GLuint vaoId;
		glDebug();
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

		if(mesh->HasBones()) {
			//std::cout << "Mesh [" << i << "]" << " have " << mesh->mNumBones << " bones" << std::endl;

			std::map<int, int> vertex_bone_use_counter;
			for(uint j = 0; j < mesh->mNumBones; j++) {
				aiBone* bone = mesh->mBones[j];
				std::string bone_name(bone->mName.data);
				//std::cout << "Bone name: " << bone_name << std::endl;
				if(bone_name_hash.find(bone_name) == bone_name_hash.end()) {
					bone_name_hash[bone_name] = bone_name_hash.size() + 1;
					//std::cout << "ID = " << bone_name_hash[bone_name] << std::endl;
				} else {
					//std::cout << "Already hashed!" << std::endl;
				}

				int current_bone_id = bone_name_hash[bone_name];
				if(current_bone_id < 0) {
					//Debug(bone_name.data());
				} else {
					//std::cout << "mNumWeights: " << bone->mNumWeights << std::endl;

					for(uint k = 0; k < bone->mNumWeights; k++) {
						aiVertexWeight *weight = &(bone->mWeights[k]);
						int vertex_index = 0;
						if(vertex_bone_use_counter.find(weight->mVertexId) == vertex_bone_use_counter.end()) {
							vertex_bone_use_counter[weight->mVertexId] = 1;
						} else {
							vertex_index = vertex_bone_use_counter[weight->mVertexId];
							vertex_bone_use_counter[weight->mVertexId]++;
						}
						//if (vertex_index > 4) std::cout << "DEU PAU!" << std::endl;
						vertex_data[weight->mVertexId].boneIDs[vertex_index] = current_bone_id;

						vertex_data[weight->mVertexId].boneWeights[vertex_index] = weight->mWeight;
					}
					copyAiMatrixToGLM(&bone->mOffsetMatrix, offset_matrix_array[current_bone_id]);
				}
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
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);

		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), 0); // Set up our vertex attributes pointer
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_TRUE, sizeof(VertexData), (void*)(3 * sizeof(GLfloat))); // Normal
		glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(6 * sizeof(GLfloat))); // Texture
		glVertexAttribPointer((GLuint)3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(8 * sizeof(GLfloat))); // Tangent
		glVertexAttribPointer((GLuint)4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(11 * sizeof(GLfloat))); // boneWeights
		glVertexAttribPointer((GLuint)5, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(15 * sizeof(GLfloat))); // boneIDs

		glBindVertexArray(0);
		glDebug();
	}
	glEnableVertexAttribArray(0); // Disable our Vertex Array Object
	//glBindVertexArray(0);
	//aiReleaseImport(MeshScene);
	Debug("import ok! (bone=" + bone_transforms.size() + ")");
	return 1;
}

void SkeletalMesh_old::setAnimTime(float time)
{

	if(!MeshScene)return;

	for(uint i = 0; i < bone_transforms.size(); i++) {
		bone_transforms[i] = glm::mat4(1.0f);
	}

	if(MeshScene->HasAnimations()) {
		//static float preTime;
		/*float time=glfwGetTime();
		double ticks_per_sec  = scene->mAnimations[0]->mTicksPerSecond;
		double time_in_tics   = time * ticks_per_sec;*/
		float animation_time = fmod(time, (float) MeshScene->mAnimations[0]->mDuration); //time; ;//= fmod(time_in_tics, scene->mAnimations[0]->mDuration);

		// animation_time=(1/(float)fps)*30;
		//animation_time=preTime-time;
		//preTime=time;
		//animation_time = CurrentAnim->update(0.01);
		//animation_time = currentAnimation->firstFrame + fmod(animation_time, currentAnimation->lastFrame - currentAnimation->firstFrame);
		//
		// updateAnim(animation_time);
		//

		for (uint i = 0; i < MeshScene->mAnimations[0]->mNumChannels; ++i) {
			const aiNodeAnim *node = MeshScene->mAnimations[0]->mChannels[i];
			std::map<std::string, int>::iterator it = bone_name_hash.find(node->mNodeName.data);
			if (it == bone_name_hash.end()) {
				continue;
			}
			if(it->second < 0)continue;



			int rotationKey = 0;
			while (animation_time >= node->mRotationKeys[rotationKey + 1].mTime) ++rotationKey;
			rotationKey = rotationKey % node->mNumRotationKeys;
			aiQuaternion ai_rotation = node->mRotationKeys[rotationKey].mValue;
			{
				int nextRotationKey = rotationKey + 1;
				aiQuaternion ai_rotation_next = node->mRotationKeys[nextRotationKey].mValue;
				float delta, alpha;
				delta = node->mRotationKeys[nextRotationKey].mTime - node->mRotationKeys[rotationKey].mTime;
				alpha = fabs((node->mRotationKeys[nextRotationKey].mTime - animation_time) / delta);
				ai_rotation = lerp(ai_rotation_next, ai_rotation, alpha);

			}

			int positionKey = 0;
			while (animation_time >= node->mPositionKeys[positionKey + 1].mTime) ++positionKey;
			positionKey = positionKey % node->mNumPositionKeys;
			aiVector3D ai_translation = node->mPositionKeys[positionKey].mValue;

			{

				int nextPositionKey = positionKey + 1;
				aiVector3D ai_translation_next = node->mPositionKeys[nextPositionKey].mValue;

				glm::vec3 a1(ai_translation.x, ai_translation.y, ai_translation.z);
				glm::vec3 b1(ai_translation_next.x, ai_translation_next.y, ai_translation_next.z);

				float delta = node->mPositionKeys[nextPositionKey].mTime - node->mPositionKeys[positionKey].mTime;
				float alpha = fabs((animation_time - node->mRotationKeys[rotationKey].mTime) / delta);
				for(unsigned int d = 0; d < 3; d++)ai_translation[d] = ai_translation[d] + (ai_translation_next[d] - ai_translation[d]) * alpha;
			}

			uint scaleIndex = 0;
			while( 1 ) {
				if( scaleIndex + 1 >= node->mNumScalingKeys )
					break;
				if( node->mScalingKeys[scaleIndex++].mTime > animation_time )
					break;
			}
			aiVector3D curScaling = node->mScalingKeys[scaleIndex].mValue;


			glm::quat rotation((GLfloat)ai_rotation.w, (GLfloat)ai_rotation.x, (GLfloat)ai_rotation.y, (GLfloat)ai_rotation.z);
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3((GLfloat)ai_translation.x, (GLfloat)ai_translation.y, (GLfloat)ai_translation.z));
			glm::mat4 M = translation * glm::mat4_cast(glm::normalize(rotation));
			//M[0][0] *= curScaling.x;
			//M[1][1] *= curScaling.y;
			//M[2][2] *= curScaling.z;

			bone_transforms[it->second] = M;
		}
	}
	buildNodeHierarchy(MeshScene->mRootNode, bone_name_hash, glm::mat4(1.0f), bone_transforms);
	for(uint i = 0; i < bone_transforms.size(); i++) {
		bone_transforms[i] = bone_transforms[i] * offset_matrix_array[i];
	}

	//glUseProgram(programId);
	//int loc=glGetUniformLocation(programId, "bone_transforms");
	//if(loc>-1)
	//glUniformMatrix4fv(loc, bone_transforms.size(), GL_FALSE, glm::value_ptr(bone_transforms[0]));
	//else printf("Skeletal mesh MatrixUniform error ");
	//
	//if(programIdShadow){
	//	glUseProgram(programIdShadow);
	//	int loc=glGetUniformLocation(programIdShadow, "bone_transforms");
	//	if(loc>-1)
	//	glUniformMatrix4fv(loc, bone_transforms.size(), GL_FALSE, glm::value_ptr(bone_transforms[0]));
	//	glUseProgram(programId);
	//}

}


//###// SkeletalMeshManager //###//

void SkeletalMeshManager::init()
{
	mesh =AssetManager::addSkeletalMesh(mFilename);// new SkeletalMesh_old(mFilename);//
	//mesh->setAnimTime(0.01);
	int bonesNumber = 60;
	bone_transforms.resize(bonesNumber);
	bone_transforms_pre.resize(bonesNumber);
	//setAnimTime(getAnimation(currentAnim),0.0);
}


void SkeletalMeshManager::setAnimTime(AnimType* CurrentAnim, float time)
{
	if(!CurrentAnim)return;
	const aiScene* MeshScene = CurrentAnim->scene;
	if(!MeshScene)return;

	for(uint i = 0; i < bone_transforms.size(); i++) {
		bone_transforms[i] = glm::mat4(1.0f);
	}

	float animation_time = 0;
	if(MeshScene->HasAnimations()) {

		animation_time = fmod(time, (float) MeshScene->mAnimations[0]->mDuration); //time; ;//= fmod(time_in_tics, scene->mAnimations[0]->mDuration);


		for (uint i = 0; i < MeshScene->mAnimations[0]->mNumChannels; ++i) {
			const aiNodeAnim *node = MeshScene->mAnimations[0]->mChannels[i];
			std::map<std::string, int>::iterator it = mesh->bone_name_hash.find(node->mNodeName.data);
			if (it == mesh->bone_name_hash.end()) {
				continue;
			}
			if(it->second < 0)continue;


			int rotationKey = 0;
			while (animation_time >= node->mRotationKeys[rotationKey + 1].mTime) ++rotationKey;
			rotationKey = rotationKey % node->mNumRotationKeys;
			aiQuaternion ai_rotation = node->mRotationKeys[rotationKey].mValue;
			{
				int nextRotationKey = rotationKey + 1;
				aiQuaternion ai_rotation_next = node->mRotationKeys[nextRotationKey].mValue;
				float delta, alpha;
				delta = node->mRotationKeys[nextRotationKey].mTime - node->mRotationKeys[rotationKey].mTime;
				alpha = fabs((node->mRotationKeys[nextRotationKey].mTime - animation_time) / delta);
				ai_rotation = lerp(ai_rotation_next, ai_rotation, alpha);

			}

			int positionKey = 0;
			while (animation_time >= node->mPositionKeys[positionKey + 1].mTime) ++positionKey;
			positionKey = positionKey % node->mNumPositionKeys;
			aiVector3D ai_translation = node->mPositionKeys[positionKey].mValue;

			{

				int nextPositionKey = positionKey + 1;
				aiVector3D ai_translation_next = node->mPositionKeys[nextPositionKey].mValue;

				glm::vec3 a1(ai_translation.x, ai_translation.y, ai_translation.z);
				glm::vec3 b1(ai_translation_next.x, ai_translation_next.y, ai_translation_next.z);

				float delta = node->mPositionKeys[nextPositionKey].mTime - node->mPositionKeys[positionKey].mTime;
				float alpha = fabs((animation_time - node->mRotationKeys[rotationKey].mTime) / delta);
				for(unsigned int d = 0; d < 3; d++)ai_translation[d] = ai_translation[d] + (ai_translation_next[d] - ai_translation[d]) * alpha;
			}

			uint scaleIndex = 0;
			while( 1 ) {
				if( scaleIndex + 1 >= node->mNumScalingKeys )
					break;
				if( node->mScalingKeys[scaleIndex++].mTime > animation_time )
					break;
			}
			aiVector3D curScaling = node->mScalingKeys[scaleIndex].mValue;


			glm::quat rotation((GLfloat)ai_rotation.w, (GLfloat)ai_rotation.x, (GLfloat)ai_rotation.y, (GLfloat)ai_rotation.z);
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3((GLfloat)ai_translation.x, (GLfloat)ai_translation.y, (GLfloat)ai_translation.z));
			glm::mat4 M = translation * glm::mat4_cast(glm::normalize(rotation));

			bone_transforms[it->second] = M;
		}
	}
	bone_transforms[mesh->bone_name_hash["Alpha_Hips"]][3][1] = 0;

	buildNodeHierarchy(MeshScene->mRootNode, mesh->bone_name_hash, glm::mat4(1.0f), bone_transforms);


	float v = animation_time / (float)CurrentAnim->duration;
	//if(nextAnim!=currentAnim)//&&(v>0.7||v<0.3))
	for(uint i = 0; i < bone_transforms.size(); i++) {
		bone_transforms[i] = bone_transforms[i] * mesh->offset_matrix_array[i];
		//bone_transforms[i] = matMix(bone_transforms[i] * mesh->offset_matrix_array[i], bone_transforms_pre[i], 0.1f);
		//bone_transforms_pre[i] = bone_transforms[i];
	}


	/*
	else
	for(uint i = 0; i < bone_transforms.size(); i++)
	{
		bone_transforms[i] = bone_transforms[i] * mesh->offset_matrix_array[i];
		bone_transforms_pre[i] = bone_transforms[i];
	}	*/

}



void SkeletalMeshManager::update(float deltaTimeMs)
{
	time += deltaTimeMs * 0.001;

	AnimType* CurrentAnim = getAnimation(currentAnim);

	float animation_time = 0;

	if(!CurrentAnim)return;

	animation_time = CurrentAnim->firstFrame + fmod(time, CurrentAnim->lastFrame - CurrentAnim->firstFrame);


	// if(preTime<time){finished=true;isPlaying=0; preTime=time;}
	// else {finished=false;isPlaying=1;AnimationTime=0; preTime=time-1;
	//    if(NextAnim){CurrentAnim=NextAnim;NextAnim=IdleAnim;}}
	//
	// if(CurrentAnim!=IdleAnim)NextAnim=IdleAnim;
	// else if(NextAnim)if(NextAnim!=IdleAnim){AnimationTime=0;preTime=time-1;CurrentAnim=NextAnim;	}
	//

	setAnimTime(CurrentAnim, animation_time);
}
void SkeletalMeshManager::draw(MaterialList m, Shader *s, unsigned int flag)
{
	if(!mesh)return;
	mesh->draw(m, s ,flag);
}

void SkeletalMeshManager::updateUniform(GLuint programID)
{
	int loc = glGetUniformLocation(programID, "bone_transforms");
	if(loc > -1)
		//glUniformMatrix4fv(loc, mesh->bone_transforms.size(), GL_FALSE, glm::value_ptr(mesh->bone_transforms[0]));
		glUniformMatrix4fv(loc, bone_transforms.size(), GL_FALSE, glm::value_ptr(bone_transforms[0]));
}


void SkeletalMeshManager::addAnimation(AnimID anim, string filename, int firstFrame, int lastFrame)
{

	const aiScene* mesh = AssetManager::addAssimpScene
						  ( filename.data(), aiProcess_CalcTangentSpace                              |
							aiProcess_GenSmoothNormals                              |
							aiProcess_JoinIdenticalVertices                 |
							aiProcess_ImproveCacheLocality                  |
							aiProcess_LimitBoneWeights                              |
							aiProcess_RemoveRedundantMaterials      |
							aiProcess_SplitLargeMeshes                              |
							aiProcess_Triangulate                                   |

							aiProcess_SortByPType                    |
							aiProcess_FindDegenerates
						  );
	AnimType a = {anim, firstFrame, lastFrame, lastFrame - firstFrame, mesh};
	animations[anim] = a;

}
void SkeletalMeshManager::addAnimation(AnimID anim, int firstFrame, int lastFrame)
{
	AnimType a = {anim, firstFrame, lastFrame, lastFrame - firstFrame, mesh->MeshScene};
	animations[anim] = a;
}
/*
void SkeletalMeshManager::addAnimation(AnimID anim,string filename,int firstFrame,int lastFrame){



}
*/

void SkeletalMeshManager::removeAnimation(AnimID anim)
{
	animations.erase(anim);
}
void SkeletalMeshManager::playAnimation(AnimID anim)
{
	currentAnim = anim;
	time = 0;
}
void SkeletalMeshManager::setAnimation(AnimID anim)
{
	currentAnim = anim;
}
SkeletalMeshManager::AnimType* SkeletalMeshManager::getAnimation(AnimID anim)
{
	Animations::iterator it = animations.find(anim);
	if (it == animations.end()){
		return 0;
	}
	else return &(animations[anim]);
}
