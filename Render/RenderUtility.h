#pragma once
#include <glm/glm.hpp>
#include "../common/Event.h"
#include "../common/Hash.h"
#include "Shader.h"
#include "../common/AssetManager.h"

namespace Engine
{


	class FrameBuffer
	{
	public:
		FrameBuffer() {}
		virtual void resetFrameBuffer(glm::vec2 size) = 0;
		virtual void bind() {}
		virtual void unbind() {}
		virtual void handleEvent(Event &msg) {
			if(msg.getType() == StringHash("windowResize").getHash()) {
				resetFrameBuffer(msg.getData<glm::vec2>());
			}
		}
	};


	class FrameBufferRGBA : public FrameBuffer
	{
		unsigned int fb, fb_tex, rb;
		
	public:
		int width, height;
		FrameBufferRGBA() {
			fb = fb_tex = rb = 0;
			width = height = 0;
		}
		virtual void resetFrameBuffer(glm::vec2 size) {
			//Debug("\n\nsto per creare il framebuffer");
			width = size.x;
			height = size.y;

			if(!width) {
				width = 256;
				height = 256;
			}

			if(fb_tex)glDeleteTextures(1, &fb_tex);
			if(fb)glDeleteFramebuffers(1, &fb);
			if(rb)glDeleteRenderbuffers(1, &rb);
			glGenFramebuffers (1, &fb);


			glGenTextures (1, &fb_tex);
			glBindTexture (GL_TEXTURE_2D, fb_tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
						// Poor filtering. Needed !
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			
			glBindFramebuffer (GL_FRAMEBUFFER, fb);
			glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_tex, 0);

			rb = 0;
			glGenRenderbuffers (1, &rb);
			glBindRenderbuffer (GL_RENDERBUFFER, rb);
			glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb);
			
			GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers (1, draw_bufs);

			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if(status != GL_FRAMEBUFFER_COMPLETE) {
				Error("error in framebuffer(class) creation");
				glBindFramebuffer (GL_FRAMEBUFFER, 0);
				return ;
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		}
		virtual unsigned int getTextureId() const{
			return fb_tex;
		}
		virtual void bind() {
			glBindFramebuffer (GL_FRAMEBUFFER, fb);
		}
		virtual void unbind() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		virtual bool ok() {
			return fb != 0;
		}

	};
	
	class FrameBufferDepth_old : public FrameBuffer
	{
		unsigned int fb, fb_tex;
		int width, height;
	public:
		FrameBufferDepth_old() {
			fb = fb_tex = 0;
			width = height = 0;
		}
		virtual void resetFrameBuffer(glm::vec2 size) {
			
			width = size.x;
			height = size.y;

			if(!width) {
				width = 256;
				height = 256;
			}

			if(fb_tex)glDeleteTextures(1, &fb_tex);
			if(fb)glDeleteFramebuffers(1, &fb);


			if(fb_tex)glDeleteTextures(1, &fb_tex);
			glGenFramebuffers(1, &fb);
			glBindFramebuffer(GL_FRAMEBUFFER, fb);

			glGenTextures(1, &fb_tex);
			glActiveTexture (GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, fb_tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

			glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fb_tex, 0);

			GLenum draw_bufs[] = { GL_NONE };

			glDrawBuffers (1, draw_bufs);
			if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				Error("\n\nLight framebuffer error\n");
				glBindFramebuffer (GL_FRAMEBUFFER, 0);
				//return 0;
			}
			glBindFramebuffer (GL_FRAMEBUFFER, 0);
			//glDebug("depth channel framebuffer");

		}
		virtual void bind() {
			glBindFramebuffer (GL_FRAMEBUFFER, fb);
		}
		virtual void unbind() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		virtual bool ok() {
			return fb != 0;
		}
		virtual unsigned int getTextureId() const{
			return fb_tex;
		}

	};
	
	class FrameBufferDepth : public FrameBuffer
	{
		
		int width, height;
		unsigned int fb, fb_tex, rb;
	public:
	
		FrameBufferDepth() {
			fb = fb_tex = rb = 0;
			width = height = 0;
		}
		virtual void resetFrameBuffer(glm::vec2 size) {
			//Debug("\n\nsto per creare il framebuffer");
			width = size.x;
			height = size.y;

			if(!width) {
				width = 256;
				height = 256;
			}

			if(fb_tex)glDeleteTextures(1, &fb_tex);
			if(fb)glDeleteFramebuffers(1, &fb);
			if(rb)glDeleteRenderbuffers(1, &rb);
			glGenFramebuffers (1, &fb);


			glGenTextures (1, &fb_tex);
			glBindTexture (GL_TEXTURE_2D, fb_tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
						// Poor filtering. Needed !
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			
			glBindFramebuffer (GL_FRAMEBUFFER, fb);
			glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_tex, 0);

			rb = 0;
			glGenRenderbuffers (1, &rb);
			glBindRenderbuffer (GL_RENDERBUFFER, rb);
			glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb);
			
			GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers (1, draw_bufs);

			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if(status != GL_FRAMEBUFFER_COMPLETE) {
				Error("error in framebuffer(class) creation");
				glBindFramebuffer (GL_FRAMEBUFFER, 0);
				return ;
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		}
		virtual unsigned int getTextureId() const{
			return fb_tex;
		}
		virtual void bind() {
			glBindFramebuffer (GL_FRAMEBUFFER, fb);
			//glViewport(0, 0, width, height);
		}
		virtual void unbind() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		virtual bool ok() {
			return fb != 0;
		}

	};
	
	//template <typename T>
	class BlurKernel
	{
			static Shader *shader;
			FrameBufferRGBA framebuffer[2];
		public:
			BlurKernel(vec2 size=vec2(0)){
				if(size!=vec2(0)){
				framebuffer[0].resetFrameBuffer(size);
				framebuffer[1].resetFrameBuffer(size);
				}
				if(!shader){
					shader=new Shader(AssetManager::getBasePath() + "Data/Shader/Blur/shader.vs", AssetManager::getBasePath() + "Data/Shader/Blur/shader.fs");
					AssetManager::addShader(shader);
				}
			}
			void setSize(vec2 size){
				framebuffer[0].resetFrameBuffer(size);
				framebuffer[1].resetFrameBuffer(size);
			}
			unsigned int blurTexture(unsigned int TextureFrom,glm::vec2 blur){
				if(blur==vec2(0)||!framebuffer[0].ok()){Debug("errore blur");return TextureFrom;}
				framebuffer[0].bind();	 	
				shader->Use();
				glUniform1i(glGetUniformLocation(shader->getProgramId(),"inputTexture"),0);
				glUniform1i(glGetUniformLocation(shader->getProgramId(),"inputTexture"),0);
				glUniform1f(glGetUniformLocation(shader->getProgramId(),"BlurFactor"),blur.x);
				glUniform1i(glGetUniformLocation(shader->getProgramId(),"axis"),1);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D,TextureFrom);
				
				
				glViewport (0, 0, framebuffer[0].width, framebuffer[0].height);
				
				glClearColor (0.0, 0.0, 0.0, 0.0);
				glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

				glMatrixMode(GL_PROJECTION);
			    glLoadIdentity();
			    gluPerspective(60,0.7,0.1,500);
			    glMatrixMode(GL_MODELVIEW);
		        glLoadIdentity();
				glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
				glBegin(GL_QUADS);
					glTexCoord2f(0,0);
					glVertex2f(-1,-1);
					glTexCoord2f(1,0);
					glVertex2f(1,-1);
					glTexCoord2f(1,1);
					glVertex2f(1,1);
					glTexCoord2f(0,1);
					glVertex2f(-1,1);
				glEnd();
				framebuffer[0].unbind();
				return framebuffer[0].getTextureId();
				glUniform1f(glGetUniformLocation(shader->getProgramId(),"BlurFactor"),blur.y);
				glUniform1i(glGetUniformLocation(shader->getProgramId(),"axis"),0);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D,framebuffer[0].getTextureId());
			

				glViewport (0, 0, framebuffer[1].width, framebuffer[1].height);
				framebuffer[1].bind();
				glClearColor (0.0, 0.0, 0.0, 0.0);
				glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
				glBegin(GL_QUADS);
					glTexCoord2f(0,0);
					glVertex2f(-1,-1);
					glTexCoord2f(1,0);
					glVertex2f(1,-1);
					glTexCoord2f(1,1);
					glVertex2f(1,1);
					glTexCoord2f(0,1);
					glVertex2f(-1,1);
				glEnd();
				framebuffer[1].unbind();
				return framebuffer[1].getTextureId();
				
			}
		uint getTextureId(){return framebuffer[0].getTextureId();}
	
		
	};
	
	class BlurKernel_new
	{
		uint fb,tex1,tex2;
		vec2 size;
		Shader* shader;

	
	public:
		BlurKernel_new(uint width,uint height){
			size=vec2(width,height);
			glGenFramebuffers (1, &fb);
			glBindFramebuffer (GL_FRAMEBUFFER, fb);
	
			glGenTextures (1, &tex1);
			glBindTexture (GL_TEXTURE_2D, tex1);
			glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, width, height, 0,GL_RGBA,GL_UNSIGNED_BYTE,0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				//GLfloat BorderColor[4]={0,0,0,0};
				//glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,BorderColor);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	
			glFramebufferTexture2D (
				GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex1, 0
			);
	
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				//glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,BorderColor);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	
			glGenTextures (1, &tex2);
			glBindTexture (GL_TEXTURE_2D, tex2);
			glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, width, height, 0,GL_RGBA,GL_UNSIGNED_BYTE,0);
			glFramebufferTexture2D (
				GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tex2, 0
			);
	
			GLenum draw_bufs[] = {GL_COLOR_ATTACHMENT0};
			glDrawBuffers (1, draw_bufs);
	
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if(status != GL_FRAMEBUFFER_COMPLETE){Error("\n\nblur framebuffer error :%x\n"+status);glBindFramebuffer (GL_FRAMEBUFFER, 0);return;}
	
	
			//cout<<"fb blur 1:"<<tex1<<" 2:"<<tex2<<"\nsize "<<size.x<<" "<<size.y<<"\n\n";
			//getch();
			// switch back to window-system-provided framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			
				shader=new Shader(AssetManager::getBasePath() + "Data/Shader/Blur/shader.vs", AssetManager::getBasePath() + "Data/Shader/Blur/shader.fs");
				AssetManager::addShader(shader);
			
		}
		~BlurKernel_new(){
			if(fb)glDeleteFramebuffers(1,&fb);
			glDeleteTextures(1,&tex1);
			glDeleteTextures(1,&tex2);
		}
	
		
		uint blurTexture(uint input,vec2 blur){
			if(size.x==0)return 0;
			glViewport(0,0,size.x,size.y);
			glBindFramebuffer (GL_FRAMEBUFFER, fb);
			glClearColor (0.0, 0.0, 0.0, 0.0);
			glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			
			shader->Use();
	
			GLenum draw_bufs[] = {GL_COLOR_ATTACHMENT0};
			glDrawBuffers (1, draw_bufs);
	
			glUniform1f(glGetUniformLocation(shader->getProgramId(),"blur"),blur.x);
			glUniform1i(glGetUniformLocation(shader->getProgramId(),"axis"),0);//x axis
	
	
			glUniform1i(glGetUniformLocation(shader->getProgramId(),"inputTexture"),0);
			
			glActiveTexture (GL_TEXTURE0);
			glBindTexture (GL_TEXTURE_2D,input);
	
			glBegin(GL_QUADS);
				glTexCoord2f(0,0);
				glVertex2f(-1,-1);
				glTexCoord2f(1,0);
				glVertex2f(1,-1);
				glTexCoord2f(1,1);
				glVertex2f(1,1);
				glTexCoord2f(0,1);
				glVertex2f(-1,1);
			glEnd();	
	
	
			glUniform1f(glGetUniformLocation(shader->getProgramId(),"blur"),blur.y);
			glUniform1i(glGetUniformLocation(shader->getProgramId(),"axis"),1);//y axis
	
			draw_bufs[0]=GL_COLOR_ATTACHMENT1;
			glDrawBuffers (1, draw_bufs);
			
			glBindTexture (GL_TEXTURE_2D,tex1);
			glBegin(GL_QUADS);
				glTexCoord2f(0,0);
				glVertex2f(-1,-1);
				glTexCoord2f(1,0);
				glVertex2f(1,-1);
				glTexCoord2f(1,1);
				glVertex2f(1,1);
				glTexCoord2f(0,1);
				glVertex2f(-1,1);
			glEnd();	
	
			glBindFramebuffer (GL_FRAMEBUFFER, 0);
			glUseProgram(0);
			return tex2;
		}
	
	};



}