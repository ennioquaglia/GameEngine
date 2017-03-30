#include "GlfwInputManager.h"
#include "GlfwWindow.h"
#include <glm/glm.hpp>

#include "Debug.h"


namespace Engine {


	
	GlfwInputManager::GlfwInputManager() :window(0), engine_window(0)
	{
	}

	GlfwInputManager::GlfwInputManager(GlfwWindow *w) : engine_window(w) {
		setup(engine_window);
	}

	GlfwInputManager::~GlfwInputManager()
	{
	}

	


	bool GlfwInputManager::isKeyDown(int key)const
	{
		return input.key[key];
	}
	bool GlfwInputManager::isKeyPressed(int key)const
	{
		return input.key[key] && !input_pre.key[key];
	}
	bool GlfwInputManager::isKeyReleased(int key)const
	{
		return !input.key[key] && input_pre.key[key];
	}


    glm::vec2 GlfwInputManager::getMousePos()const
	{
		return glm::vec2(input.mouse.x, input.mouse.y);
	}
    glm::vec2 GlfwInputManager::getMousePositionDelta()const
	{
		return glm::vec2(input_pre.mouse.x, input_pre.mouse.y) - glm::vec2(input.mouse.x, input.mouse.y);
	}
    glm::vec2 GlfwInputManager::getMousePositionDeltaAndCenter(float centerZone)
	{
		
		glm::vec2 size = glm::vec2(engine_window->getWidth(), engine_window->getHeight()) / 2.0f;
		glm::vec2 pos = getMousePos();

		float centro = centerZone;
		if (pos.x>(size.x + centro) || pos.x<(size.x - centro) || pos.y>(size.y + centro) || pos.y<(size.y - centro)) {
			setMousePos(glm::vec2(-1));
			float deltaX = (float)pos.x - (size.x);
			float deltaY = (float)pos.y - (size.y);
			return glm::vec2(deltaX, deltaY);
		}
		return glm::vec2(0);
	}



	void GlfwInputManager::setMousePos(glm::vec2 pos)//todo
	{
		//engine_window->setMousePos(pos);
		_mouseMoveCallback(window, pos.x, pos.y);
	}


    bool GlfwInputManager::isMouseButtonDown(int button)const
	{
		return input.mouse.button[button];
	}
    bool GlfwInputManager::isMouseButtonPressed(int button)const
	{
		return input.mouse.button[button] && !input_pre.mouse.button[button];
	}
    bool GlfwInputManager::isMouseButtonReleased(int button)const
    {
		return !input.mouse.button[button] && input_pre.mouse.button[button];
	}
    int GlfwInputManager::getDeltaMouseScroll()const
	{
		return input.mouse.wheelDelta;
	}
    glm::vec3 GlfwInputManager::getPositionMovement()const
	{
		float x = 0, y = 0, z = 0;
        //float xrotrad = 0;

        if (isKeyDown(ENGINE_KEY_W))z++;
        if (isKeyDown(ENGINE_KEY_A))x--;
        if (isKeyDown(ENGINE_KEY_S))z--;
        if (isKeyDown(ENGINE_KEY_D))x++;
        if (isKeyDown(ENGINE_KEY_Q))y--;
        if (isKeyDown(ENGINE_KEY_E))y++;

		glm::vec3 dir = glm::vec3(x, y, z);
        if(dir!=glm::vec3(0))dir=glm::normalize(dir);
		return dir;
	}


    glm::vec3 GlfwInputManager::getPositionMovementRotated(float ang)const
	{
		ang = glm::radians(-ang + 90);

		glm::vec3 p(0);
		glm::vec3 pos = getPositionMovement();
		glm::vec3 rot = glm::vec3(cos(ang), 0, sin(ang));
		p += pos.z*rot;
		rot = glm::vec3(cos(ang + 1.57), 0, sin(ang + 1.57));
		p += pos.x*rot;
		p.y = pos.y;
		return p;
	}



	std::map<GLFWwindow*, GlfwInputManager*> GlfwInputManager::windowsMap = std::map<GLFWwindow*, GlfwInputManager*>();



	


	void GlfwInputManager::setup(GlfwWindow* w) {

		engine_window = w;
		window = w->m_Window;


		windowsMap[window] = this;
		glfwSetKeyCallback(window, _keyCallback);
		glfwSetCursorPosCallback(window, _mouseMoveCallback);
		glfwSetMouseButtonCallback(window, _mouseButtonCallback);
		glfwSetScrollCallback(window, _mouseScrollCallback);

		QENGINE_INFO("new window in GlfwInputManager");
		//Debug("\n\n\nnew window in GlfwInputManager\n\n\n");
	}


	void GlfwInputManager::Update() {
        //QENGINE_INFO("GlfwInputManager::Update");
        //QENGINE_INFO("mouse : "+input.mouse.x+" "+input.mouse.y);
        //if(input.mouse.x!=input_pre.mouse.x)QENGINE_INFO("mouse move");
        input_pre = input;
		input.mouse.wheelDelta = 0;
	}



	void GlfwInputManager::_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

		GlfwInputManager *w = windowsMap.find(window) != windowsMap.end() ? windowsMap[window] : 0;
		if (!w) return;
		
		
        //QENGINE_INFO("key input: "+key+","+action+" (where A="+ENGINE_KEY_A);

        if (key == ENGINE_KEY_ESCAPE && action == GLFW_PRESS)w->engine_window->Close();

		w->input.key[key] = action;

		/*
		switch (action) {
		case 1:	//ignores 2: key repeat
		case 0:
		{
			Event e;
			e.setType(Event::input);
			InputEvent i(InputEvent::keyboard);

			// i.setData(i);
			//e.setChildEvent(i);
			w->send(e);
		}
		break;
		}

		

		*/
	}

	void GlfwInputManager::_mouseMoveCallback(GLFWwindow *window, double x, double y)
	{

        //QENGINE_INFO("mouse : "+x+" "+y);
		GlfwInputManager *w = windowsMap.find(window) != windowsMap.end() ? windowsMap[window] : 0;

		//if (!w) { Debug("no istance"); return; }

		w->input.mouse.x = x;
		w->input.mouse.y = y;
	}

	void GlfwInputManager::_mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
	{
		GlfwInputManager *w = windowsMap.find(window) != windowsMap.end() ? windowsMap[window] : 0;

		//if (!w) { Debug("no istance"); return; }

		switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
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

	void GlfwInputManager::_mouseScrollCallback(GLFWwindow *window, double non_so, double delta)
	{
		GlfwInputManager *w = windowsMap.find(window) != windowsMap.end() ? windowsMap[window] : 0;
		//if (!w) { Debug("no istance"); return; }

		w->input.mouse.wheelDelta = delta;
	}


}
