#pragma once

#include <vector>

namespace Engine {

	class InputManager;
    class Event;
    class PostProcessor;
    class Object;
    class Scene;

	class Context
	{
        std::vector<Scene*> m_ObserverScene;
	protected:
		const char *m_Title;
		unsigned int m_Width, m_Height;
        //InputManager* m_Input=0;
	public:
        Context(const char *title="Engine!!!", unsigned int width=800, unsigned int height=600);
		virtual ~Context();

		virtual bool Init() = 0;
		virtual bool Destroy() = 0;
		virtual void Close() = 0;

        virtual void handleEvent(Event &msg);
        virtual void attachObserver(Object *o);
        virtual void detachObserver(Object *o);
        virtual void attachObserver(Scene *s);
        virtual void detachObserver(Scene *s);
        virtual void attachObserver(PostProcessor *p);
        virtual void detachObserver(PostProcessor *p);



        virtual bool isClosed() const = 0;
        virtual bool isVisible() const = 0;

		virtual void Update() = 0;
		virtual void ClearBuffer() = 0;

		const int getWidth() const;
		const int getHeight() const;
		virtual void Resize(int x, int y)=0;

		const char* getTitle() const;
		virtual void setTitle(const char *newTitle)=0;




        virtual InputManager& getInputManager() const = 0;
	};


}
