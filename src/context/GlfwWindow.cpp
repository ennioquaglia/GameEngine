#include "GlfwWindow.h"
#include "GlfwInputManager.h"
#include "PlatformRenderer.h"
#include "Debug.h"
#include "Hash.h"

#include "Event.h"
#include <GLFW\glfw3.h>

namespace Engine {

    void GlfwWindow::window_size_callback(GLFWwindow* window, int width, int height)
    {
        //QENGINE_INFO("new size : "+width+"-"+height);
        GlfwWindow* w=(GlfwWindow*)glfwGetWindowUserPointer(window);
        if(!w)return;


        w->m_Width=width;
        w->m_Height=height;

        //EventData<glm::vec2>
        Event e;
        e.setType(Hash("windowResize"));
        e.setData(glm::vec2(width, height));

        w->handleEvent(e);
    }

    void GlfwWindow::window_iconify_callback(GLFWwindow* window, int iconified)
    {
        GlfwWindow* w=(GlfwWindow*)glfwGetWindowUserPointer(window);
        if(!w)return;


        QENGINE_INFO("WindowVisible = "+iconified);
        w->m_isVisible=!iconified;

        //EventData<bool>
        Event e;
        e.setType(Hash("WindowVisible"));
        e.setData(iconified);

        w->handleEvent(e);
    }

    GlfwWindow::GlfwWindow(const char * title, unsigned int width, unsigned int height):Context(title,width,height), m_isVisible(1)
	{

	}

	GlfwWindow::~GlfwWindow()
	{

        Close();
        Destroy();
		glfwTerminate();
	}

	bool GlfwWindow::Init()
	{
		/* Initialize the library */
		if (!glfwInit())
			return 0;
		/* Create a windowed mode window and its OpenGL context */
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwSwapInterval(1);
		m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, NULL, NULL);
		if (!m_Window)
		{
			glfwTerminate();
			return 0;
		}

		/* Make the window's context current */
		glfwMakeContextCurrent(m_Window);

        glfwSetWindowUserPointer(m_Window,this);
        glfwSetWindowSizeCallback(m_Window,window_size_callback);
        glfwSetWindowIconifyCallback(m_Window,window_iconify_callback);


        PlatformRenderer::Init();

		m_Input = new GlfwInputManager(this);
		return true;
	}

	bool GlfwWindow::Destroy()
	{
		return true;
	}

	void GlfwWindow::Close()
	{
		glfwSetWindowShouldClose(m_Window, GL_TRUE);
	}

    bool GlfwWindow::isClosed() const
	{
		return glfwWindowShouldClose(m_Window) != 0;
	}

    bool GlfwWindow::isVisible() const
    {
        return m_isVisible;
    }


	void GlfwWindow::Update()
	{

        //QENGINE_INFO("GlfwWindow::Update");
		m_Input->Update();
		/* Swap front and back buffers */
		glfwSwapBuffers(m_Window);

		/* Poll for and process events */
		glfwPollEvents();
	
		
	}

	void GlfwWindow::ClearBuffer()
	{
		glClearColor(0, 0, 0.5, 1);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 

	}
	void GlfwWindow::setTitle(const char * newTitle)
	{
		m_Title = newTitle;
		glfwSetWindowTitle(m_Window, m_Title);
	}
	void GlfwWindow::Resize(int x, int y)
	{
	}

    InputManager& GlfwWindow::getInputManager() const {
        return *m_Input;
    }
}
