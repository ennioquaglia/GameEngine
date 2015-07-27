#include "glfwWindow.h"
#include "../State/State.h"
#include "../Core/Application.h"

using namespace Engine;

#define ENGINE_USE_GLFW

static void errorCallback(int error, const char* description)
{
    fputs(description, stderr);
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	glfwWindow* w=(glfwWindow*)glfwGetWindowUserPointer(window);
    if(!w)return;
    
    EventData<glm::vec2> e;
	e.setType(StringHash("windowResize").getHash());
	e.setData(vec2(width, height));
    
    w->handleEvent(e);
}
/*
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    glfwWindow* w=(glfwWindow*)glfwGetWindowUserPointer(window);
    if(!w)return;


    w->input.key[key]=action;
}
static void mouseMoveCallback(GLFWwindow *window,double x,double y)
{
	
    glfwWindow* w=(glfwWindow*)glfwGetWindowUserPointer(window);
    if(!w)return;

	w->input.mouse.x = x;
    w->input.mouse.y = y;
}

static void mouseButtonCallback(GLFWwindow *window,int button,int action,int mods)
{
    glfwWindow* w=(glfwWindow*)glfwGetWindowUserPointer(window);
    if(!w)return;

    switch(button) {
            case GLFW_MOUSE_BUTTON_LEFT :
                w->input.mouse.button[MouseState::LEFT_BUTTON] = action;
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                w->input.mouse.button[MouseState::RIGHT_BUTTON] = action;
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                w->input.mouse.button[MouseState::MIDDLE_BUTTON] = action;
                break;
            default:break;
	}
}

static void mouseScrollCallback(GLFWwindow *window,double non_so,double delta)
{
	glfwWindow* w=(glfwWindow*)glfwGetWindowUserPointer(window);
    if(!w)return;
    w->input.mouse.wheelDelta=delta;
}

*/ 
void glfwWindow::setMousePos(glm::vec2 pos)
{
	if(pos.x<0||pos.y<0)pos=vec2(Width/2,Height/2);
	glfwSetCursorPos(window,pos.x,pos.y);
}
int glfwWindow::finalize()
{
    glfwDestroyWindow(window);
    // When you are done using GLFW, typically at the very end of the program,
    // you need to call glfwTerminate.
    glfwTerminate();
}
void glfwWindow::setup()
{


	setFocus();
    glfwSetErrorCallback(errorCallback);
    
    
    
    // Before you can use most GLFW functions, the library must be initialized.
    // This is done with glfwInit, which returns non-zero if successful,
    // or zero if an error occurred.
    if(!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint (GLFW_SAMPLES, 8);

	//window = glfwCreateWindow(mode->width, mode->height, "QEngine", monitor, NULL);
	
    window = glfwCreateWindow(1024, 768, "QEngine", NULL, NULL);
    if(!window) {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }	
    glfwMakeContextCurrent(window);
    
   // glfwSetInputMode( window , GLFW_CURSOR , GLFW_CURSOR_HIDDEN );

   	 
	Engine::glfwInputManager *glfwInput=new Engine::glfwInputManager();	  
    inputManager=glfwInput;
    glfwInput->setup(this);
    
    //glfwSetKeyCallback(window, keyCallback);
    // glfwSetCursorPosCallback(window,mouseMoveCallback);
    // glfwSetMouseButtonCallback(window,mouseButtonCallback);
    // glfwSetScrollCallback(window,mouseScrollCallback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetWindowUserPointer(window, (void*)this);


	
    //scene=new SceneManager();
    //scene->setWindow(this);
    //scene->Setup();
    //scene->useDebugCamera();


    Width=1024;
	Height=768;
    //glfwGetWindowSize(window, &width, &height);

    // Set the color and depth clear values
    glClearDepth(1.f);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    // Enable Z-buffer read and write
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    // Disable lighting and texturing
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);


    // Configure the viewport (the same size as the window.
    glViewport(0, 0, Width,Height);

    // Setup a perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();


    GLfloat ratio =Width/Height;

    glFrustum(-ratio, ratio, -1.f, 1.f, 1.f, 500.f);


    debug("window context...[ OK ]");
}

void glfwWindow::makeCurrent(){glfwMakeContextCurrent(window);}


int glfwWindow::exec()
{

    float deltaTime,currentFrame,lastFrame;
    lastFrame=glfwGetTime();
    
    while(!glfwWindowShouldClose(window)) {
    		glDebug();
            currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

			//debugInput();
            if(gameMain)gameMain();
			if(state){
				glDebug();
				state->Update(deltaTime*1000);
				glDebug();
			}		
			if(application){
				application->update();
			}
            float ratio;

	        glfwGetFramebufferSize(window, &Width, &Height);
	        ratio = Width / (float) Height;
	        GlobalsManager::Istance()->set("window_width",Width);
	        GlobalsManager::Istance()->set("window_height",Height);
	         
    		glViewport(0, 0, Width,Height);

		    // Setup a perspective projection
		    glMatrixMode(GL_PROJECTION);
		    glLoadIdentity();
		    gluPerspective(60,ratio,0.1,500);
		    glMatrixMode(GL_MODELVIEW);
	        glLoadIdentity();
	        
	        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	        
      		//Debug(deltaTime+"s-  "+deltaTime*1000+"ms-  "+(1/(deltaTime))+"fps");
      		
      		
	  	    if(state){
	  	    	glDebug();
				glPushMatrix();
				//scene->Update(deltaTime*1000);
				state->Draw();//(deltaTime*1000);
				glPopMatrix();
				glDebug();
			}
			inputManager->update();
            
            
            // Swap buffers
            glfwSwapBuffers(window);
			
            poolEvents();
            
            glfwPollEvents();
            	
			glDebug();
  
        }
    return finalize();

}


void glfwWindow::poolEvents(){

}
void glfwWindow::appConsoleLine(std::string str)
{

}






bool glfwInputManager::isKeyDown(int key)
{
    return input.key[key];
}
bool glfwInputManager::isKeyPressed(int key)
{
    return input.key[key]&&!input_pre.key[key];
}
bool glfwInputManager::isKeyReleased(int key)
{
    return !input.key[key]&&input_pre.key[key];
}


vec2 glfwInputManager::getMousePos()
{
    return vec2(input.mouse.x,input.mouse.y);
}
vec2 glfwInputManager::getMousePositionDelta()
{
    return vec2(input_pre.mouse.x,input_pre.mouse.y)-vec2(input.mouse.x,input.mouse.y);
}
vec2 glfwInputManager::getMousePositionDeltaAndCenter(float centerZone)
{
	vec2 size=vec2(engine_window->Width,engine_window->Height)/2.0f;
	vec2 pos=getMousePos();

	float centro=centerZone;
	if(pos.x>(size.x+centro)||pos.x<(size.x-centro)||pos.y>(size.y+centro)||pos.y<(size.y-centro)){
		setMousePos(vec2(-1));
		float deltaX=(float)pos.x-(size.x);
		float deltaY=(float)pos.y-(size.y);
		return vec2(deltaX,deltaY);
	}	
    return vec2(0);
}
void glfwInputManager::setMousePos(vec2 pos)
{
	
	engine_window->setMousePos(pos);
	_mouseMoveCallback(window,pos.x,pos.y);
}
bool glfwInputManager::isMouseButtonDown(int button)
{
    return input.mouse.button[button];
}
bool glfwInputManager::isMouseButtonPressed(int button)
{
    return input.mouse.button[button]&&!input_pre.mouse.button[button];
}
bool glfwInputManager::isMouseButtonReleased(int button)
{
    return !input.mouse.button[button]&&input_pre.mouse.button[button];
}
int glfwInputManager::getDeltaMouseScroll()
{
    return input.mouse.wheelDelta;
}
vec3 glfwInputManager::getPositionMovement()
{
    float x=0,y=0,z=0;
    float xrotrad = 0;

#ifdef ENGINE_USE_SFML

    if(isKeyDown(sf::Keyboard::W))debug("sfml input si");
    else debug("sfml input no");

    if(isKeyDown(sf::Keyboard::W))z++;
    if(isKeyDown(sf::Keyboard::A))x--;
    if(isKeyDown(sf::Keyboard::S))z--;
    if(isKeyDown(sf::Keyboard::D))x++;
    if(isKeyDown(sf::Keyboard::Q))y--;
    if(isKeyDown(sf::Keyboard::E))y++;

#endif
#ifdef ENGINE_USE_GLFW


    //if (isKeyDown(GLFW_KEY_W))debug("input si");
    //else debug("input no");

    if(isKeyDown(GLFW_KEY_W))z++;
    if(isKeyDown(GLFW_KEY_A))x--;
    if(isKeyDown(GLFW_KEY_S))z--;
    if(isKeyDown(GLFW_KEY_D))x++;
    if(isKeyDown(GLFW_KEY_Q))y--;
    if(isKeyDown(GLFW_KEY_E))y++;

#endif

    vec3 dir=vec3(x,y,z);
    //if(dir!=vec3(0))dir=normalize(dir);
    return dir;
}


vec3 glfwInputManager::getPositionMovementRotated(float ang)
{
    ang=glm::radians(-ang+90);

    vec3 p(0);
    vec3 pos=getPositionMovement();
    vec3 rot=vec3(cos(ang),0,sin(ang));
    p+=pos.z*rot;
    rot=vec3(cos(ang+1.57),0,sin(ang+1.57));
    p+=pos.x*rot;
    p.y=pos.y;
    return p;
}



map<GLFWwindow*,glfwInputManager*> glfwInputManager::windowsMap=map<GLFWwindow*,glfwInputManager*>();



void glfwInputManager::setup(glfwWindow* w){
	
	engine_window=w;
	window=w->window;

	
	windowsMap[window]=this;
	glfwSetKeyCallback(window, _keyCallback);
	glfwSetCursorPosCallback(window,_mouseMoveCallback);
    glfwSetMouseButtonCallback(window,_mouseButtonCallback);
    glfwSetScrollCallback(window,_mouseScrollCallback);
    
	Debug("\n\n\nnew window in glfwInputManager\n\n\n");
} 
void glfwInputManager::update(){
	input_pre=input;
	input.mouse.wheelDelta=0;
}


 
void glfwInputManager::_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){

	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    glfwInputManager *w=windowsMap.find(window)!=windowsMap.end()?windowsMap[window]:0;

    if(!w)return ;


	switch(action){
		case 1:	//ignores 2: key repeat
		case 0:
			{
				Event e;
				e.setType(Event::input);
				InputEvent i(InputEvent::keyboard);
				/*KeyboardEvent k;
				k.key=key;
				k.action=action;
				i.setData(k);*/
				// i.setData(i);
				//e.setChildEvent(i);
				w->send(e);
			} 
			break;	
	} 
	
	w->input.key[key]=action;
}

void glfwInputManager::_mouseMoveCallback(GLFWwindow *window,double x,double y)
{
	
	glfwInputManager *w=windowsMap.find(window)!=windowsMap.end()?windowsMap[window]:0;

    if(!w){Debug("no istance");return ;}

	w->input.mouse.x = x;
    w->input.mouse.y = y;
}

void glfwInputManager::_mouseButtonCallback(GLFWwindow *window,int button,int action,int mods)
{
	glfwInputManager *w=windowsMap.find(window)!=windowsMap.end()?windowsMap[window]:0;

    if(!w){Debug("no istance");return ;}

    switch(button) {
            case GLFW_MOUSE_BUTTON_LEFT :
                w->input.mouse.button[MouseState::LEFT_BUTTON] = action;
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                w->input.mouse.button[MouseState::RIGHT_BUTTON] = action;
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                w->input.mouse.button[MouseState::MIDDLE_BUTTON] = action;
                break;
            default:break;
	}
}

void glfwInputManager::_mouseScrollCallback(GLFWwindow *window,double non_so,double delta)
{
	glfwInputManager *w=windowsMap.find(window)!=windowsMap.end()?windowsMap[window]:0;
    if(!w){Debug("no istance");return ;}
    
    w->input.mouse.wheelDelta=delta;
}
