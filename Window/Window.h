#pragma once


#include "../common/utility.h"
#include "../Input/InputManager.h"
#include "../Input/InputEvent.h"

#ifndef ENGINE_USE_GLFW
#ifndef ENGINE_USE_SFML

#define ENGINE_USE_GLFW 	//use default gflw 

#endif
#endif


namespace Engine
{
    
	class GameState;    
    class Application;
    typedef void (__stdcall *voidFunctor)();
    

	class Window{
		protected:
					
			voidFunctor gameMain;
			int mConsoleLineIndex;
			static const int mMaxNumberLineConsole = 11;
			std::string mConsoleLine[mMaxNumberLineConsole];
			
			
			GameState* state;
			Application* application;
			
		public:
		    InputManager* inputManager;
            InputState input,input_pre;
			int Width, Height;	
					    
			Window();
			virtual ~Window();

			virtual void setup()=0;
			virtual void setState(GameState* s);
			virtual void setApplication(Application* app);
			virtual int exec()=0;
			virtual void poolEvents(){};
			virtual void setMousePos(glm::vec2 pos){}
			virtual void setGameMain(voidFunctor f);
			
			virtual void appConsoleLine(std::string str){}
			virtual void setFocus();
			virtual void lostFocus(){}
			void debugInput();
			virtual void makeCurrent(){}
			virtual void handleEvent(Event &msg);
			friend class inputManager;
	};
	
	
	
	
	
	//######//implementations//######//
	
	
#ifdef ENGINE_USE_SFML
	class sfmlWindow{
		sf::RenderWindow window;
		sf::Font font;
		
		voidFunctor gameMain;
		int mConsoleLineIndex;
		sf::Text mConsoleText;
		static const int mMaxNumberLineConsole = 11;
		std::string mConsoleLine[mMaxNumberLineConsole];
		
		
		
		public:
		    InputState input,input_pre;
            SceneManager* scene;
				
					    
			Window(){
				#ifdef ENGINE_USE_GLFW
				#undef ENGINE_USE_GLFW
				#endif

				
				gameMain=0;
				for(int i=0;i<256;i++)input.key[i]=0;
				input_pre=input;
				mConsoleLineIndex=0;
				scene=0;
			}
			~Window();

			void setup();
			int exec();
			void setMousePos(glm::vec2);
			void setGameMain(voidFunctor f){
				gameMain=f;
			}
			
			void appConsoleLine(std::string str);
			void setFocus();
			void lostFocus();
	};
#endif
	


	extern Window *current_window;
	   
	   
	   	
}