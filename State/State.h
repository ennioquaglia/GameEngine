#pragma once

#include <stack>
#include "../common/Event.h"
namespace Engine
{



	class DefaultSceneState;
    class Window;

    class GameState
    {

    public:
        GameState() {}
        ~GameState() {Cleanup();}
        virtual void Update(float deltaTimeMs) {}
        virtual void Draw() {}
        virtual void handleEvent(Event &msg){}
        
        virtual void Init() {}
        virtual void Cleanup() {}
		virtual void Pause() {}
        virtual void Resume() {}
    };
    

    
    class DefaultMenuState : public GameState
    {

    public:
        DefaultMenuState() {}
        virtual void Update(float deltaTimeMs);
		
        virtual void Draw();
        virtual void handleEvent(Event &msg);
        
        virtual void Init();
        virtual void Cleanup();
		virtual void Pause();
        virtual void Resume();
    };
    

 	
    
	class DefaultGameStateManager : public GameState
    {
    
    protected:	
		std::stack<GameState*> StateStack;
		Window* window;
    public:	
    	DefaultMenuState* menuState;
    	DefaultSceneState* sceneState;
    	
        DefaultGameStateManager(Window *w) {
			window=w;
			menuState=0;
			sceneState=0;
		}
        virtual void Update(float deltaTimeMs);
		
        virtual void Draw();
        virtual void handleEvent(Event &msg);
        
        virtual void Init();
        virtual void Cleanup();
		virtual void Pause();
        virtual void Resume();
        
        virtual void ChangeState(GameState* newState);
        virtual void PushState(GameState* newState);
        virtual void PopState();
        
    };



};
