#pragma once

#include "../common/Event.h"




namespace Engine
{
	
    class Window;
    class ActorComponent;
	class Actor;
    
    
    class InputManager
    {
    protected:
        list<ActorComponent*> components;
        list<Actor*> actors;
        
        InputManager() {}
        void send(Event &e);
        
        friend class Window;
    public:
		virtual void update(){}
		
        virtual void attachObserver(ActorComponent *a);
        virtual void detachObserver(ActorComponent *a);
        virtual void attachObserver(Actor *a);
        virtual void detachObserver(Actor *a);


        virtual bool isKeyDown(int key)=0;
        virtual bool isKeyPressed(int key)=0;
        virtual bool isKeyReleased(int key)=0;

        virtual vec2 getMousePos()=0;
        virtual vec2 getMousePositionDelta()=0;
        virtual vec2 getMousePositionDeltaAndCenter(float centerZone)=0;
        virtual void setMousePos(vec2 pos)=0;
        virtual bool isMouseButtonDown(int button)=0;
        virtual bool isMouseButtonPressed(int button)=0;
        virtual bool isMouseButtonReleased(int button)=0;
        virtual int getDeltaMouseScroll()=0;

        virtual vec3 getPositionMovement()=0;
        virtual vec3 getPositionMovementRotated(float ang)=0;
        
    };


};
