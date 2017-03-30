#include "physics.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Engine{

btQuaternion toBullet(glm::quat quaternion){
    return btQuaternion(quaternion.x,quaternion.y,quaternion.z,quaternion.w).normalized();
}


glm::quat fromBullet(btQuaternion q){
    return glm::quat(q.w(),q.x(),q.y(),q.z());
}

btVector3 toBullet(glm::vec3 q){
    return btVector3(q.x,q.y,q.z);
}

glm::vec3 fromBullet(btVector3 q){
    return glm::vec3(q.x(),q.y(),q.z());
}

std::string approximate_time(float seconds){
    if(seconds==0)return "0s";

    std::string SUFFIXEX[]={"s","ms","\x0e6s","ns"};// s - ms - µs - ns
    float time=seconds;
    std::string a;
    for(int i=0;i<4;i++){
        if(time>=1 || i==3){
            a=std::to_string(time);
            a+=SUFFIXEX[i];
            return a;
        }
        time=time*1000.0f;
    }
}



}
