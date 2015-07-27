#pragma once

#define GLEW_STATIC
#include <string>
#include <GL/glew.h>
#include "../common/Preprocessor.h"

namespace Engine
{
    class Shader
    {
    public:
        enum {
            Default=0,
            Vertex = 1,
            Fragment = 2,
            Geometry = 4,
            Tess_Control = 8,
            Tess_Eval = 16,
            Extra =32
        };
        Shader();
        Shader(std::string VertexFilename,std::string FragmentFilename,Preprocessor *precpu=0);
        void setShaderFile(std::string filename,unsigned char shaderType);
        GLuint Compile(unsigned char mode=Default);

        bool operator==(const Shader& s) const;
        bool compare(Shader *s);

        GLuint getProgramId() {
            return program;
        }
        void Use() {
        	if(program)
            glUseProgram(program);
        }
        GLuint getVersion() {
            return version;
        }
        bool useLight(){
			return use_light;
		}
		bool isWorking(){
			return is_working;
		}
		int getUniformLocation(string name);
		
		void setUniform(string name,int value){
			   int loc=getUniformLocation(name);
			   if(loc<0){glGetError();return;}
			   glUniform1i(loc,value);		
		}
		void setUniform(string name,float value){
			   int loc=getUniformLocation(name);
			   if(loc<0){glGetError();return;}
			   glUniform1f(loc,value);		
		}
		
		
		bool useDefaultAttributes;
    protected:
    	std::map<std::string,int> locations;
    	Preprocessor *preProcessor;
    	GLuint getShaderId(std::string path, GLuint shader_type);
    	
        std::string vertex,fragment,geometry,tess_c,tess_e,extra;
        GLuint vs,fs,gs,tcs,tes;
		GLuint program;
        unsigned char programs;
        int version;
        
        bool use_light,is_working;
        friend class AssetManager;
    };


}
