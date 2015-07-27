#pragma once

#include "utility.h"
#include "Geometry.h"

using namespace glm;
using namespace std;

namespace Engine
{

    class Camera
    {
        mat4 matrix;
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
		
		vec3 corners[8];
		Plane planes[6];
		void UpdatePlanes();
        //set
        //int inFrustum(GameObject *obj);
        glm::mat4 getMatrix();
        void setMatrix(mat4 m);
        inline void MultMatrix() {
            glMultMatrixf(&getMatrix()[0][0]);
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
			
		vec3 getPos();	
        void setDir(glm::vec3 d);
        vec3 getDir();
        inline void setMode(unsigned char m) {
            mode=m;
        }
    };


}