#pragma once
#include "Context.h"
#include <GLFW\glfw3.h>

struct GLFWwindow;

namespace Engine {

	class GlfwInputManager;
    class InputManager;

	class GlfwWindow : public Context
	{
		GLFWwindow *m_Window;
        GlfwInputManager *m_Input=0;
        bool m_isVisible;
	public:
		GlfwWindow(const char *title, unsigned int width, unsigned int height);
		~GlfwWindow();

		bool Init();
		bool Destroy();

		void Close();
        bool isClosed() const;
        bool isVisible() const;

        void Update();
		void ClearBuffer();
		void setTitle(const char *newTitle);
		void Resize(int x, int y);
        InputManager& getInputManager() const;
	private:
        static void window_size_callback(GLFWwindow* window, int width, int height);
        static void window_iconify_callback(GLFWwindow* window, int iconified);
		friend class GlfwInputManager;
	};



}
