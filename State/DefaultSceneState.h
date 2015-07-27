#pragma once

#include "../State/State.h"
#include "../Window/Window.h"
#include "../Core/Object.h"
#include "../common/ScriptLoader.h"
#include "../common/Camera.h"

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

    class DefaultSceneState : public GameState
    {
    public:
    	Scene *scene;
    	Window *window;
    	
		DefaultSceneState();
        
        void setWindow(Window *w);
		void makeCurrentContext();
        
        virtual void Update(float deltaTimeMs);

        virtual void Draw();
        virtual void handleEvent(Event &msg);

        virtual void Init();
        virtual void Cleanup();
        virtual void Pause();
        virtual void Resume();
    };



}
