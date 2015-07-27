#pragma once

#include "../Core/Scene.h"
#include "../common/Debug.h"
#include "RenderUtility.h"
#include "Texture.h"
namespace Engine{
	
	
	class PostProcessor
	{
		protected: 
			Scene *scene;
		public:
			PostProcessor(Scene *s):scene(s){}
			virtual void Render(){
				scene->Draw();
			}
			virtual void handleEvent(Event &msg) {
				
			}
 
	};
	
	
	class DeferredPostProcessor : public PostProcessor
	{
			FrameBufferRGBA framebuffer;
			Shader *shader;
		public:
			DeferredPostProcessor(Scene *s):PostProcessor(s){
				framebuffer.resetFrameBuffer(glm::vec2(scene->window->Width,scene->window->Height));//s->window->Width,s->window->Height));

				shader = new Shader(AssetManager::getBasePath() + "Data/Shader/Debug/shader.vs", AssetManager::getBasePath() + "Data/Shader/Debug/shader.fs");
				AssetManager::addShader(shader);
			}
			virtual void Render(){
				framebuffer.bind();
				
				float ratio;

		        float Width=scene->window->Width, Height=scene->window->Height;
		        ratio = Width / (float) Height;

	    		glViewport(0, 0, Width,Height);
	
			    // Setup a perspective projection
			    glMatrixMode(GL_PROJECTION);
			    glLoadIdentity();
			    gluPerspective(60,ratio,0.1,500);
			    glMatrixMode(GL_MODELVIEW);
		        glLoadIdentity();
		        
		        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		        
				scene->Draw(RenderFlag::Default);
				framebuffer.unbind();
				
				
				
				glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

				shader->Use();
				glEnable(GL_TEXTURE_2D);
				glUniform1i(glGetUniformLocation(shader->getProgramId(),"tex"),0);
				//glUniform1i(glGetUniformLocation(MatrixShader->getProgramId(),"char"),1);
				glActiveTexture (GL_TEXTURE0);
				glBindTexture (GL_TEXTURE_2D,framebuffer.getTextureId());
				//glActiveTexture (GL_TEXTURE1);
				//glBindTexture (GL_TEXTURE_2D,charSetTex->getID());
		
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
				glUseProgram(0);
				
			}
			virtual void handleEvent(Event &msg) {
				if(msg.getType()==StringHash("windowResize").getHash()){
					framebuffer.resetFrameBuffer(msg.getData<vec2>());
				}
			}

	};
	
	
	class MatrixModePostProcessor : public PostProcessor
	{
			FrameBufferRGBA framebuffer;
			Shader *shader;
			Texture *charTex;
		public:
			MatrixModePostProcessor(Scene *s):PostProcessor(s){
				framebuffer.resetFrameBuffer(glm::vec2(scene->window->Width,scene->window->Height));//s->window->Width,s->window->Height));

				shader = new Shader(AssetManager::getBasePath() + "Data/Shader/MatrixMode/shader.vs", AssetManager::getBasePath() + "Data/Shader/MatrixMode/shader.fs");
				AssetManager::addShader(shader);
				
				charTex=AssetManager::addTexture(AssetManager::getBasePath()+"Data/Texture/char.png");
			}
			virtual void Render(){
				framebuffer.bind();
				
				float ratio;

		        float Width=scene->window->Width, Height=scene->window->Height;
		        ratio = Width / (float) Height;

	    		glViewport(0, 0, Width,Height);
	
			    // Setup a perspective projection
			    glMatrixMode(GL_PROJECTION);
			    glLoadIdentity();
			    gluPerspective(60,ratio,0.1,500);
			    glMatrixMode(GL_MODELVIEW);
		        glLoadIdentity();
		        
		        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		        
				scene->Draw(RenderFlag::Default);
				framebuffer.unbind();
				
				
				
				glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

				shader->Use();
				glEnable(GL_TEXTURE_2D);
				glUniform1i(glGetUniformLocation(shader->getProgramId(),"tex"),0);
				glUniform1i(glGetUniformLocation(shader->getProgramId(),"char"),1);
				glUniform2f(glGetUniformLocation(shader->getProgramId(),"pixel"),Width/8,Height/8);
				glActiveTexture (GL_TEXTURE0);
				glBindTexture (GL_TEXTURE_2D,framebuffer.getTextureId());
				glActiveTexture (GL_TEXTURE1);
				glBindTexture (GL_TEXTURE_2D,charTex->getID());
		
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
				glUseProgram(0);
				
			}
			virtual void handleEvent(Event &msg) {
				if(msg.getType()==StringHash("windowResize").getHash()){
					framebuffer.resetFrameBuffer(msg.getData<vec2>());
				}
			}

	};
	
	
	
	class RealDeferredPostProcessor : public PostProcessor
	{
			FrameBufferRGBA framebuffer;
			Shader *shader;
			//BlurKernel blurKernel;
		public:
			RealDeferredPostProcessor(Scene *s):PostProcessor(s){
				framebuffer.resetFrameBuffer(glm::vec2(scene->window->Width,scene->window->Height));//s->window->Width,s->window->Height));
			//	blurKernel.setSize(glm::vec2(scene->window->Width,scene->window->Height));
				
				shader = new Shader(AssetManager::getBasePath() + "Data/Shader/Debug/shader.vs", AssetManager::getBasePath() + "Data/Shader/Debug/shader.fs");
				AssetManager::addShader(shader);
			}
			virtual void Render(){
				framebuffer.bind();
				
				float ratio;

		        float Width=scene->window->Width, Height=scene->window->Height;
		        ratio = Width / (float) Height;

	    		glViewport(0, 0, Width,Height);
	
			    // Setup a perspective projection
			    glMatrixMode(GL_PROJECTION);
			    glLoadIdentity();
			    gluPerspective(60,ratio,0.1,500);
			    glMatrixMode(GL_MODELVIEW);
		        glLoadIdentity();
		        
		        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		        
				scene->Draw(RenderFlag::Default);
				framebuffer.unbind();
				

				
				
				static BlurKernel blur(vec2(Width,Height));
				glDisable(GL_DEPTH_TEST);
				unsigned int tex= blur.blurTexture(framebuffer.getTextureId(),vec2(0.01));
				glEnable(GL_DEPTH_TEST);

				
				glBindFramebuffer (GL_FRAMEBUFFER, 0);
				glViewport(0, 0, Width,Height);
			    // Setup a perspective projection
			    glMatrixMode(GL_PROJECTION);
			    glLoadIdentity();
			    gluPerspective(60,ratio,0.1,500);
			    glMatrixMode(GL_MODELVIEW);
		        glLoadIdentity();
		        
		        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				shader->Use();
				glEnable(GL_TEXTURE_2D);
				glUniform1i(glGetUniformLocation(shader->getProgramId(),"tex"),0);
				//glUniform1i(glGetUniformLocation(MatrixShader->getProgramId(),"char"),1);
				glActiveTexture (GL_TEXTURE0);
				glBindTexture (GL_TEXTURE_2D,tex);
				//glBindTexture (GL_TEXTURE_2D,framebuffer.getTextureId());
				//glActiveTexture (GL_TEXTURE1);
				//glBindTexture (GL_TEXTURE_2D,charSetTex->getID());
		
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
				glUseProgram(0);
				
			}
			virtual void handleEvent(Event &msg) {
				if(msg.getType()==StringHash("windowResize").getHash()){
					framebuffer.resetFrameBuffer(msg.getData<vec2>());
				}
			}

	};
	
}