#include "MeshData.h"
#include "AssimpScene.h"
#include "Debug.h"
#include "Hash.h"

namespace Engine{


StaticMeshData::StaticMeshData(){}
StaticMeshData::StaticMeshData(PathName filename){

    if (filename.toStd().empty())return;
    const aiScene* MeshScene = AssimpScene::load(filename, aiProcessPreset_TargetRealtime_Quality | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals)->scene;


    if (!MeshScene){
        //#if ENGINE_USE_QT_PROPERTY_BROWSER==0
            QENGINE_ERROR("cannot load the file at " + filename.data);
            QENGINE_PAUSE();
        //#endif
        return;
    }
    else QENGINE_INFO(MeshScene->mNumMeshes + " models loaded! ("+filename.toStd()+")");

    int indices_size=0;
    int vertex_size=0;
    for (GLuint i = 0; i < MeshScene->mNumMeshes; i++) {
           indices_size+=MeshScene->mMeshes[i]->mNumFaces*3;
           vertex_size+=MeshScene->mMeshes[i]->mNumVertices;
    }


//#####// Only one VAO //#####//
//    std::vector<GLuint> indices(indices_size);
//    std::vector<StaticMeshVertexData> vertex_data(vertex_size);
//    indices_size=0;
//    vertex_size=0;

//    for (GLuint i = 0; i < MeshScene->mNumMeshes; i++) {
//        //GLuint vaoId;

//        glDebug("StaticMeshData::import opengl error");


//        const struct aiMesh* mesh;
//        mesh = MeshScene->mMeshes[i];
//        for (GLuint j = 0; j < mesh->mNumFaces; j++) {
//            const struct aiFace* face = &mesh->mFaces[j];
//            indices[indices_size+j*3]=face->mIndices[0]+vertex_size;
//            indices[indices_size+j*3+1]=face->mIndices[1]+vertex_size;
//            indices[indices_size+j*3+2]=face->mIndices[2]+vertex_size;
//        }
//        indices_size+=mesh->mNumFaces*3;

//        //vertex_data.resize(3 * mesh->mNumFaces);
//        //memset(&vertex_data[vertex_size], 0, vertex_data.size()*sizeof(StaticMeshVertexData));

//        if (mesh->HasPositions()) {
//            for (GLuint j = 0; j < mesh->mNumVertices; j++) {
//                vertex_data[vertex_size+j].position[0] = mesh->mVertices[j].x;
//                vertex_data[vertex_size+j].position[1] = mesh->mVertices[j].y;
//                vertex_data[vertex_size+j].position[2] = mesh->mVertices[j].z;
//            }
//        }

//        if (mesh->HasNormals()) {
//            for (GLuint j = 0; j < mesh->mNumVertices; j++) {
//                vertex_data[vertex_size+j].normal[0] = mesh->mNormals[j].x;
//                vertex_data[vertex_size+j].normal[1] = mesh->mNormals[j].y;
//                vertex_data[vertex_size+j].normal[2] = mesh->mNormals[j].z;
//            }
//        }

//        if (mesh->HasTextureCoords(0)) {
//            for (GLuint j = 0; j < mesh->mNumVertices; j++) {
//                vertex_data[vertex_size+j].textureCoord[0] = mesh->mTextureCoords[0][j].x;
//                vertex_data[vertex_size+j].textureCoord[1] = mesh->mTextureCoords[0][j].y;
//            }
//        }

//        if (mesh->HasTangentsAndBitangents()) {
//            for (GLuint j = 0; j < mesh->mNumVertices; j++) {
//                vertex_data[vertex_size+j].tangent[0] = mesh->mTangents[j][0];
//                vertex_data[vertex_size+j].tangent[1] = mesh->mTangents[j][1];
//                vertex_data[vertex_size+j].tangent[2] = mesh->mTangents[j][2];
//            }
//        }

//        vertex_size+= mesh->mNumVertices;

//    }
//    VAOs.push_back(VAO(vertex_data, indices));


//#####// More VAOs //#####//
    for (GLuint i = 0; i < MeshScene->mNumMeshes; i++) {

        //GLuint vaoId;

        glDebug("StaticMeshData::import opengl error");

        std::vector<GLuint> indices;
        const struct aiMesh* mesh;
        mesh = MeshScene->mMeshes[i];
        for (GLuint j = 0; j < mesh->mNumFaces; j++) {
            const struct aiFace* face = &mesh->mFaces[j];
            indices.push_back(face->mIndices[0]);
            indices.push_back(face->mIndices[1]);
            indices.push_back(face->mIndices[2]);
        }

        std::vector<StaticMeshVertexData> vertex_data;
        vertex_data.resize(3 * mesh->mNumFaces);
        memset(&vertex_data[0], 0, vertex_data.size()*sizeof(StaticMeshVertexData));

        if (mesh->HasPositions()) {
            for (GLuint j = 0; j < mesh->mNumVertices; j++) {
                vertex_data[j].position[0] = mesh->mVertices[j].x;
                vertex_data[j].position[1] = mesh->mVertices[j].y;
                vertex_data[j].position[2] = mesh->mVertices[j].z;
            }
        }

        if (mesh->HasNormals()) {
            for (GLuint j = 0; j < mesh->mNumVertices; j++) {
                vertex_data[j].normal[0] = mesh->mNormals[j].x;
                vertex_data[j].normal[1] = mesh->mNormals[j].y;
                vertex_data[j].normal[2] = mesh->mNormals[j].z;
            }
        }

        if (mesh->HasTextureCoords(0)) {
            for (GLuint j = 0; j < mesh->mNumVertices; j++) {
                vertex_data[j].textureCoord[0] = mesh->mTextureCoords[0][j].x;
                vertex_data[j].textureCoord[1] = mesh->mTextureCoords[0][j].y;
            }
        }

        if (mesh->HasTangentsAndBitangents()) {
            for (GLuint j = 0; j < mesh->mNumVertices; j++) {
                vertex_data[j].tangent[0] = mesh->mTangents[j][0];
                vertex_data[j].tangent[1] = mesh->mTangents[j][1];
                vertex_data[j].tangent[2] = mesh->mTangents[j][2];
            }
        }
        VAOs.push_back(VAO(vertex_data, indices));

    }
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



SkeletalMeshData::SkeletalMeshData(){}
SkeletalMeshData::SkeletalMeshData(PathName filename){
    using namespace glm;

    if (filename.data.empty())return;

    MeshScene = AssimpScene::load(filename,
                                  aiProcess_CalcTangentSpace          |
                                  aiProcess_GenSmoothNormals          |
                                  aiProcess_JoinIdenticalVertices     |
                                  aiProcess_ImproveCacheLocality      |
                                  aiProcess_LimitBoneWeights          |
                                  aiProcess_RemoveRedundantMaterials  |
                                  aiProcess_SplitLargeMeshes          |
                                  aiProcess_Triangulate               |
                                  aiProcess_GenUVCoords               |
                                  aiProcess_SortByPType               |
                                  aiProcess_FindDegenerates           //aiProcess_FindInvalidData
                                  )->scene;

    if(!MeshScene) {
        QENGINE_ERROR( "cannot load the file at " + filename.data);
        QENGINE_PAUSE();
        return;
    } else {
        QENGINE_INFO(Engine::EString() + MeshScene->mNumMeshes + " models loaded!");
    }


    bonesNumber = 0;
    for(GLuint i = 0; i < MeshScene->mNumMeshes; i++) {
        const struct aiMesh* mesh;
        mesh = MeshScene->mMeshes[i];
        if(mesh)bonesNumber += mesh->mNumBones;
    }
    bonesNumber = 60;
    //bone_transforms.resize(bonesNumber);
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
            for(uint j = 0; j < mesh->mNumBones; ++j) {
                aiBone* bone = mesh->mBones[j];
                std::string bone_name(bone->mName.data);
                //std::cout << "Bone name: " << bone_name << std::endl;
                if(bone_name_hash.find(bone_name) == bone_name_hash.end()) {
                    int newId=bone_name_hash.size();
                    bone_name_hash[bone_name] = newId;
                    bone_name_hash_index[Hash(bone_name)]=newId;
                    //std::cout << "ID = " << bone_name_hash[bone_name] << std::endl;
                } else {
                    //std::cout << "Already hashed!" << std::endl;
                    //QENGINE_INFO( "\n\nAlready hashed!"+bone_name_hash[bone_name]+"\n\n");
                }

                int current_bone_id = bone_name_hash[bone_name];
                if(current_bone_id < 0) {
                    QENGINE_WARN("WARNING bone out of index:"+bone_name.data());
                } else {
                    //std::cout << "mNumWeights: " << bone->mNumWeights << std::endl;

                    for(uint k = 0; k < bone->mNumWeights; ++k) {
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

}


}
