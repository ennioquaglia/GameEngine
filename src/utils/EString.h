#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Engine{
class PathName;

class EString
{


public:
    EString() {}
    EString(const char* c) {
        data = std::string(c);
    }
    /*EString(const GLubyte* c) {
    data=string((char*)c);
    }*/
    EString(float f) {
        data = std::to_string(f);
    }
    EString(double f) {
        data = std::to_string(f);
    }
    EString(int i) {
        data = std::to_string(i);
    }
    EString(unsigned int i) {
        data = std::to_string(i);
    }
    EString(char c) {
        data += c;
    }
    EString(std::string s) {
        data = s;
    }
    EString(PathName s);
    EString(glm::vec2 v) {
        data = "vec2(" + std::to_string(v.x) + "," + std::to_string(v.y) + ")";
    }
    EString(glm::vec3 v) {
        data = "vec3(" + std::to_string(v.x) + "," + std::to_string(v.y) + "," + std::to_string(v.z) + ")";
    }
    EString(glm::vec4 v) {
        data = "vec4(" +std::to_string(v.x) + "," + std::to_string(v.y) + "," + std::to_string(v.z) + "," + std::to_string(v.w)+ ")";
    }
    EString(glm::quat v) {
        data = "quaternion(" +std::to_string(v.x) + "," + std::to_string(v.y) + "," + std::to_string(v.z) + "," + std::to_string(v.w)+ ")";
    }
    EString(glm::mat4 m) {
        data = "mat4(";
        for (int i = 0; i<16; i++)data += "[" + std::to_string(m[i / 4][i % 4]) + "]";
        data += ")";
    }
    EString(glm::mat2 m) {
        data = "mat2(";
        for (int i = 0; i<4; i++)data += std::to_string(m[i / 2][i % 2]) + ",";
        data += ")";
    }
    EString& operator+(const EString &str) {
        EString *result = new EString(std::string(toStd() + str.toStd()));
        return *result;
    }

    static void replaceAll(std::string& str, const std::string& from, const std::string& to) {
        if(from.empty())
            return;
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }

    std::string toStd() const{
        return data;
    }

    std::string data;

};

extern std::string trim(std::string s);



#define TO_STRING(a) #a
#define SERIALIZE_VAR(a) SERIALIZE_VAR_STR(a) //(std::string(#a)+"="+Engine::EString(a).data+" ")
#define SERIALIZE_VAR_STR(a) (std::string(#a)+"=\""+Engine::EString(a).toStd()+"\" ")
#define SERIALIZE_VAR_PATH(a) (std::string(#a)+"=\""+a.original+"\" ")
#define SERIALIZE_VAR_NAME_PATH(name,a) (std::string(#name)+"=\""+a.original+"\" ")

}
