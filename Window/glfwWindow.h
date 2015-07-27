#pragma once


#include "Window.h"
#include <map>

#ifdef ENGINE_USE_GLFW	
  
class GLFWwindow;
	
namespace Engine{
	
	class glfwWindow;
	class glfwInputManager: public InputManager
	{
		protected:

			static map<GLFWwindow*,glfwInputManager*> windowsMap;
			
			static void _keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
			static void _mouseMoveCallback(GLFWwindow *window,double x,double y);
			static void _mouseButtonCallback(GLFWwindow *window,int button,int action,int mods);
			static void _mouseScrollCallback(GLFWwindow *window,double non_so,double delta);
			
			GLFWwindow* window;
			glfwWindow* engine_window;
			
			MessageQueue keyQueue,mouseQueue,joystickQueue,generalQueue,extraQueue;
			InputState input,input_pre;
			
			void setup(glfwWindow* w);
			
			
			glfwInputManager():window(0){}
			~glfwInputManager(){} 	
			
			friend class glfwWindow;
			virtual void update();
		public:	

			virtual bool isKeyDown(int key);
			virtual bool isKeyPressed(int key);
			virtual bool isKeyReleased(int key);
			
			virtual vec2 getMousePos();
			virtual vec2 getMousePositionDelta();
			virtual vec2 getMousePositionDeltaAndCenter(float centerZone);
			virtual void setMousePos(vec2 pos);
			virtual bool isMouseButtonDown(int button);
			virtual bool isMouseButtonPressed(int button);
			virtual bool isMouseButtonReleased(int button);
			virtual int getDeltaMouseScroll();
			
			virtual vec3 getPositionMovement();
			virtual vec3 getPositionMovementRotated(float ang);
			
			
	};
	
	class glfwWindow: public Window{
		
		GLFWwindow* window;
		
		public:
				
					    
			glfwWindow(){
			
				gameMain=0;
				for(int i=0;i<256;i++)input.key[i]=0;
				input_pre=input;
				mConsoleLineIndex=0;
				state=0;
			}
			~glfwWindow(){}

			void setup();
			int exec();
			void setMousePos(glm::vec2 pos);
			void setGameMain(voidFunctor f){
				gameMain=f;
			}
			
			void appConsoleLine(std::string str);
			int finalize();
			void makeCurrent();
			virtual void poolEvents();
			
			friend class glfwInputManager;
	};
}
	
#endif