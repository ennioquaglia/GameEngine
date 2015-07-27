#pragma once
#include "../common/Camera.h"
#include "../common/Event.h"
#include "../Render/RenderFlag.h"

namespace Engine
{
	

	class ActorManager;
	class SystemManager;
	class Window;
	class GameState;
    class Camera;
    class AssetManager;
	class Actor;
	class PostProcessor;
	
	struct Options{
		Options():isShadowEnable(1),shadowLOD(10),useVSM(1){}
		bool isShadowEnable;
		int shadowLOD;
		bool useVSM;   	
		
	};
	
	class Scene{
		
		Camera debugCamera;
        bool _isUsingDebugCamera;
        
        //ScriptLoader script;
        
        unsigned int counter;
        inline unsigned int getUnivocalId(){return counter++;}
        
    public:
    	Options options;
    	ActorManager *actorManager;
        SystemManager *systemManager;
        Window *window;
        GameState *state;
		PostProcessor *effect;
		
		Scene();
		void addActor(Actor* obj);
        void setCamera(Camera cam);
		void setEffect(PostProcessor *e);
		
        AssetManager* mAsset;
        Camera camera;
        Camera *current_camera;
        
        
        void setWindow(Window *w);
        

		bool isUsingDebugCamera();
        void useDebugCamera(bool use=true);

        virtual void Update(float deltaTimeMs);

        virtual void Draw(uint flag=RenderFlag::Default);
        virtual void Render();
        virtual void handleEvent(Event &msg);
        virtual void Init();
        virtual void Cleanup();
    };
	
}

