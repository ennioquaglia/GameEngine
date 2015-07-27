#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



namespace Engine{
	
	
class Shape 
{

};

class Plane : public Shape
{
	
	glm::vec3 normal;
public:
	float a,b,c,d;
	Plane(){a=0;b=0;c=0;d=0;}
	Plane(float a,float b,float c,float d){
		this->a=a;
		this->b=b;
		this->c=c;
		this->d=d;
		normal=glm::normalize(glm::vec3(a,b,c));
	}
	Plane(glm::vec3 p0,glm::vec3 p1,glm::vec3 p2){
		using namespace glm;
		vec3 v=p1-p0,u=p2-p0;
		normal=normalize(cross(v,u));
		a=normal.x;
		b=normal.y;
		c=normal.z;
		d=dot(-normal,p0);
	}

	Plane Normalize()
	{
		Plane Result;
		float Distance = sqrtf(a * a + b * b + c * c);
		Result.a = a / Distance;
		Result.b = b / Distance;
		Result.c = c / Distance;
		Result.d = d / Distance;
		a=Result.a;
		b=Result.b;
		c=Result.c;
		d=Result.d;
		return Result;
	}
	float Distance(const glm::vec3 &Pt) const
	{
		return (a * Pt.x + b * Pt.y + c * Pt.z + d);
	}
	void render(){
		//Engine::renderVector(normal*d,normal*(d+1.0f));
	}
	glm::vec4 getValue(){return glm::vec4(a,b,c,d);}
	glm::vec3 getNormal(){return glm::normalize(glm::vec3(a,b,c));}
};

class Sphere : public Shape
{
public:
	glm::vec3 center;
	float radius;

	Sphere(glm::vec3 c,float r){center=c;radius=r;}
};


class AABBox : public Shape
{
public:
	glm::vec3 min,max;

	AABBox(glm::vec3 m, glm::vec3 M)
	{
		min=m;
		max=M;
	}
};


}//namespace Engine