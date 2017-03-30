#pragma once
#include "Flag.h"
#include "enum_flags.h"
#include "Hash.h"
#include <type_traits>
#include <Container.h>

namespace Engine {

struct Event {
protected:
    bool isset;
    VariantData data;
    int _type;

    virtual Event* clone() const {
        Event* i=new Event();
        i->isset=isset;
        i->data=data;
        // i->derived=derived;
        return i;
    }

public:

    typedef enum {
        none  = 0,
        input = -1,
        logic = -2,
        other = 1
    }Type;

    enum class Target {
        // none=0,
        entity=1,
        scene=2,
        byFlag=4
    };//Target;

    Event(unsigned int type=none):_type(type) {}


    virtual ~Event() {
    }
    virtual	inline int getType() const {
        return _type;
    }
    //        template<class T>//, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
    //        void setType(T type){
    //            _type=static_cast<int>(type);
    //        }
    void setType(int type){
        _type=type;
    }
    void setType(StringHash type){
        _type=type.getHash();
    }



    template <typename T>
    void setData(T value) {
        data.setData(value);
    }
    template <typename T>
    T getData()  {
        return data.getData<T>();
    }

//    void setChildEvent(Event& f) {
//        child=f.clone();
//    }
//    Event* getChildEvent() {
//        return child;
//    }


};


//    template <typename T>
//    struct EventData;




//    struct Event {
//    protected:
//        bool isset;
//        bool derived;
//        Event* child;
//        int _type;

//        virtual Event* clone() const {
//            Event* i=new Event();
//            i->isset=isset;
//            i->derived=derived;
//            return i;
//        }

//    public:

//        typedef enum {
//            none  = 0,
//            input = -1,
//            logic = -2,
//            other = 1
//        }Type;

//        enum class Target {
//           // none=0,
//            entity=1,
//            scene=2,
//            byFlag=4
//        };//Target;

//        Event(unsigned int type=none):derived(0),child(0),_type(type) {}


//        virtual ~Event() {
//            delete child;
//        }
//        virtual	inline int getType() const {
//            return _type;
//        }
////        template<class T>//, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
////        void setType(T type){
////            _type=static_cast<int>(type);
////        }
//        void setType(int type){
//            _type=type;
//        }
//        void setType(StringHash type){
//            _type=type.getHash();
//        }



//        template <typename T>
//        void setData(T value) {
//            if(derived)
//                ((EventData<T>*)this)->setData(value);
//            else{
//                *this=EventData<T>();
//                setData<T>(value);
//            }
//        }
//        template <typename T>
//        T getData()  {
//            if(derived)
//                return ((EventData<T>*)this)->getData();
//            return T();
//        }

//        void setChildEvent(Event& f) {
//            child=f.clone();
//        }
//        Event* getChildEvent() {
//            return child;
//        }


//    };

//    //inline Event::Target  operator    &   (Event::Target x,Event::Target y)      {   return static_cast<Event::Target>   (static_cast<int>(x) & static_cast<int>(y));    };
//     ENUM_FLAGS(Event::Target)

//    template <typename T>
//    struct EventData : Event {
//    protected:
//        T data;
//        EventData(const EventData<T> *f) {
//            data=f->data;
//            isset=f->isset;
//            derived=1;
//        }
//        virtual Event* clone() const {
//            Event* i=new EventData<T>(this);
//            return i;
//        }
//    public:
//        EventData() {
//            derived=1;
//            isset=0;
//        }

//        void setData(T value) {
//            data=value;
//            isset=1;
//        }
//        T getData() const {
//            if(isset)return data;
//            return T();
//        }
//    };

}
