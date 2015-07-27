#pragma once

#include <list>
#include <string.h>
#include "Debug.h"

#ifdef ENGINE_USE_GLFW

#include "GlfwUtility.h"

#endif

namespace Engine
{

    template <typename T>
    struct EventData;

    struct Event {
    protected:
        bool isset;
        bool derived;
        Event* child;
		int _type;
		
        virtual Event* clone() const {
            Event* i=new Event();
            i->isset=isset;
            i->derived=derived;
            return i;
        }

    public:
    	typedef enum {
            none=0,
            input=-1,
            logic=-2
        }Type; 
    	
        Event(int type=none):derived(0),child(0),_type(type) {}
        
        virtual ~Event() {
            delete child;
        }
   	    virtual	inline int getType() const {
            return _type;
        }
        virtual void setType(int type){
    		_type=type;
    	}
		
		
        template <typename T>
        void setData(T value) {
            if(derived)
                ((EventData<T>*)this)->setData(value);
            else{
				*this=EventData<T>();
				setData<T>(value);
			}
        }
        template <typename T>
        T getData() const {
            if(derived)
                return ((EventData<T>*)this)->getData();
            return T();
        }

        void setChildEvent(Event& f) {
            child=f.clone();
        }
        Event* getChildEvent() {
            return child;
        }


    };



    template <typename T>
    struct EventData : Event {
    protected:
        T data;
        EventData(const EventData<T> *f) {
            data=f->data;
            isset=f->isset;
            derived=1;
        }
        virtual Event* clone() const {
            Event* i=new EventData<T>(this);
            return i;
        }
    public:
        EventData() {
            derived=1;
            isset=0;
        }

        void setData(T value) {
            data=value;
            isset=1;
        }
        T getData() const {
            if(isset)return data;
            return T();
        }
    };

    typedef std::list<Event> MessageQueue;

    /*
    class Event
    {
    protected:
        int type;
    public:

        Event(int type=EventType::none) {
            this->type=type;
        }

        virtual	int getType()  {
            return type;
        }
        virtual void setType(int type){
    		this->type=type;
    	}
    };




    class Event
    {
    protected:
        int type;
        char* data;
        int dataSize;

    public:
    	typedef enum {
            none=0,
            input=-1,
            logic=-2
        } Type;

        Event(int type=none) {
            this->type=type;
            data=0;
            dataSize=0;
        }
        ~Event() {
            if(data) {
                    delete [] data;
                }
        }
        Event& operator=(const Event& e) {

            type=e.type;
            if(e.data) {
                    dataSize=e.dataSize;
                    memcpy(data,e.data,dataSize);
                }

            return *this;
        }

        Event(const Event& e) {
            type=none;
            data=0;
            dataSize=0;

            if(this != &e) {
                    type=e.type;
                    if(e.data&&e.dataSize) {
                            dataSize=e.dataSize;
                            data=new char[dataSize];
                            memcpy(data,e.data,dataSize);
                        }
                }
        }
        template <typename T>
        void setData(T pData) {
            dataSize=sizeof(T);
            data=new char[dataSize];
            *(T*)data=pData;
        }
        template <typename T>
    	T getData() {
            if(!data)return T();
            return *(T*)(data);
        }


        virtual	inline int getType() const {
            return type;
        }
        virtual void setType(int type){
    		this->type=type;
    	}

    };
    */


    /*
    class Event
    {
        char* data;
        int dataSize;
    	//ActorComponent *_sender;
    public:
        int type;

        Event() {
            type=0;
            data=0;
            dataSize=0;
            _sender=0;
        }
        ~Event() {
            //	delete [] data;
            //free(data); 
        }
        Event(int pType,void *pData=0);

        template <typename T>
        void setData(T pData) {
            dataSize=sizeof(T);
            data=new char[dataSize];
            *(T*)data=pData;
        }
        template <typename T>
        T getData() {

            return *(T*)(data);

        }
        virtual	int getType(){return type;}

        ActorComponent* getSender() {
            return _sender;
        }

    };*/




}
