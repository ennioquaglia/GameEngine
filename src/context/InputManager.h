#pragma once

#include "InputState.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>


namespace Engine
{

	class Window;
	class ActorComponent;
	class Actor;


	class InputManager
	{
	protected:
		/*
		list<ActorComponent*> components;
		list<Actor*> actors;

		
		void send(Event &e);
		*/
		InputManager();

		friend class Window;
        friend class Context;
	public:
        virtual ~InputManager();
		virtual void Update() {}
		/*
		virtual void attachObserver(ActorComponent *a);
		virtual void detachObserver(ActorComponent *a);
		virtual void attachObserver(Actor *a);
		virtual void detachObserver(Actor *a);

		*/
        virtual bool isKeyDown(int key)const = 0;
        virtual bool isKeyPressed(int key)const = 0;
        virtual bool isKeyReleased(int key)const = 0;

        virtual glm::vec2 getMousePos() const  = 0;
        virtual glm::vec2 getMousePositionDelta() const  = 0;
        virtual glm::vec2 getMousePositionDeltaAndCenter(float centerZone)   = 0;
        virtual void setMousePos(glm::vec2 pos) = 0;
        virtual bool isMouseButtonDown(int button) const = 0;
        virtual bool isMouseButtonPressed(int button) const  = 0;
        virtual bool isMouseButtonReleased(int button) const  = 0;
        virtual int getDeltaMouseScroll() const = 0;

        virtual glm::vec3 getPositionMovement() const = 0;
        virtual glm::vec3 getPositionMovementRotated(float ang) const = 0;

	};

    class DummyInputManager : public InputManager
    {
    protected:

        virtual void Update(){}
    public:


        InputState input, input_pre;
        virtual bool isKeyDown(int key)const;
        virtual bool isKeyPressed(int key)const;
        virtual bool isKeyReleased(int key)const;

        virtual glm::vec2 getMousePos() const;
        virtual glm::vec2 getMousePositionDelta() const;
        virtual glm::vec2 getMousePositionDeltaAndCenter(float centerZone);
        virtual void setMousePos(glm::vec2 pos);
        virtual bool isMouseButtonDown(int button) const;
        virtual bool isMouseButtonPressed(int button) const;
        virtual bool isMouseButtonReleased(int button) const;
        virtual int getDeltaMouseScroll() const;

        virtual glm::vec3 getPositionMovement() const;
        virtual glm::vec3 getPositionMovementRotated(float ang) const;

    };


};


