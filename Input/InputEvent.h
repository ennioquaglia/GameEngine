#pragma once

namespace Engine{
	
	struct MouseState
	{
	    enum Button{
	        LEFT_BUTTON   =0,
	        MIDDLE_BUTTON =1,
	        RIGHT_BUTTON  =2
	    };
	
	    float x,y;
	    bool button[3],isMoving;
		int wheelDelta;
		
		MouseState(){
			x=y=0;
			button[0]=0;
			button[1]=0;
			button[2]=0;
			isMoving=0;
			wheelDelta=0;
		}
	};
	
	struct JoystickState
	{
		bool button[10];
		glm::vec2 rightAnalog,leftAnalog,POV;
		float triggerLeft,triggerRight;
		JoystickState(){
			for(int i=0;i<10;i++)button[i]=0;
			triggerLeft=triggerRight=0;
		}
	};
	
	struct InputState
	{
	    bool key[350];
	    MouseState mouse;
		JoystickState joystick[4];
		
		InputState(){
			for(int i=0;i<350;i++)key[i]=0;
		}
	};
	
	
	struct KeyboardEvent{
		int key;
		int action;
	};
	struct MouseButtonEvent{
		int button;
		int action;
	};
	struct MouseMoveEvent{
		vec2 position;
	};

	class InputEvent : public Event
    {
  	protected:
	  	int source;		
        int action;
    public:
        enum {
            none=0,
            keyboard,
            mouse,
            joystick,
            extra
        };
        
        InputEvent(int source=none){
        	this->source=source;
        	action=0;
		}
        
        virtual	inline void setAction(int action){this->action=action;}
        virtual	inline void setSource(int source){this->source=source;}
        
		virtual	inline int getSource() const {return source;} 
		virtual	inline int getAction() const {return action;} 	 
    };
    
    
    
	
}