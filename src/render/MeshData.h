#pragma once

#include "Resource.h"
#include "VAO.h"
#include <vector>
#include <AssimpScene.h>
#include <unordered_map>

class aiScene;


namespace Engine{

    class StaticMeshData : public Resource<StaticMeshData,PathName>
    {
    //private:
        friend class Resource<StaticMeshData,PathName>;
        friend class ResourceManager<StaticMeshData,PathName>;
        StaticMeshData();
        StaticMeshData(PathName filename);
    public:
        bool operator()()const {return VAOs.size();}
        std::vector<VAO> VAOs;
    };


    class SkeletalMeshData : public Resource<SkeletalMeshData,PathName>
    {
        friend class Resource<SkeletalMeshData,PathName>;
        friend class ResourceManager<SkeletalMeshData,PathName>;
        SkeletalMeshData();
        SkeletalMeshData(PathName filename);
    public:
        std::vector<VAO> VAOs;
        std::unordered_map <std::string, int> bone_name_hash;
        std::unordered_map <unsigned int, int> bone_name_hash_index;
        std::vector<glm::mat4> offset_matrix_array;
        unsigned int bonesNumber;
        const aiScene* MeshScene;
    };

    extern inline void copyAiMatrixToGLM(const aiMatrix4x4 *from, glm::mat4 &to);
}

