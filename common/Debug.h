//Debug.h
#pragma once
#include <glm/glm.hpp>
#include <string>

namespace Engine
{
    using namespace glm;
    using namespace std;

	class EString;
	
	extern void debug(EString str);// todo printf-like function
    extern void error(EString str);
    extern void warning(EString str);
    extern std::string stoi(int number);
    extern std::string stof(float number);
    
    class EString
    {

    public:
        EString() {}
        EString(const char* c) {
            data=string(c);
        }
        /*EString(const GLubyte* c) {
            data=string((char*)c);
        }*/
        EString(float f) {
            data=stof(f);
        }
        EString(double f) {
            data=stof(f);
        }
        EString(int i) {
            data=stoi(i);
        }
        EString(unsigned int i) {
            data=stoi(i);
        }
        EString(char c) {
            data+=c;
        }
        EString(string s) {
            data=s;
        }
        EString(vec2 v) {
            data="vec2("+stof(v.x)+","+stof(v.y)+")";
        }
        EString(vec3 v) {
            data="vec3("+stof(v.x)+","+stof(v.y)+","+stof(v.z)+")";
        }
        EString(vec4 v) {
            data=stof(v.x)+" "+stof(v.y)+" "+stof(v.z)+" "+stof(v.w);
        }
        EString(mat4 m) {
            data="mat4(";
			for(int i=0;i<16;i++)data+="["+stof(m[i/4][i%4])+"]";
			data+=")";
	   	}
        EString& operator+(const EString &str) {
            EString *result=new EString(string(data+str.data));
            return *result;
        }



        string data;
    };


}

#define Debug(a) Engine::debug(Engine::EString()+a)
#define Error(a) Engine::error(Engine::EString()+a)
#define Warning(a) Engine::warning(Engine::EString()+a)
