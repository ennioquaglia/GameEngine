#include "utility.h"
#include "../Physic/Physics.h"
#include <math.h>

namespace Engine{
	

	btQuaternion toBullet(glm::quat quaternion){
		return btQuaternion(quaternion.x,quaternion.y,quaternion.z,quaternion.w).normalized();
	}
	
	
	quat fromBullet(btQuaternion &q){
		return quat(q.w(),q.x(),q.y(),q.z());
	}
	
	btVector3 toBullet(vec3 q){
		return btVector3(q.x,q.y,q.z);
	}
	
	vec3 fromBullet(btVector3 &q){
		return vec3(q.x(),q.y(),q.z());
	}
	
	
}

void Engine::Rotation::setNormalDirection(vec3 Dir) {
        if(Dir==vec3(0,0,0)) {
                Rotation();
                return;
            }

        dir=normalize(vec3(rotate(mat4(1),-90.0f,vec3(0,1,0))*vec4(Dir,0)));
		vec3 up=vec3(1,0,0);
		
		//if(dir==up||dir==-up){Debug("##############si");dir.y+=0.001;dir.z+=0.001;dir=normalize(dir);}//todo
		//else Debug("############ dir="+dir);
			
        if(dir!=up&&dir!=-up) {
                vec3 axis=cross(up,dir);
                float RotationAngle = glm::acos(dot(up,dir) / (glm::length(up)*glm::length(dir)));
                btQuaternion bq(toBullet(axis),RotationAngle);
                q=fromBullet(bq);//quat(RotationAngle,axis);
                q=normalize(q);
                setQuaternion(q);
				//euler=vec3(glm::yaw(q),glm::roll(q),glm::pitch(q));
                //euler=vec3(0,glm::radians(glm::yaw(q)-270),0);//glm::degrees(glm::roll(q)));
                //setEulerAngles(euler);
    //             vec3 up=vec3(0,1,0);
    //             vec3 h=up+dir;
    //             
    //             q.w=glm::dot(up,h);
    //             q.x = up.y*h.z - up.z*h.y;     
				// q.y = up.z*h.x - up.x*h.z;     
				// q.z = up.x*h.y - up.y*h.x; 
				// q=normalize(q);

            }
        else {
                q=quat();
                euler=vec3(0);
            }
        //vec2 xz(dir.x,dir.z);
        //float yaw=atan2(xz.x,xz.y);
        //float pitch= asin(dir.y);
        //q=quat(vec3(pitch,yaw+1.57,0));
        //q=normalize(q);
        //euler=vec3(yaw+1.57,0,pitch);
    }
