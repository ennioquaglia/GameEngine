#pragma once


#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/random.hpp>

#include <string>
 

#include <list>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>

#define ASSIMP_STATIC
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/anim.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <typeinfo>

#include "debug.h"
#include "GlobalsManager.h"

#include <stdint.h>
#include <stdio.h>

#include "Hash.h"

#define H1(s,i,x)   (x*65599u+(glm::uint8_t)s[(i)<strlen(s)?strlen(s)-1-(i):strlen(s)])
#define H4(s,i,x)   H1(s,i,H1(s,i+1,H1(s,i+2,H1(s,i+3,x))))
#define H16(s,i,x)  H4(s,i,H4(s,i+4,H4(s,i+8,H4(s,i+12,x))))
#define H64(s,i,x)  H16(s,i,H16(s,i+16,H16(s,i+32,H16(s,i+48,x))))
#define H256(s,i,x) H64(s,i,H64(s,i+64,H64(s,i+128,H64(s,i+192,x))))

#define HASH(s)    ((glm::uint32_t)(H256(s,0,0)^(H256(s,0,0)>>16)))


#define glDebug0() {\
					int e=glGetError();\
					if(e)Error("OpenGL ERROR "+e+" in line number "+__LINE__+" in file "+__FILE__);\
		}
		
#define glDebug1(s) {\
					int e=glGetError();\
					if(e)Error("OpenGL ERROR "+e+" in line number "+__LINE__+" in file "+__FILE__+"\n\tMessage:"+s);\
		}					

#define GET_MACROd(_0,_1,NAMEd,...) NAMEd
//#define glDebug(...) GET_MACROd(_0,##__VA_ARGS__,glDebug1, glDebug0)(__VA_ARGS__)	
#define glDebug(...)

class btQuaternion;
class btVector3;
namespace Engine
{
    using namespace glm;
    using namespace std;
    
    
    class EString;
    
    extern void consolePause();

    extern unsigned int getHash(string str);
    extern string getStringFromHash(unsigned int hash);
    extern void clearHashTable();
    extern unsigned int getUnivocalId();
    
    extern vec3 getPosFromMatrix(mat4 matrix);
    extern std::string loadTextFile(std::string path);

    extern btQuaternion toBullet(glm::quat quaternion);
	extern glm::quat fromBullet(btQuaternion &q);
	extern btVector3 toBullet(glm::vec3 q);
	extern glm::vec3 fromBullet(btVector3 &q);

    
     
    
    class Rotation
    {
    public:
        glm::quat q;
        vec3 dir;
        vec3 euler;

        Rotation() {
            q=quat();
            dir=vec3(0);
            euler=vec3(0);
        }
 
        void setNormalDirection(vec3 Dir);
        void setEulerAngles(vec3 angle) {//yaw(y) -pitch(x)-roll(z)//pitch - yaw - roll
            euler=angle;
           //mat4 rot=glm::rotate(mat4(1),angle.x,vec3(1,0,0))*glm::rotate(mat4(1),angle.y,vec3(0,1,0))*glm::rotate(mat4(1),angle.z,vec3(0,0,1));
            //q=glm::quat_cast(rot);
			q=quat(euler);
            q=normalize(q);
            
            vec4 d=glm::mat4_cast(q)*vec4(0,-1,0,0);
            dir=normalize(vec3(d)/d.w);
            // glm::vec3 t;
            // t.x=glm::cos(glm::radians(euler.x));
            // t.z=glm::sin(glm::radians(euler.x));
            // t.y=glm::sin(glm::radians(euler.z));
            // t.x*=glm::cos(glm::radians(euler.z))+0.001;
            // t.z*=glm::cos(glm::radians(euler.z))+0.001;
            // dir=t=glm::normalize(t);
        }
        void setQuaternion(quat Q) {
            q=normalize(Q);
            dir=q*vec3(1,0,0);
            vec2 xz(dir.x,dir.z);
            float yaw=atan2(xz.x,xz.y);
            float pitch= asin(dir.y);
            euler=vec3(glm::radians(glm::pitch(q)),glm::radians(glm::yaw(q)),glm::radians(glm::roll(q)));
            setEulerAngles(euler);
            //euler=vec3(yaw,0,pitch);
            
            vec4 d=glm::mat4_cast(q)*vec4(0,-1,0,0);
            dir=normalize(vec3(d)/d.w);
        }
        
        mat4 getMatrix(){
        	q=normalize(q);
			return glm::mat4_cast(q);
		}
		quat getQuaternion(){return q;}
		vec3 getEulerAngles(){return euler;}
		vec3 getDir(){return dir;}

    };

}

