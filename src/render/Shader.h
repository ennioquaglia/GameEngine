#pragma once
#pragma once

#include "Pathname.h"
#include "Resource.h"
#include "Hash.h"

#define GLEW_STATIC
#include <string>
#include <GL/glew.h>
#include "Preprocessor.h"


namespace Engine
{
    class PathName;

    class Shader
    {

	public:
        enum ShaderType{
			Default = 0,
			Vertex = 1,
			Fragment = 2,
			Geometry = 4,
			Tess_Control = 8,
			Tess_Eval = 16,
			Extra = 32
		};
		Shader();
        Shader(PathName VertexFilename, PathName FragmentFilename, EString preprocessorDefinitions="");

        void setShaderFile(PathName filename_, ShaderType shaderType);


        static void Compile(Shader* shader,ShaderType mode = Default){
            shader->compile(mode);
        }

        //GLuint compile(unsigned char mode = Default);
        GLuint compile(ShaderType mode = Default);

		bool operator==(const Shader& s) const;
		bool compare(Shader *s);

        void Use() {if (program)glUseProgram(program);}
        GLuint getProgramId() {	return program;}
        GLuint getVersion() {return version;}

        explicit operator bool() const{ return is_working;}

        bool useLight() {return use_light;}
        bool isWorking() {return is_working;}
		int getUniformLocation(std::string name);

//		void setUniform(std::string name, int value) {
//			int loc = getUniformLocation(name);
//			if (loc<0) { glGetError(); return; }
//			glUniform1i(loc, value);
//		}
//        void setUniform(std::string name, unsigned int value) {
//            int loc = getUniformLocation(name);
//            if (loc<0) { glGetError(); return; }
//            glUniform1i(loc, (int)value);
//        }
//		void setUniform(std::string name, float value) {
//			int loc = getUniformLocation(name);
//			if (loc<0) { glGetError(); return; }
//			glUniform1f(loc, value);
//		}
//        void setUniform(std::string name, glm::vec3 value) {
//            int loc = getUniformLocation(name);
//            if (loc<0) { glGetError(); return; }
//            glUniform3f(loc, value[0],value[1],value[2]);
//        }
//        void setUniform(std::string name,int count, bool transpose, glm::mat4 matrix) {
//            int loc = getUniformLocation(name);
//            if (loc<0) { glGetError(); return; }
//            glUniformMatrix4fv(loc, count, transpose, &(matrix)[0][0]);
//        }
//        void setUniform(std::string name,int count, bool transpose, glm::mat2 matrix) {
//            int loc = getUniformLocation(name);
//            if (loc<0) { glGetError(); return; }
//            glUniformMatrix2fv(loc, count, transpose, &(matrix)[0][0]);
//        }




        void setUniform(StringHash name, int value) {
            auto it=m_uniforms.find(name.getHash());
            if(it!=m_uniforms.end())
                glUniform1i(it->second, value);
        }
        void setUniform(StringHash name, unsigned int value) {
            auto it=m_uniforms.find(name.getHash());
            if(it!=m_uniforms.end())
            glUniform1i(it->second, (int)value);
        }
        void setUniform(StringHash name, float value) {
            auto it=m_uniforms.find(name.getHash());
            if(it!=m_uniforms.end())
            glUniform1f(it->second, value);
        }
        void setUniform(StringHash name, glm::vec3 value) {
            auto it=m_uniforms.find(name.getHash());
            if(it!=m_uniforms.end())
            glUniform3f(it->second, value[0],value[1],value[2]);
        }
        void setUniform(StringHash name,int count, bool transpose, glm::mat4 matrix) {
            auto it=m_uniforms.find(name.getHash());
            if(it!=m_uniforms.end())
            glUniformMatrix4fv(it->second, count, transpose, &(matrix)[0][0]);
        }
        void setUniform(StringHash name,int count, bool transpose, glm::mat2 matrix) {
            auto it=m_uniforms.find(name.getHash());
            if(it!=m_uniforms.end())
            glUniformMatrix2fv(it->second, count, transpose, &(matrix)[0][0]);
        }


		bool useDefaultAttributes;
	protected:
        std::map< unsigned , GLint > m_uniforms;
        void setupUniformLocations();




		std::map<std::string, int> locations;
        std::string m_PreprocessorDefinitions;
		GLuint getShaderId(std::string path, GLuint shader_type);

        std::string m_Filenames[6];
        GLuint m_obj_id[6];

		GLuint program;
		unsigned char programs;

		int version;
        bool use_light, is_working;
	};


}

