#pragma once

//#include "utility.h"

#include <string>
#include <list>

namespace Engine{
	
	class Texture;
	class Shader;
	
	class Material
	{
		Texture* diffuse,*specular,*normal;
		float shininess;
		Shader *shader;
		public:
		
		Material();
		void setDiffuse(std::string source);
		void setSpecular(std::string source);
		void setNormal(std::string source);
		void setShininess(float value);
		void setShader(Shader* s,bool alredy_use_program=0);
		void updateShader(Shader* s);//?
		void bind();
		
		static Material* getDefaultMaterial();
		static void setupShaderUniform(Shader *s);
	};
	
	typedef std::list<Material*> MaterialList;

}