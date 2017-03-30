#pragma once
#include "Pathname.h"
#include "Debug.h"
#include <vector>
#include <string>
#include <map>
#include <tuple>
#include <unordered_map>
#include <algorithm>

#include "Debug.h"


namespace Engine{





/*
Texture::load(asdas);
Shader::compile(shader);

*/
/*
template<class T,typename ...Args>
struct _Resource  {

    static T & load(Args... args) {
        static T t=T(args...);
        return t;
        //return ResourceManager<T>::istance().load(filename.data);
    }
    static T & load(unsigned int id) {
        //return ResourceManager<T>::istance().load(id);
    }
};


template<class T,void (T::*CompileMethod)(T&)>
class _ResourceManager: public Singleton< _ResourceManager<T,CompileMethod> > {
    friend class _Resource<T,CompileMethod>;

    std::vector<T> res;
    T nullElement;

    void compile(T& t) {
        std::vector<T>::iterator it=res.begin();
        bool ok=0;
        while(it!=res.end()&&!ok){
            if(res[i]==t){
                t=res[i];
                ok=1;
            }
        }
        if(!ok){
            CompileMethod(t);
            res.pop_back(t);
        }
    }
    T & get(unsigned int id) {
        if(id < res.size())return res[id];
        return nullElement;
    }
};

template<class T,void (T::*CompileMethod)(T&)>
struct _Resource<T,CompileMethod>{
    static void compile(T& t) {
         ResourceManager<T,CompileMethod>::istance().compile(t);
        //return ResourceManager<T>::istance().load(filename.data);
    }
};

class Texture1 : public Resource1<Texture1, PathName>{

};*/

    template<class T,typename ...Args> class Resource;

    template<class T>
    class Singleton {
    protected:
        Singleton() {}
        static T & istance() {
            static T t;
            return t;
        }
    };
#if false
    template<class T,typename ...Args>
    class ResourceManager: public Singleton< ResourceManager<T,Args...> > {
        friend class Resource<T,Args...>;
        std::vector<T*> res;
        std::vector<unsigned> instance;

        std::vector<std::tuple<Args...>> map_args;
        T nullElement;



        T* load(Args... args) {
            //return nullElement;
            unsigned int i=0;
            std::tuple<Args...> t(args...);
            for(;i<map_args.size();++i){
                if(map_args[i]==t)break;
            }
            if(i<map_args.size()){
                ++instance[i];
                return res[i];
            } else {
                unsigned int id = res.size();
                map_args.push_back(t);
                res.push_back(new T(args...));
                instance.push_back(1);
                return res[id];
            }
        }

        void remove(T* t) {
            for(unsigned i=0;i<res.size();i++)
                if(res[i]==t){
                    if(!(--instance[i])){
                        res.erase(res.begin()+i);
                        instance.erase(instance.begin()+i);
                        map_args.erase(map_args.begin()+i);
                        delete t;
                        QENGINE_INFO("  --Resource deleted-- ");
                    }
                    return;
                }

        }


        T & get(unsigned int id) {
            if(id < res.size())return res[id];
            //return load("null");
            return nullElement;
        }
    };
#endif

    template<class T,typename ...Args>
    class ResourceManager: public Singleton< ResourceManager<T,Args...> > {
        friend class Resource<T,Args...>;
        //std::vector<T> res;
        T res[100];//todo
        std::vector<unsigned> instance;
        int num=0;

        std::vector<std::tuple<Args...>> map_args;
        T nullElement;




        T* load(Args... args) {
            //return nullElement;
            unsigned int i=0;
            std::tuple<Args...> t(args...);
            for(;i<map_args.size();++i){
                if(map_args[i]==t)break;
            }
            if(i<map_args.size()){
                ++instance[i];
                return &res[i];
            } else {
                unsigned int id = num++;
                map_args.push_back(t);
                //res.resize(num);//push_back(new T(args...));
                res[id]=T(args...);
                instance.push_back(1);
                return &res[id];
//                unsigned int id = res.size();
//                map_args.push_back(t);
//                res.push_back(new T(args...));
//                instance.push_back(1);
//                return res[id];
            }
        }

        void remove(T* t) {
            for(unsigned i=0;i<res.size();++i)
                if(&res[i]==t){
                    if(!(--instance[i])){
                        res.erase(res.begin()+i);
                        instance.erase(instance.begin()+i);
                        map_args.erase(map_args.begin()+i);
                        delete t;
                        QENGINE_INFO("  --Resource deleted-- ");
                    }
                    return;
                }

        }


        T & get(unsigned int id) {
            if(id < res.size())return res[id];
            //return load("null");
            return nullElement;
        }
    };

    template<class T,typename ...Args>
    struct Resource  {
        static T* load(Args... args) {
            //return new T(args...);
            return (ResourceManager<T,Args...>::istance().load(args...));
        }
        /*static T & load(Args... args) {
            return ResourceManager<T,Args...>::istance().load(args...);
        }*/
        static T* get(unsigned int id) {
            return (ResourceManager<T,Args...>::istance().get(id));
        }

        static void remove(T* t) {
            //return new T(args...);
            (ResourceManager<T,Args...>::istance().remove(t));
        }
    };


/*
    template<class T,typename CompileMethod>
    class ResourceManager<T,CompileMethod> : public Singleton< ResourceManager<T,CompileMethod> > {
        friend class Resource<T,CompileMethod>;

        std::vector<T> res;
        T nullElement;

        void compile(T& t) {
            bool ok=0;
//            std::vector<T>::iterator it=res.begin();

//            while(it!=res.end()&&!ok){
//                if(*it==t){
//                    t=*it;
//                    ok=1;
//                }
//                ++it;
//            }
            for(int i=0;i<res.size()&&!ok;i++){
                if(res[i]==t){
                    t=res[i];
                    ok=1;
                }
            }
            if(!ok){
                (t.CompileMethod)();
                res.push_back(t);
            }
        }
        T & get(unsigned int id) {
            if(id < res.size())return res[id];
            return nullElement;
        }
    };

    template<class T,typename CompileMethod>
    struct Resource<T,CompileMethod>{
        static void compile(T& t) {
             ResourceManager<T,CompileMethod>::istance().compile(t);
        }
        static T* get(unsigned int id) {
            return &(ResourceManager<T,CompileMethod>::istance().get(id));
        }
    };*/

}
