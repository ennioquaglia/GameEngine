#pragma once


#include "InputManager.h"
#include "InputState.h"
#include <GLFW\glfw3.h>
#include <map>

namespace Engine {

	class Context;
	class GlfwWindow;

	class GlfwInputManager : public InputManager
	{
	protected:
		friend class GlfwWindow;

		static std::map<GLFWwindow*, GlfwInputManager*> windowsMap;

		static void _keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void _mouseMoveCallback(GLFWwindow *window, double x, double y);
		static void _mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
		static void _mouseScrollCallback(GLFWwindow *window, double non_so, double delta);

		GLFWwindow* window;
		GlfwWindow* engine_window;

		InputState input, input_pre;

		void setup(GlfwWindow* w);

		GlfwInputManager();
		GlfwInputManager(GlfwWindow *w);
		~GlfwInputManager();

		friend class glfwWindow;
		virtual void Update();
	public:

        virtual bool isKeyDown(int key)const ;
        virtual bool isKeyPressed(int key)const ;
        virtual bool isKeyReleased(int key)const ;

        virtual glm::vec2 getMousePos() const  ;
        virtual glm::vec2 getMousePositionDelta() const  ;
        virtual glm::vec2 getMousePositionDeltaAndCenter(float centerZone)   ;
        virtual void setMousePos(glm::vec2 pos) ;
        virtual bool isMouseButtonDown(int button) const ;
        virtual bool isMouseButtonPressed(int button) const  ;
        virtual bool isMouseButtonReleased(int button) const  ;
        virtual int getDeltaMouseScroll() const ;

        virtual glm::vec3 getPositionMovement() const ;
        virtual glm::vec3 getPositionMovementRotated(float ang) const ;


	};

}
