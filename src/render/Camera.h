#pragma once
#include "Geometry.h"
#include <glm/glm.hpp>

using namespace std;

namespace Engine
{

    class Camera
    {
        glm::mat4 matrix;
    public:
        enum mode {
            LookAt,
            PosDir,
            Rot ,
            TargetRot,
            Matrix
        };

        enum {OUTSIDE=0, INTERSECT=1, INSIDE=2};

        Camera();

        glm::vec3 Pos;
        glm::vec3 Target;
        glm::vec3 Up;
        glm::vec3 Dir;

        glm::mat4 projection;
        float rotX,rotY;
        unsigned char mode;

        glm::vec3 corners[8];
        Plane planes[6];
        void UpdatePlanes();
        //set
        //int inFrustum(GameObject *obj);
        glm::mat4 getMatrix();
        void setMatrix(glm::mat4 m);
        void MultMatrix() ;
        inline void setProjectionMatrix(glm::mat4 matrix){
            projection= matrix;
        }
        inline glm::mat4 getProjectionMatrix(){
            return projection;
            //float ratio=GlobalsManager::Istance()->get("window_width")/GlobalsManager::Istance()->get("window_height");
            //return glm::perspective(60.0f,ratio,0.1f,500.0f);
        }
        float distance;

        void setRotX(float degree);
        void setRotY(float degree);
        void setRotZ(float degree);
        float getRotX();
        float getRotY();
        float getRotZ();

        glm::vec3 getPos();
        void setDir(glm::vec3 d);
        glm::vec3 getDir();
        inline void setMode(unsigned char m) {
            mode=m;
        }
    };


}

