#pragma once

#include "../../Core/object.h"
#include "../../common/AssetManager.h"
#include "../../Render/Texture.h"


class  TextureDebug : public GraphicComponent
{

	ENGINE_COMPONENT_MACRO
	Shader * shader;
public:
	TextureDebug() {
		shader = new Shader(AssetManager::getBasePath() + "Data/Shader/Debug/shader.vs", AssetManager::getBasePath() + "Data/Shader/Debug/shader.fs");
		AssetManager::addShader(shader);
		glDebug();

	}
	void unitDraw(RenderParameter &param) {
		if(param.flag != RenderFlag::Default)return;
		shader->Use();

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(-1, -0.2);
		glTexCoord2f(1, 0);
		glVertex2f(0, -0.2);
		glTexCoord2f(1, 1);
		glVertex2f(0, 0.2);
		glTexCoord2f(0, 1);
		glVertex2f(-1, 0.2);
		glEnd();
		glUseProgram(0);
	}

};

class  ParticleSystem : public GraphicComponent
{

	ENGINE_COMPONENT_MACRO

protected:
	Shader * renderShader, *updateShader;

	uint fb[2], rb[2];
	uint fb_tex[2][2];

	bool setup_done;
	uint list_index;
	uint current_index;
	Texture* noiseTex;
	uint numberParticles;
public:

	ParticleSystem() {}
	
	virtual void setup(){
		
		flag.add("castShadow");
		setup_done = 0;
		current_index = 1;
		numberParticles = 32;


		renderShader = new Shader(AssetManager::getBasePath() + "Data/Shader/ParticleSystem/render/shader.vs", AssetManager::getBasePath() + "Data/Shader/ParticleSystem/render/shader.fs");
		AssetManager::addShader(renderShader);
		glDebug();

		updateShader = new Shader(AssetManager::getBasePath() + "Data/Shader/ParticleSystem/update/shader.vs", AssetManager::getBasePath() + "Data/Shader/ParticleSystem/update/shader.fs");
		AssetManager::addShader(updateShader);
		glDebug();

		noiseTex = AssetManager::addTexture("noise512");

		glPointSize(1);
		for(int i = 0; i < 2; i++) {
			fb[i] = rb[i] = 0;
			fb_tex[i][0] = fb_tex[i][1] = 0;
			createFramebuffer(fb[i], fb_tex[i], rb[i], numberParticles, numberParticles);
			Error("FB=" + fb[i]);
		}

		list_index = glGenLists(1);

		// compile the display list, store a triangle in it
		glNewList(list_index, GL_COMPILE);
		glBegin(GL_POINTS);
		for(int i = 0; i < numberParticles; i++) {
			for(int x = 0; x < numberParticles; x++) {
				glVertex3f((1 / (float)numberParticles) * (i), (1 / (float)numberParticles) * (x), 0);
			}
		}
		glEnd();
		glEndList();

		glColor3f(1, 0, 0);
		UpdateParameter p;
		update(p);
		update(p);

		setup_done = 1;

		glDebug();




	}

	virtual void unitDraw(RenderParameter &param) { // {
		glPointSize(5);
		glColor3f(1, 0, 0);
		glDebug();
		renderShader->Use();

		glUniform1i(glGetUniformLocation(renderShader->getProgramId(), "PosTex"), 0);
		glUniform1i(glGetUniformLocation(renderShader->getProgramId(), "VelTex"), 1);

		glUniformMatrix4fv(glGetUniformLocation(renderShader->getProgramId(), "CM"), 1, 0, &(param.scene->current_camera->getMatrix())[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(renderShader->getProgramId(), "PM"), 1, 0, &(param.scene->current_camera->getProjectionMatrix())[0][0]);

		TransformComponent * t;
		if(m_pOwner)t = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
		if(t) {
			glUniformMatrix4fv(glGetUniformLocation(renderShader->getProgramId(), "MVM"), 1, 0, &(t->getMatrix())[0][0]);
		}

		glActiveTexture (GL_TEXTURE0);
		glBindTexture (GL_TEXTURE_2D, fb_tex[current_index][0]);
		glActiveTexture (GL_TEXTURE1);
		glBindTexture (GL_TEXTURE_2D, fb_tex[current_index][1]);
		//glBindTexture (GL_TEXTURE_2D,noiseTex->getID());
		//glBindTexture (GL_TEXTURE_2D,noiseTex->getID());

		glCallList(list_index);
		/*
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex2f(-1,-0.1);
			glTexCoord2f(1,0);
			glVertex2f(0,-0.1);
			glTexCoord2f(1,1);
			glVertex2f(0,0.1);
			glTexCoord2f(0,1);
			glVertex2f(-1,0.1);
		glEnd();

		glBindTexture (GL_TEXTURE_2D,fb_tex[current_index][1]);
		//glBindTexture (GL_TEXTURE_2D,noiseTex->getID());
		//glBindTexture (GL_TEXTURE_2D,noiseTex->getID());

		//glCallList(list_index);

		glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex2f(0,-0.1);
			glTexCoord2f(1,0);
			glVertex2f(1,-0.1);
			glTexCoord2f(1,1);
			glVertex2f(1,0.1);
			glTexCoord2f(0,1);
			glVertex2f(0,0.1);
		glEnd();
		*/
		glDebug();
		glUseProgram(0);



	}

	virtual void update(UpdateParameter &param) { //float deltaTimeMs,Actor *actor,Engine::Scene* scene) {


		//if(time>=500){current_index=!current_index;time=0;}
		//	else return;

		//if(current_index)return;
		if(setup_done)
			glColor3f(0, 0, 0);
		current_index = !current_index;

		glDebug();
		glBindFramebuffer(GL_FRAMEBUFFER, fb[current_index]);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, numberParticles, numberParticles);

		updateShader->Use();
		glDebug();
		static float time = 0;
		time += param.deltaTimeMs / 1000.;
		glUniform2f(glGetUniformLocation(updateShader->getProgramId(),"bufferSize"), numberParticles, numberParticles);
		glUniform3f(glGetUniformLocation(updateShader->getProgramId(), "emitterPos"), sin(time * 2), cos(time * 5), cos(time));
		glUniform1f(glGetUniformLocation(updateShader->getProgramId(), "deltaTime"), param.deltaTimeMs / 1000.0f);

		glUniform1i(glGetUniformLocation(updateShader->getProgramId(), "PosTex"), 0);
		glUniform1i(glGetUniformLocation(updateShader->getProgramId(), "VelTex"), 1);
		glUniform1i(glGetUniformLocation(updateShader->getProgramId(), "NoiseTex"), 2);
		glDebug();
		glActiveTexture (GL_TEXTURE0);
		glBindTexture (GL_TEXTURE_2D, fb_tex[!current_index][0]);
		glActiveTexture (GL_TEXTURE1);
		glBindTexture (GL_TEXTURE_2D, fb_tex[!current_index][1]);
		glActiveTexture (GL_TEXTURE2);
		glBindTexture (GL_TEXTURE_2D, noiseTex->getID());

		glDebug();
		//glCallList(list_index);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(-1, -0.1);
		glTexCoord2f(1, 0);
		glVertex2f(1, -0.1);
		glTexCoord2f(1, 1);
		glVertex2f(1, 0.1);
		glTexCoord2f(0, 1);
		glVertex2f(-1, 0.1);
		glEnd();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(0);
		glDebug();
	}


	uint createFramebuffer(uint &fb, uint fb_tex[2], uint &rb, int width, int height) {
		if(!width) {
			width = 256;
			height = 256;
		}

		if(fb_tex[0])glDeleteTextures(1, &fb_tex[0]);
		if(fb_tex[1])glDeleteTextures(1, &fb_tex[1]);
		if(fb)glDeleteFramebuffers(1, &fb);
		if(rb)glDeleteRenderbuffers(1, &rb);
		glGenFramebuffers(1, &fb);

		glDisable(GL_CULL_FACE);
		glDebug();
		for(int i = 0; i < 2; i++) {
			glGenTextures(1, &fb_tex[i]);
			glBindTexture(GL_TEXTURE_2D, fb_tex[i]);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, width, height, 0, GL_RGBA, GL_FLOAT, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


			glBindFramebuffer(GL_FRAMEBUFFER, fb);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, fb_tex[i], 0);
		}

		glDebug();
		rb = 0;
		glGenRenderbuffers(1, &rb);
		glBindRenderbuffer(GL_RENDERBUFFER, rb);
		glRenderbufferStorage(
			GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height
		);
		glFramebufferRenderbuffer(
			GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb
		);

		GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, draw_bufs);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if(status != GL_FRAMEBUFFER_COMPLETE) {
			printf("\n\nframebuffer error(fb:%d,tex:%d):%x\n", fb, fb_tex[0], status);
			system("pause");
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return 0;
		}

		glDebug();

		// switch back to window-system-provided framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//system("pause");
		return 1;
	}
};


class ParticleSystem1 : public ParticleSystem
{
	ENGINE_COMPONENT_MACRO

public:
	
	virtual void setup(){
		
		flag.add("ParticleSystem1");
		flag.add("castShadow");
		setup_done = 0;
		current_index = 1;
		numberParticles = 128;


		renderShader = new Shader(AssetManager::getBasePath() + "Data/Shader/ParticleSystem/render/shader.vs", AssetManager::getBasePath() + "Data/Shader/ParticleSystem/render2/shader.fs");
		AssetManager::addShader(renderShader);
		glDebug();

		updateShader = new Shader(AssetManager::getBasePath() + "Data/Shader/ParticleSystem/update2/shader.vs", AssetManager::getBasePath() + "Data/Shader/ParticleSystem/update2/shader.fs");
		AssetManager::addShader(updateShader);
		glDebug();

		noiseTex = AssetManager::addTexture("noise512");

		glPointSize(1);
		for(int i = 0; i < 2; i++) {
			fb[i] = rb[i] = 0;
			fb_tex[i][0] = fb_tex[i][1] = 0;
			createFramebuffer(fb[i], fb_tex[i], rb[i], numberParticles, numberParticles);
			Error("FB=" + fb[i]);
		}

		list_index = glGenLists(1);

		// compile the display list, store a triangle in it
		glNewList(list_index, GL_COMPILE);
		glBegin(GL_POINTS);
		for(int i = 0; i < numberParticles; i++) {
			for(int x = 0; x < numberParticles; x++) {
				glVertex3f((1 / (float)numberParticles) * i, (1 / (float)numberParticles) * x, 0);
			}
		}
		glEnd();
		glEndList();

		glColor3f(1, 0, 0);
		UpdateParameter p;
		update(p);
		update(p);

		setup_done = 1;

		glDebug();

	}

};
