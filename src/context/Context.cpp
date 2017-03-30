#include "Context.h"
#include "InputManager.h"
#include "Event.h"
#include "Scene.h"
#include "PostProcessEffect.h"
#include "Object.h"

#include <string>

namespace Engine {


	Context::Context(const char * title, unsigned int width, unsigned int height)
	{
		m_Title = title;
		m_Width = width;
		m_Height = height;
        //m_Input = 0;
	}

	Context::~Context()
	{

	}

	const int Context::getWidth() const {
		return m_Width;
	}

	const int Context::getHeight() const {
		return m_Height;
	}

    const char * Context::getTitle() const {
		return m_Title;
	}

    void Context::handleEvent(Event &msg){
//        if(state){
//            state->handleEvent(msg);
//        }
        for(auto s: m_ObserverScene)s->handleEvent(msg);

    }

    void Context::attachObserver(Object *o){}
    void Context::detachObserver(Object *o){}
    void Context::attachObserver(Scene *s){
        m_ObserverScene.push_back(s);
    }
    void Context::detachObserver(Scene *s){}
    void Context::attachObserver(PostProcessor *p){}
    void Context::detachObserver(PostProcessor *p){}


//    InputManager& Context::getInputManager() const {
//        static DummyInputManager dummy;
//        return dummy;
//    }

}
