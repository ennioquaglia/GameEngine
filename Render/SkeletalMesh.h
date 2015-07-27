#pragma once
#include "vao.h"
#include "RenderFlag.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Material.h"
#include <map>

using namespace std;

class aiScene;

namespace Engine
{
    class SkeletalMesh
    {
        string mFilename;

    public:
        SkeletalMesh(string filename=""):mFilename(filename) {
            Import(filename);
        }

        virtual void draw(MaterialList m = MaterialList(),Shader *s=0,unsigned int flag=RenderFlag::Default);
        virtual void setAnimTime(float time);

    protected:


        bool Import(std::string filename);
        vector<VAO> VAOs; 
        
        std::vector<glm::mat4> bone_transforms;
        std::map <std::string, int> bone_name_hash;
        std::vector<glm::mat4> offset_matrix_array;

        const aiScene* MeshScene;

        friend class SkeletalMeshManager;
    };
    
    
    class SkeletalMesh_old
    {
        string mFilename;

    public:
        SkeletalMesh_old(string filename=""):mFilename(filename) {
            Import(filename);
        }

        struct VertexData {
            GLfloat position[3];
            GLfloat normal[3];
            GLfloat textureCoord[2];
            GLfloat tangent[3];
            GLfloat boneWeights[4];
            GLfloat boneIDs[4];
        };

        virtual void draw(MaterialList m = MaterialList(),Shader *s=0,unsigned int flag=RenderFlag::Default);
        virtual void setAnimTime(float time);

    protected:


        bool Import(std::string filename);

        std::vector<GLuint> num_indices;
        //std::vector<GLuint> vertex_data_buffers;
        std::vector<GLuint> mesh_materials;
        std::vector<GLuint> vao;
        vector<GLuint> indicesSize;

        std::vector<GLuint> index_buffers;
        std::vector<GLuint> vbo_buffers;

        std::vector<glm::mat4> bone_transforms;
        std::map <std::string, int> bone_name_hash;
        std::vector<glm::mat4> offset_matrix_array;

        const aiScene* MeshScene;

        friend class SkeletalMeshManager;
    };

    class SkeletalMeshManager
    {
    public:
		typedef int AnimID;
        enum {
            none=0,
            idle,
            usertype
        };
		struct AnimType {
            AnimID id;  //yes or no? redundant?
            int firstFrame;
            int lastFrame;
            int duration;
            const aiScene* scene; 	
        };
    private:
    	
    	
		typedef map<AnimID,AnimType> Animations;

        float time,preTime;
        bool finished,isPlaying;;

        Animations animations;
        AnimID currentAnim,nextAnim;
        
        std::vector<glm::mat4> bone_transforms,bone_transforms_pre;
        void setAnimTime(AnimType* CurrentAnim,float time);
        
        string mFilename;
    public:

        SkeletalMeshManager(string filename=""):mFilename(filename),time(0),currentAnim(idle),nextAnim(idle) {
            init();
        }
        void init();
        void update(float deltaTimeMs);
        void updateUniform(GLuint programID);
        void draw(MaterialList m = MaterialList(),Shader *s=0,unsigned int flag=RenderFlag::Default);
        void addAnimation(AnimID anim,int firstFrame,int lastFrame);
        void addAnimation(AnimID anim,string filename,int firstFrame=0,int lastFrame=0);
        void removeAnimation(AnimID anim);
        void playAnimation(AnimID anim);
        void setAnimation(AnimID anim);
        SkeletalMesh *mesh;
    
	protected:
		AnimType* getAnimation(AnimID anim);
    };
}
