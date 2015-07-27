#include "Material.h"
#include "Texture.h"
#include "../common/AssetManager.h"

#include "Shader.h"
using namespace Engine;



Material* Material::getDefaultMaterial()
{
	static Material *mat = new Material();
	return mat;
}
void Material::setupShaderUniform(Shader *s)
{
	glUniform1i(glGetUniformLocation(s->getProgramId(), "diffuseMap"), 0);
	glUniform1i(glGetUniformLocation(s->getProgramId(), "normalMap"), 2);
	glUniform1i(glGetUniformLocation(s->getProgramId(), "specularMap"), 1);
	glDebug();
}

Material::Material()
{
	diffuse = AssetManager::addTexture("#666666FF");
	specular = AssetManager::addTexture("#666666FF");
	normal = AssetManager::addTexture("normal");
	shininess = 5;
}
void Material::setDiffuse(string source)
{
	diffuse = AssetManager::addTexture(source);
}
void  Material::setSpecular(std::string source)
{
	specular = AssetManager::addTexture(source);
}
void  Material::setNormal(std::string source)
{
	normal = AssetManager::addTexture(source);
}
void  Material::setShininess(float value)
{
	shininess = value;
}

void  Material::setShader(Shader* s, bool alredy_use_program)
{
	if(s == NULL)return;
	shader = s;
	glDebug();
	if(!alredy_use_program)
		shader->Use();

	uint p = shader->getProgramId();

	
	
	shader->setUniform("diffuseMap", 0);
	shader->setUniform("specularMap", 1);
	shader->setUniform("normalMap", 2);
	shader->setUniform("shininess", shininess);

	glDebug();
	//if(!alredy_use_program)
	//	glUseProgram(0);
	glDebug();
}

void Material::updateShader(Shader* s)
{
	shader->setUniform("shininess", shininess);
	glGetError();
}

void Material::bind()
{
	glActiveTexture(GL_TEXTURE0);
	diffuse->bind();
	//glBindTexture(GL_TEXTURE_2D, diffuse->getID());
	glActiveTexture(GL_TEXTURE1);
	specular->bind();
	//glBindTexture(GL_TEXTURE_2D, specular->getID());
	glActiveTexture(GL_TEXTURE2);
	normal->bind();
	//glBindTexture(GL_TEXTURE_2D, normal->getID());

}

void Texture::load(std::string filename)
{

	this->filename = filename;

	{
		//one pixel texture
		float rgba[4] = { -1, 0, 0, 1};
		if(filename[0] == '#') {
			//float rgba[4]= {0,0,0,1};
			for(uint i = 0; i < 4; i++) {
				int num = 0;
				string c = filename.substr(1 + i * 2, 2);
				sscanf(c.data(), "%x", &num);
				//printf("%s-%x- ",c.data(),num);
				rgba[i] = (float)num;
				rgba[i] /= (float)255;
				//printf("%f ",rgba[i]);
			}
			if(filename.size() == 7)rgba[3] = 1;
		}
		if(filename == "white") {
			rgba[0] = 1;
			rgba[1] = 1;
			rgba[2] = 1;
		}
		if(filename == "black") {
			rgba[0] = 0;
		}
		if(filename == "normal") {
			rgba[0] = 0.5;
			rgba[1] = 0.5;
			rgba[2] = 1;
			rgba[3] = 0;

		}
		if(rgba[0] >= 0) {
			glDebug();
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, rgba);

			glDebug();
			return;
		}
	}

	if(filename == "noise512") {
		glDebug();
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		const int size = 512;
		vector<float> data(size * size * 4);
		for(int i = 0; i < size * size; i++) {
			vec3 n = glm::linearRand(0.1f,1.0f)*glm::sphericalRand(1.0f) / 2.0f + vec3(0.5f);
			data[i * 4 + 0] = n.x;
			data[i * 4 + 1] = n.y;
			data[i * 4 + 2] = n.z;
			data[i * 4 + 3] = (n.x + n.y + n.z) / 3.;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_FLOAT, &data[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glDebug();
		return;
	}

	texture = SOIL_load_OGL_texture(
				  filename.c_str(),
				  SOIL_LOAD_AUTO,
				  SOIL_CREATE_NEW_ID,
				  SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
			  );

	if(!texture) {
		printf("SOIL loading error(%s): '%s'\n", filename.c_str(), SOIL_last_result());
		return;
	}


	glBindTexture(GL_TEXTURE_2D, texture);
	glDebug();
	//gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, 1, 1, GL_RGBA, GL_FLOAT, pixel);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glDebug();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glDebug();

	//GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
	GLfloat fLargest = 1;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

	glDebug();
	Debug("new Texture (" + filename + ")");
}
