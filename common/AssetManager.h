#pragma once

#include <string>
#include <map>
#include <list>

using namespace std;

class aiScene;
namespace Engine
{
	class Texture;
	class StaticMesh;
	class SkeletalMesh;
	class Shader;
	class Scene;

	
	class AssetManager
	{
		 
		
		static AssetManager* istance;
	//private:
	public:
		map<string,StaticMesh*> meshes;
		map<string,Texture*> textures;
		map<string,SkeletalMesh*> smeshes;
		map<string,map<unsigned int,const aiScene*> > scenes;
		list<Shader*> shaders;
		string basePath;
	public:
	    AssetManager(){}
	    
	    inline static void setIstance(AssetManager* i){istance=i;}
	    inline static AssetManager* getIstance(){return istance;}
	    
	    ~AssetManager(){}
	    static Engine::Texture* addTexture(std::string filename);
	    static Engine::StaticMesh* addStaticMesh(std::string filename);
	    static Engine::SkeletalMesh* addSkeletalMesh(std::string filename);
	    static const aiScene* addAssimpScene(std::string filename,unsigned int flag);
	    static bool addShader(Engine::Shader* shader);
	    
	    void loadScene(std::string filename,Engine::Scene *scene);
		static inline string getBasePath() { if(istance)return istance->basePath;return "";}
		static inline void setBasePath(string str) { if(istance)istance->basePath=str;}
	};
	

	
}