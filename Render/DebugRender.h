#pragma once

#include "../Core/Object.h"
#include "../Core/Scene.h"

namespace Engine
{

#define __DebugRender ((DebugRenderSubSystem*)(param.scene->systemManager->getSystem(StringHash("DebugRender").getHash())))
#define RenderSphere(center,radius,ttl) __DebugRender->renderSphere(center,radius,ttl,0);

	
#define RenderLine2(p1,p2) __DebugRender->renderLine(p1,p2);
#define RenderLine3(p1,p2,ttl) __DebugRender->renderLine(p1,p2,ttl);
#define RenderLine4(p1,p2,ttl,ss) __DebugRender->renderLine(p1,p2,ttl,ss);
		
#define GET_MACRO(_1,_2,_3,_4,NAME,...) NAME
#define RenderLine(...) GET_MACRO(__VA_ARGS__,RenderLine4, RenderLine3, RenderLine2)(__VA_ARGS__)	
	

	class DebugRenderSubSystem : public RenderSubSystem
	{
		struct line{
			vec3 p1,p2;	
			float ttl;
			bool screenSpace;
		};
		struct sphere{
			vec3 center;
			float radius;	
			float ttl;
			bool screenSpace;
		};
		list<line> lines;
		list<sphere> spheres;
		list<line> lines_ss;
		
	public:
	    DebugRenderSubSystem() {}
	
	    void Init(){}
	    void render(uint flag=RenderFlag::Default){

	    	glUseProgram(0);
	    	glPushMatrix();
			_Scene->current_camera->MultMatrix();
			
			glColor3f(1,0,0);
	    	glBegin(GL_LINES);
			for(list<line>::iterator it=lines.begin();it!=lines.end();it++){
				glVertex3fv(&(*it).p1[0]);
				glVertex3fv(&(*it).p2[0]);
			}
			glEnd();
			
			glColor3f(1,1,0);
			
			for(list<sphere>::iterator it=spheres.begin();it!=spheres.end();it++){
				glPushMatrix();
				glTranslatef((*it).center[0],(*it).center[1],(*it).center[2]);
				glutWireSphere((*it).radius,10,10);
				glPopMatrix();
			}
			
			glPopMatrix();
			glColor3f(0,1,0);
			/*
			for(list<line>::iterator it=lines_ss.begin();it!=lines_ss.end();it++){
				if((*it).ttl--){lines_ss.erase(it--);}
			}*/
		}
	    
	    void update(float deltaTimeMs) {
	    	float sec=deltaTimeMs/1000.0f;
	        for(list<line>::iterator it=lines.begin();it!=lines.end();it++){
				if((*it).ttl<0){lines.erase(it--);}
				(*it).ttl-=sec;	
			}
			for(list<line>::iterator it=lines_ss.begin();it!=lines_ss.end();it++){
				if((*it).ttl<0){lines_ss.erase(it--);}
				(*it).ttl-=sec;	
			}
			for(list<sphere>::iterator it=spheres.begin();it!=spheres.end();it++){
				if((*it).ttl<0){spheres.erase(it--);}
				(*it).ttl-=sec;	
			}
	    }
	    virtual SystemID getSystemType(void) const {
	        return StringHash("DebugRender").getHash();
	    }
	    
	    
	    void renderLine(vec3 p1,vec3 p2,float ttl=1,bool screenSpace=0){
			if(screenSpace){
				
			}else{
				line l={p1,p2,ttl,screenSpace};
				lines.push_back(l);	
			}
		}
		void renderSphere(vec3 center,float radius,float ttl=1,bool screenSpace=0){
			if(screenSpace){
				
			}else{
				sphere s={center,radius,ttl,screenSpace};
				spheres.push_back(s);	
			}
			
		}
	};

	
	
}

