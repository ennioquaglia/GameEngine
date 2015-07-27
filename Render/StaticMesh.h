#pragma once
#include "vao.h"
#include <string>
#include <vector>
#include "Material.h"
#include "../common/Debug.h"
#include "../common/GlobalsManager.h"
#include "RenderFlag.h"

using namespace std;
class aiNode;

namespace Engine
{

    class EmptyStaticMesh{
        public:
            EmptyStaticMesh(string filename = ""){}
            virtual void draw(MaterialList m = MaterialList(),Shader *s=0,uint flag=RenderFlag::Default){}
        protected:
            virtual bool Import(std::string filename){}
            virtual bool Import(aiNode &node){}
    };
    class StaticMesh_330 : public EmptyStaticMesh
    {
    	string mFilename;
    public:
    	StaticMesh_330(string filename = ""): mFilename(filename)
        {
            Import(filename);
        }
    	virtual void draw(MaterialList m = MaterialList(),Shader *s=0,uint flag=RenderFlag::Default);
    protected: 

        bool Import(std::string filename);
		
		vector<VAO> VAOs; 	
	};
	
    class StaticMesh_330_old : public EmptyStaticMesh
    {
        string mFilename;

    public:
        StaticMesh_330_old(string filename = ""): mFilename(filename)
        {
            Import(filename);
        }

        struct VertexData
        {
            GLfloat position[3];
            GLfloat normal[3];
            GLfloat textureCoord[2];
            GLfloat tangent[3];
        };

        virtual void draw(MaterialList m = MaterialList(),Shader *s=0,uint flag=RenderFlag::Default);

    protected: 

        bool Import(std::string filename);

        std::vector<GLuint> num_indices;
        //std::vector<GLuint> vertex_data_buffers;
        std::vector<GLuint> mesh_materials;
        std::vector<GLuint> vao;
        vector<GLuint> indicesSize;

        std::vector<GLuint> index_buffers;
        std::vector<GLuint> vbo_buffers;
    };
    /*
    class StaticMesh_120 : public EmptyStaticMesh
    {
        string mFilename;

    public:
        StaticMesh_120(string filename = "",Shader *s=0): mFilename(filename)
        {
            Import(filename,s);
        }

        struct VertexData
        {
            GLfloat position[3];
            GLfloat normal[3];
            GLfloat textureCoord[2];
            GLfloat tangent[3];
        };

        virtual void draw(MaterialList m = MaterialList());

    protected:


        bool Import(std::string filename,Shader *s=0);

        std::vector<GLuint> num_indices;
        //std::vector<GLuint> vertex_data_buffers;
        std::vector<GLuint> mesh_materials;
        std::vector<GLuint> vao;
        vector<GLuint> indicesSize;

        std::vector<GLuint> index_buffers;
        std::vector<GLuint> vbo_buffers;
    };*/
    
    class StaticMesh_120 : public EmptyStaticMesh
    {
    	struct VertexData
        {
            GLfloat position[3];
            GLfloat normal[3];
            GLfloat textureCoord[2];
            GLfloat tangent[3];
        };
        string mFilename;
		std::vector<std::vector<GLuint> > Indices;
		std::vector<std::vector<VertexData> > vertex_datas;
		std::vector<GLuint> listId;
    public:
        StaticMesh_120(string filename = ""): mFilename(filename)
        {
            Import(filename);
        }

        

        virtual void draw(MaterialList m = MaterialList(),Shader *s=0,uint flag=RenderFlag::Default);

    protected:


        bool Import(std::string filename);

        std::vector<GLuint> num_indices;
        //std::vector<GLuint> vertex_data_buffers;
        std::vector<GLuint> mesh_materials;
        std::vector<GLuint> vao;
        vector<GLuint> indicesSize;

        std::vector<GLuint> index_buffers;
        std::vector<GLuint> vbo_buffers;
    };
    
    
    class StaticMesh
    {
        string mFilename;
        EmptyStaticMesh* mesh;
    public:
        StaticMesh(string filename = ""): mFilename(filename)
        {
            float glslVersion=GlobalsManager::Istance()->get("glsl_version");
            //Debug("Mesh version:"+glslVersion);
            if(glslVersion<3.30){
            	//Debug(120);
				mesh=new StaticMesh_120(filename);
			}else{
				//Debug(330); 
				mesh=new StaticMesh_330(filename);
			}
            //mesh->Import(filename);
        }
        virtual void draw(MaterialList m = MaterialList(),Shader *s=0,uint flag=RenderFlag::Default){
            if(mesh){
                mesh->draw(m,s);   
            }   
        }
    protected:
        bool Import(std::string filename);
    };
}
