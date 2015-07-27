//StandardComponent.h
#pragma once
#include "../Core/Object.h"

using namespace glm;
using namespace Engine;

class TransformComponent : public ActorComponent
{
	
    mat4 matrix,local_matrix;
    map<ComponentId::Type,mat4> localMatrices;
    vec3 pos,scale;
    Rotation rot;
	ENGINE_COMPONENT_MACRO
public:
    TransformComponent() {
        local_matrix=matrix=mat4(1);
        pos=vec3(0);
        scale=vec3(1);
    }
    virtual ComponentId::Type getComponentType(void) const {
        return ComponentId::Transform;
    }
    virtual mat4 getLocalMatrix(ComponentId::Type id){
		if(localMatrices.find(id)==localMatrices.end())localMatrices[id]=mat4(1);
		return localMatrices[id];
	}
	virtual void setLocalMatrix(ComponentId::Type id,mat4 matrix){
		localMatrices[id]=matrix;
	}
	virtual vec3 getLocalScale(ComponentId::Type id,bool global=1){
		mat4 m;
		if(global)m=glm::scale(mat4(1),scale)*getLocalMatrix(id);
		else m=getLocalMatrix(id);
		
		return vec3(m[0][0],m[1][1],m[2][2]);
	}
	virtual vec3 getLocalPos(ComponentId::Type id,bool global=1){
		mat4 m;
		if(global)m=getMatrix(id);
		else m=getLocalMatrix(id);
		
		return vec3(m[3][0],m[3][1],m[3][2]);
	}
	virtual Rotation getLocalRotation(ComponentId::Type id,bool global=1){
		mat4 m;
		if(global)m=getMatrix(id);
		else m=getLocalMatrix(id);
		Rotation rot;
		quat q=glm::quat_cast(m);
		q=normalize(q);
		rot.setQuaternion(q);
		return rot;
	}
	virtual void setLocalPos(ComponentId::Type id,vec3 v){
		mat4 m=getLocalMatrix(id);
		m[3][0]=v.x;
		m[3][1]=v.y;
		m[3][2]=v.z;
		setLocalMatrix(id,m);
	}
	virtual void setLocalScale(ComponentId::Type id,vec3 v){
		mat4 m=getLocalMatrix(id);
		m[0][0]=v.x;
		m[1][1]=v.y;
		m[2][2]=v.z;
		setLocalMatrix(id,m);
	}
	
	virtual void setPos(vec3 v){pos=v;}
	virtual void addPos(vec3 v){pos+=v;}
	virtual void setScale(vec3 v){scale=v;}
	virtual void setRotation(Rotation r){rot=r;}
	virtual mat4 getMatrix(ComponentId::Type id=ComponentId::Render){
		//if(id){ 
			return glm::translate(mat4(1),pos)*rot.getMatrix()*glm::scale(mat4(1),scale)*getLocalMatrix(id);
		//}else return glm::translate(mat4(1),pos)*glm::scale(mat4(1),scale);
	}
	
	
    void MultMatrix() {
         glMultMatrixf(&getMatrix(ComponentId::Render)[0][0]);
    }
    void setMatrix(mat4 m,bool use_scale=0) {
		 pos=vec3(m[3][0],m[3][1],m[3][2]);
		 quat q=glm::quat_cast(m);
		 q=normalize(q);
		 rot.setQuaternion(q);
		 if(use_scale)
		 	 scale=vec3(m[0][0],m[1][1],m[2][2]);//vec3(m*vec4(1,1,1,1));// 
         matrix=m;
    }
    
    virtual vec3 getPos(bool global=1){if(global)return vec3(getMatrix()*vec4(0,0,0,1));return pos;}
    vec3 pos1;
    vec3 returnPos(){return pos1;}
    virtual vec3 getScale(){return scale;}
    
    
};
