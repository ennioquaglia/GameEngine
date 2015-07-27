#include "Camera.h"

using namespace Engine;

Camera::Camera()
{
    Up=vec3(0,1,0);
    Dir=Target=vec3(0,0,-1);
    Pos=vec3(0);
    rotX=rotY=0;
    mode=LookAt;
    distance=10;
    
    float ratio=GlobalsManager::Istance()->get("window_width")/GlobalsManager::Istance()->get("window_height");
    projection=glm::perspective(60.0f,ratio,0.1f,500.0f);
    
}

vec3 Camera::getPos(){
	return vec3(inverse(getMatrix())[3]);
	
}

glm::mat4 Camera::getMatrix()
{
    switch(mode) {
            case LookAt :
                return glm::lookAt(Pos,Target,Up);
            case PosDir :
                return glm::lookAt(Pos,Pos+Dir,Up);
            case Rot : {
            		/*
                    glm::vec3 t;
                    t.x=std::cos(glm::radians(rotX));
                    t.z=std::sin(glm::radians(rotX));
                    t.y=t.y=std::sin(glm::radians(rotY));
                    t.x*=glm::cos(glm::radians(rotY));//+0.01;
                    t.z*=glm::cos(glm::radians(rotY));//+0.01;
                    Dir=t=glm::normalize(t);
                    */
                    quat q(vec3(glm::radians(rotY),glm::radians(rotX),0));
                    Target=Pos+normalize(vec3(q*vec4(0,0,1,0)));
					//return glm::lookAt(Pos,Pos+normalize(vec3(mat4_cast(q)*vec4(1,0,0,0))+vec3(0,-0.5,0)),Up);
                    return glm::lookAt(Pos,Target,Up);
                }
            case TargetRot: {
                    quat q(vec3(glm::radians(rotY),glm::radians(rotX),0));
                    Pos=normalize(vec3(q*vec4(0,0,1,0)));//rotate(mat4(1),rotX,vec3(0,1,0))*rotate(mat4(1),rotY,vec3(1,0,0))*vec4(1,0,0,0)));
                    //return glm::lookAt(Pos,Pos+normalize(vec3(mat4_cast(q)*vec4(1,0,0,0))+vec3(0,-0.5,0)),Up);
                    Pos=Target-Pos*distance;
                    return glm::lookAt(Pos,Target,Up);
                }
            case Matrix:
                return matrix;

        }
    return glm::mat4(1);
}

void Camera::setMatrix(mat4 m)
{
    matrix=m;
}


void Camera::setRotX(float degree)
{
    //degree=fmod(degree,360);
    if(degree<-180)degree=-degree;
    else if(degree>180)degree=-180;
    rotX=degree;
}
float Camera::getRotX()
{
    return rotX;
}
void Camera::setRotY(float degree)
{
    if(degree<-89)degree=-89;
    else if(degree>89)degree=89;
    rotY=degree;
}

float Camera::getRotY()
{
    return rotY;
}

void Camera::setDir(glm::vec3 d)
{

    d=normalize(d);
    if(d.x==0&&d.z==0) {
            d.z=0.0001;
            d=normalize(d);
        }
    Dir=d;
    vec2 xz(d.x,d.z);
    //xz=normalize(xz);
    setRotX(180/3.14*atan2(xz.x,xz.y));
    //rotX=atan2(xz.x,xz.y);
    setRotY(180/3.14*sin(d.y));
}

vec3 Camera::getDir()
{
    if(mode&PosDir)return Dir;
    glm::vec3 t;
    t.x=glm::cos(glm::radians(rotX));
    t.z=glm::sin(glm::radians(rotX));
    t.y=glm::sin(glm::radians(rotY));
    t.x*=glm::cos(glm::radians(rotY))+0.001;
    t.z*=glm::cos(glm::radians(rotY))+0.001;
    Dir=t=glm::normalize(t);
    return Dir;
}



void Engine::Camera::UpdatePlanes()
{

    //else {cout<<"Error: camera::UpdatePlanes() scene null pointer ";return;}

    mat4 mvp=transpose(projection*getMatrix());//inverse(getMatrix())/**inverse(scale(mat4(1),vec3(2)))*/;;
    float a=1.0;
    for(int z=-1,i=0; z<=1; z+=2)
        for(int x=-1; x<=1; x+=2)
            for(int y=-1; y<=1; y+=2,i++) {
                    vec4 p=inverse(projection*getMatrix())*vec4(x/a,y/a,z/a,1);
                    corners[i]=vec3(p)/p.w;
                }

    //left
    planes[0]=Plane(mvp[0][0]+mvp[3][0],mvp[0][1]+mvp[3][1],mvp[0][2]+mvp[3][2],mvp[0][3]+mvp[3][3]);
    // Left clipping plane.
    //planes[1]=Plane( mvp[0][3]+mvp[0][0], mvp[1][3]+mvp[1][0], mvp[2][3]+mvp[2][0], mvp[3][3]+mvp[3][0] );

    //right
    planes[1]=Plane(-mvp[0][0]+mvp[3][0],-mvp[0][1]+mvp[3][1],-mvp[0][2]+mvp[3][2],-mvp[0][3]+mvp[3][3]);

    // Bottom clipping plane.
    planes[2]=Plane(mvp[1][0]+mvp[3][0],mvp[1][1]+mvp[3][1],mvp[1][2]+mvp[3][2],mvp[1][3]+mvp[3][3]);
    //planes[2]=Plane( mvp[0][3]+mvp[0][1], mvp[1][3]+mvp[1][1], mvp[2][3]+mvp[2][1], mvp[3][3]+mvp[3][1] );
    // Top clipping plane.
    planes[3]=Plane(-mvp[1][0]+mvp[3][0],-mvp[1][1]+mvp[3][1],-mvp[1][2]+mvp[3][2],-mvp[1][3]+mvp[3][3]);
    //planes[3]=Plane( mvp[0][3]-mvp[0][1], mvp[1][3]-mvp[1][1], mvp[2][3]-mvp[2][1], mvp[3][3]-mvp[3][1] );
    // Far clipping plane.
    planes[4]=Plane(mvp[2][0]+mvp[3][0],mvp[2][1]+mvp[3][1],mvp[2][2]+mvp[3][2],mvp[2][3]+mvp[3][3]);
    //planes[4]=Plane( mvp[0][3]-mvp[0][2], mvp[1][3]-mvp[1][2], mvp[2][3]-mvp[2][2], mvp[3][3]-mvp[3][2] );
    // Near clipping plane.
    //planes[5]=Plane( mvp[0][3]+mvp[0][2], mvp[1][3]+mvp[1][2], mvp[2][3]+mvp[2][2], mvp[3][3]+mvp[3][2] );
    planes[5]=Plane(-mvp[2][0]+mvp[3][0],-mvp[2][1]+mvp[3][1],-mvp[2][2]+mvp[3][2],-mvp[2][3]+mvp[3][3]);

}
