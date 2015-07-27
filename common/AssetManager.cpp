#include "AssetManager.h"
#include "utility.h"
#include "../Render/StaticMesh.h"
#include "../Render/SkeletalMesh.h"
#include "../Render/Shader.h"
#include "../Render/Texture.h"

using namespace Engine;

AssetManager* AssetManager::istance=0;

Texture* AssetManager::addTexture(std::string filename) 
{
	
	if(!istance)return 0;
	
	if(filename.length()==0){return NULL;}
	Texture *t=0;

	if(istance->textures.find(filename)!=istance->textures.end())
	{
		t=istance->textures[filename];
	}
	else 
	{
		t=new Texture(filename);
		istance->textures[filename]=t;
	}
	return t;	
}

StaticMesh* AssetManager::addStaticMesh(std::string filename)
{

	
	if(!istance)return 0;
	
	if(filename.length()==0){return NULL;}
	StaticMesh *m=0;

	if(istance->meshes.find(filename)!=istance->meshes.end())
	{
		m=istance->meshes[filename];
	}
	else 
	{
		m=new StaticMesh(filename);
		istance->meshes[filename]=m;
	}
	return m;
}

const aiScene* AssetManager::addAssimpScene(std::string filename,unsigned int flag){
	
	if(!istance)return 0;
	
	if(filename.length()==0){return NULL;}
	const aiScene *m=0;

	if(istance->scenes.find(filename)!=istance->scenes.end())
	{
		if(istance->scenes[filename].find(flag)!=istance->scenes[filename].end()){
		   	m=istance->scenes[filename][flag];
		   	return m;
		}
	}
	
	{
		m=aiImportFile(filename.data(),flag);
		istance->scenes[filename][flag]=m;
	}
	return m;
	
}


SkeletalMesh* AssetManager::addSkeletalMesh(std::string filename) 
{
	if(!istance)return 0;
	
	if(filename.length()==0){return NULL;}
	SkeletalMesh *m=0;

	if(istance->smeshes.find(filename)!=istance->smeshes.end())
	{
		m=istance->smeshes[filename];
	}
	else 
	{
		m=new SkeletalMesh(filename);
		istance->smeshes[filename]=m;
	}
	return m;
}

bool AssetManager::addShader(Engine::Shader* shader){
	
	if(!istance)return 0;
	
	std::list<Shader*>::iterator it;

	// iterator to vector element:
	it = find (istance->shaders.begin(),istance->shaders.end(), shader);
	
	if(it!=istance->shaders.end()){
		shader->program=(*it)->program;
	}
	else
	{
		shader->Compile();
		istance->shaders.push_back(shader);
	}
	return 1;
}


void AssetManager::loadScene(std::string filename,Engine::Scene *scene){
	
	
	
	
}
