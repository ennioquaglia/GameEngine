#pragma once


#include <bitset>
#include <cassert>

#include <memory>
#include <vector>
#include <unordered_map>
#include "Debug.h"

namespace Engine{

template<typename T,int Size=128>
class VectorContainer{
    std::vector<T> data;
    int first_free=-1;
    std::vector<bool> flag;
public:
    VectorContainer(){
        data.reserve(Size);
    }

    class iterator{
        friend class VectorContainer<T>;
        VectorContainer *c=0;
        int index=0;
        iterator(VectorContainer *c,int index):c(c),index(index){}
    public:
        void operator++(){ //++it
             if(index==-1)return;
             while( ++index<c->data.size() && !c->flag[index] );
             if(index>=c->data.size())index=-1;
        }
        void operator++(int){//it++
            ++(*this);
        }

        bool operator==( iterator it) const{return c==it.c && index==it.index;}
        bool operator!=( iterator it) const{return !(*this==it);}
        T& operator*()      {return c->data[index];}   // *end()  causes the program to crash ( rightly )
        T  operator*()const {return c->data[index];}
    };

    iterator begin(){return iterator(this,0);}
    iterator& end(){static iterator i(this,-1);return i;}

    T& insert(T value){

        if(first_free==-1){
            data.push_back(value);
            flag.emplace_back(1);
            return *data.rbegin();
        }
        int i=first_free;
        data[i]=value;

        while(first_free>=flag.size() && !flag[++first_free]);
        if(first_free>=flag.size())first_free=-1;
        return data[i];

    }
    T& insert(){
        if(first_free==-1){
            data.push_back(T());
            flag.emplace_back(1);
            return *data.rbegin();
        }
        int i=first_free;
        data[i]=T();

        while(first_free>=flag.size() && !flag[++first_free]);
        if(first_free>=flag.size())first_free=-1;
        return data[i];

    }

    void erase(T& t){
        erase(find(t));
    }
    iterator find(T& t){
        for(iterator it=begin();it!=end();++it)
            if(*it==t)return it;
        return end();
    }

    void erase(iterator it){
        if(it.index!=-1 && it.c==this && it.index<flag.size() && flag[it.index]){
            flag[it.index]=0;
            if(first_free==-1)first_free=it.index;
            else first_free=std::min(it.index,first_free);
        }
    }

    [[ deprecated("use insert")]] T& push_back(T value){insert(value);}
};




class object_t {
  public:
    template <typename T>
    object_t(T x) : self_(new model<T>(move(x)))
    { }

    object_t(const object_t& x) : self_(x.self_->copy_())
    { }
    object_t(object_t&&) noexcept = default;

    object_t& operator=(const object_t& x)
    { object_t tmp(x); *this = std::move(tmp); return *this; }
    object_t& operator=(object_t&&) noexcept = default;

    friend void draw(const object_t& x, std::ostream& out, size_t position)
    { x.self_->draw_(out, position); }

  private:
    struct concept_t {
        virtual ~concept_t() = default;
        virtual concept_t* copy_() const = 0;
        virtual void draw_(std::ostream&, size_t) const = 0;
    };
    template <typename T>
    struct model : concept_t {
        model(T x) : data_(move(x)) { }
        concept_t* copy_() const { return new model(*this); }
        void draw_(std::ostream& out, size_t position) const
        { draw(data_, out, position); }

        T data_;
    };

   std::unique_ptr<const concept_t> self_;
};

class VariantData{
    char* data=0;
    //size_t size=0;
    VariantData(const VariantData &a){

    }

public:
    VariantData(){}
    ~VariantData(){
        //delete[] data;
        free(data);
    }

    template<typename T>
    void setData(T t){
        data=(char*)malloc(sizeof(T));//new char(sizeof(T));
        memcpy(data,&t,sizeof(T));
        //size=sizeof(T);
    }


    template<typename T>
    T getData(){
        return *(T*)data;
    }
};



//class MultiComponentsContainer{
//    std::unordered_map<ComponentType,object_t> containers;

//    template<class T>
//    T* create(){
//        auto it=containers.find(T::getTypeStatic());
//        if(it!=containers.end()){
//            return ((VectorContainer<T>)*it).insert();
//        }
//    }

//    template<class T>
//    VectorContainer<T>& getVector(){
//        return ((VectorContainer<T>)containers[T::getTypeStatic()]);
//    }

//};




}
