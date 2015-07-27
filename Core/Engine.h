#pragma once

//#include "utility.h"
#include "../Window/Window.h"
#include "Object.h"
#include "../common/ScriptLoader.h"
#include "../common/GlobalsManager.h"
#include "../common/Camera.h"

using namespace glm;
using namespace std;

namespace Engine
{


    class StaticMesh;
    class SkeletalMesh;
    class Texture;
    class Material;
    class Object;
    class Skybox;
    class GameObject;
    class Camera;
    class AssetManager;

    //typedef SceneManager DefaultSceneState;
    
    class SceneManager
    {
        Camera debugCamera;
        bool isUsingDebugCamera;
        
        ScriptLoader script;
        
        unsigned int counter;
    public:
    	ActorManager *actorManager;
        SystemManager *systemManager;
    	
        SceneManager();
        ~SceneManager();
        int Setup();
        int reload();

        void Render(float deltaTimeMs);
        void act(float deltaTimeMs);
        void draw(float deltaTimeMs);
		void PoolEvents(list<Event*> events){}
		
        void addActor(Actor* obj);
        void setCamera(Camera cam);

        AssetManager* mAsset;
        Camera camera;
        Camera *current_camera;
        float fps;
        
        Window *window;
        void setWindow(Window *w){window=w;}
        void makeCurrentContext(){if(!window)return;window->makeCurrent();}
        
        
		unsigned int getUnivocalId(){return counter++;}
	
		bool IsUsingDebugCamera(){return isUsingDebugCamera; }
        void useDebugCamera(bool use=true) {
        	Debug("UseDebugCamera="+use);
            isUsingDebugCamera=use;
            if(use) {
                    debugCamera=camera;
                    debugCamera.setMode(Camera::Rot);
                    current_camera=&debugCamera;
                }
            else {
                    current_camera=&camera;
                }
        }
        
    };


}
