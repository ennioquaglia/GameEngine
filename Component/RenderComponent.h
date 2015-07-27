#pragma once
#include "StandardComponent.h"
#include "../Render/Material.h"
#include "../Render/Shader.h"
#include "../common/AssetManager.h"
#include "../Render/StaticMesh.h"
#include "../Render/SkeletalMesh.h"
#include "../Core/Scene.h"
#include "../Render/RenderUtility.h"

class SphereGraphicComponent : public GraphicComponent
{
	ENGINE_COMPONENT_MACRO
	vec3 color;
public:
	SphereGraphicComponent() {
		color = vec3(1, 0, 0);
	}
	virtual void unitDraw(RenderParameter &param) { //Actor *actor,Engine::Scene* scene) {
		TransformComponent* trans = 0;
		if(param.actor)trans = param.actor->getComponent<TransformComponent>(ComponentId::Transform);

		//if((float)rand()/(float)RAND_MAX<0.01)
		//if(trans)Engine::debug(trans->getPos());

		glPushMatrix();
		param.scene->current_camera->MultMatrix();
		if(trans)trans->MultMatrix();

		glColor3f(color.x, color.y, color.z);
		glutSolidSphere(1, 10, 10);
		glColor3f(1, 1, 1);
		glPopMatrix();
	}
	virtual void setColor(vec3 color) {
		this->color = color;
	}
};



class MeshGraphicComponent : public GraphicComponent
{
	string mFilename;
	StaticMesh *mesh;
	static Shader* shader;
	static Shader* shader_shadow;
	mat2 textureMatrix;
	mat4 localMatrix;
	GLuint MaterialMode;
	MaterialList materials;

	ENGINE_COMPONENT_MACRO

public:
	MeshGraphicComponent(string filename);

	virtual void bindRenderState(RenderParameter &param);
	virtual void unbindRenderState(RenderParameter &param);
	virtual void unitDraw(RenderParameter &param);//Actor *actor,Engine::Scene* scene);

	virtual bool init();
	void setTextureMatrix(mat2 matrix);
	void addMaterial(Material* m);

};


class SkeletalMeshGraphicComponent : public GraphicComponent
{
	string mFilename;

	static Shader* shader;
	static Shader* shader_shadow;
	mat2 textureMatrix;
	mat4 localMatrix;
	GLuint MaterialMode;
	MaterialList materials;

	ENGINE_COMPONENT_MACRO

public:
	SkeletalMeshManager *mesh;//todo private?
	SkeletalMeshGraphicComponent(string filename) {
		mFilename = filename;
		mesh = 0;
		velocity = 0;
		flag.add("castShadow");
		init();
	}

	virtual void bindRenderState(RenderParameter &param);
	virtual void unbindRenderState(RenderParameter &param);
	virtual void unitDraw(RenderParameter &param);
	virtual void update(UpdateParameter &param) { //float deltaTimeMs,Actor *actor,Engine::Scene* scene){
		mesh->update(param.deltaTimeMs * velocity);
	}
	float velocity;
	virtual bool init() {
		//Debug("Skeletal Mesh init");
		mesh = new SkeletalMeshManager(mFilename);

		if(!mesh) {
			Engine::error(Engine::EString() + "unable to load mesh at : " + mFilename);
			Debug("unable to load mesh at : " + mFilename);
			destroy();
			return 0;
		}
		if(!shader) {

			Preprocessor *preprocessor = new Preprocessor();
			preprocessor->addVoiceToDictionary("GLSL_VERSION", int(GlobalsManager::Istance()->get("glsl_version") * 100));
			preprocessor->addVoiceToDictionary("MAX_LIGHTS", 60);
			preprocessor->addVoiceToDictionary("MAX_BONE_MATRIX", 110);

			//if(GlobalsManager::Istance()->get("glsl_version")>=3.3)
			shader = new Shader(AssetManager::getBasePath() + "Data/Shader/SkeletalMesh/shader.vs", AssetManager::getBasePath() + "Data/Shader/NormalMap.fs", preprocessor);
			//else shader=new Shader("Data/Shader/StaticMesh/Default_120/shader.vs","Data/Shader/StaticMesh/Default_120/shader.fs");
			AssetManager::addShader(shader);
			delete preprocessor;
		}
		if(!shader_shadow) {
			Preprocessor *preprocessor = new Preprocessor();
			preprocessor->addVoiceToDictionary("GLSL_VERSION", int(GlobalsManager::Istance()->get("glsl_version") * 100));
			preprocessor->addVoiceToDictionary("MAX_BONE_MATRIX", 110);
			
			shader_shadow = new Shader(AssetManager::getBasePath() + "Data/Shader/SkeletalMesh/shader.vs", AssetManager::getBasePath() + "Data/Shader/Shadow.fs", preprocessor);
			AssetManager::addShader(shader_shadow);
			delete preprocessor;
		}
		//glDebug();
		//Debug("Skeletal Mesh finish");
		return 1;
	}

	void setTextureMatrix(mat2 matrix) {
		textureMatrix = matrix;
	}
	void addMaterial(Material* m) {
		if(shader)
			m->setShader(shader, 0);
		materials.push_back(m);
	}
};





class LightComponent : public ActorComponent
{
	ENGINE_COMPONENT_MACRO
public:
	mat4 depthMVP;
	uint shadowMap, fb_shadow, rb;
	vec4 diffuse;
	vec4 specular;
	float constantAttenuation, linearAttenuation, quadraticAttenuation;
	float spotCutoff, spotExponent;
	vec3 dir;
	bool isDirectionalLight;

public:
	LightComponent() {
		shadowMap = 0;
		diffuse = vec4(1);
		specular = vec4(1);
		constantAttenuation = 0;
		linearAttenuation = 0.2;
		quadraticAttenuation = 0;
		spotCutoff = 180;
		spotExponent = 0;
		dir = vec3(0);
		isDirectionalLight = 0;
		depthMVP = mat4(1);
		flag.add("light");
	}

	virtual ComponentId::Type getComponentType(void) const {
		return ComponentId::Light;
	}

	virtual void handleEvent(Event &msg) {
		if(!m_pOwner)return;
		if(msg.getType() == componentAdded) {

		}
	}
	virtual void update(UpdateParameter &param) {
		updateFrameBuffer(param.scene);
	}

	virtual void updateUniforms(uint programId, uint numLight) {
		if(!m_pOwner)return;
		TransformComponent* trans;
		trans = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
		if(!trans)return;

		vec3 pos = trans->getLocalPos(ComponentId::Light);
		//vec3 dir=vec3(normalize(trans->getMatrix(ComponentId::Light)*vec4(0,0,1,0)));
		char s[35] = {0};
		int loc = 0;
		uint l = numLight, p = programId;
		sprintf(s, "lights[%d].position", l);
		loc = glGetUniformLocation(p, s);
		if(loc > -1) {
			float w = 1;
			if(isDirectionalLight)w = 0;
			glUniform4f(loc, pos[0], pos[1], pos[2], w);
		}

		sprintf(s, "lights[%d].diffuse", l);
		loc = glGetUniformLocation(p, s);
		if(loc > -1) {
			glUniform4fv(loc, 1, &diffuse[0]);
		}
		sprintf(s, "lights[%d].specular", l);
		loc = glGetUniformLocation(p, s);
		if(loc > -1) {
			glUniform4fv(loc, 1, &specular[0]);
		}
		sprintf(s, "lights[%d].constantAttenuation", l);
		loc = glGetUniformLocation(p, s);
		if(loc > -1) {
			glUniform1f(loc, constantAttenuation);
		}
		sprintf(s, "lights[%d].linearAttenuation", l);
		loc = glGetUniformLocation(p, s);
		if(loc > -1) {
			glUniform1f(loc, linearAttenuation);
		}
		sprintf(s, "lights[%d].quadraticAttenuation", l);
		loc = glGetUniformLocation(p, s);
		if(loc > -1) {
			glUniform1f(loc, quadraticAttenuation);
		}
		sprintf(s, "lights[%d].spotCutoff", l);
		loc = glGetUniformLocation(p, s);
		if(loc > -1) {
			glUniform1f(loc, spotCutoff);
		}
		sprintf(s, "lights[%d].spotExponent", l);
		loc = glGetUniformLocation(p, s);
		if(loc > -1) {
			glUniform1f(loc, spotExponent);
		}
		sprintf(s, "lights[%d].dir", l);
		loc = glGetUniformLocation(p, s);
		if(loc > -1) {
			glUniform3fv(loc, 1, &dir[0]);
		}
		sprintf(s, "lights[%d].DepthMVP", l);
		loc = glGetUniformLocation(p, s);
		if(loc > -1) {
			glUniformMatrix4fv(loc, 1, 0, &depthMVP[0][0]);
		}
	}

	virtual void setDirectional(bool True = true) {
		isDirectionalLight = True;
	}

	virtual bool createFrameBuffer(Scene *scene) {

		if(!scene)return 0;

		if(shadowMap)glDeleteTextures(1, &shadowMap);
		if(!(isDirectionalLight || spotCutoff <= 90))return 0;

		if(!scene->options.isShadowEnable)return 0;
		//Debug("creazione framebuffer");

		int shadow_size = (int)std::pow(2.0f, scene->options.shadowLOD);
		// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
		glGenFramebuffers(1, &fb_shadow);
		glBindFramebuffer(GL_FRAMEBUFFER, fb_shadow);


		// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
		glGenTextures(1, &shadowMap);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, shadow_size, shadow_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,shadow_size, shadow_size, 0,GL_RGB,GL_FLOAT,0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		GLfloat BorderColor[4] = {1, 1, 1, 0};
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BorderColor);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

		//	glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,shadowMap, 0);

		//glDrawBuffer(GL_NONE); // No color buffer is drawn to.
		//GLenum draw_bufs[] = { GL_NONE };
		GLenum draw_bufs[] = { GL_NONE };

		glDrawBuffers(1, draw_bufs);
		// Always check that our framebuffer is ok
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			Error("Light framebuffer error(normal light)");
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return 0;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glDebug("light framebuffer");

	}

	virtual void updateFrameBuffer(Scene* scene) {

		if(!scene)return;

		if(!scene->options.isShadowEnable || shadowMap <= 0)return;
		if(isDirectionalLight || spotCutoff < 90);
		else return;

		if(!m_pOwner)return;
		TransformComponent* trans;
		trans = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
		if(!trans)return;

		vec3 pos = trans->getLocalPos(ComponentId::Light);


		int shadow_resolution = (int)std::pow(2.0f, scene->options.shadowLOD);

		glm::mat4 depthViewMatrix;
		glm::mat4 depthProjectionMatrix;

		float shadowSize = 200;

		if(isDirectionalLight) {
			scene->current_camera->getDir();


			vec3 camPos = scene->current_camera->getPos();
			//vec3 texelSize = vec3(1.0f)/vec3(shadow_resolution);
			//camPos = floor(camPos / texelSize) * texelSize;

			vec3 center = vec3(0);
			float distFromCentroid = 500;

			for(int i = 0; i < 8; i++)center += scene->current_camera->corners[i];
			center /= 8.0f;

			vec3 texelSize = vec3(1.0f) / vec3(shadow_resolution);
			center = floor(center / texelSize) * texelSize;



			glm::vec3 light_pos = center + dir * distFromCentroid ;


			//setPos(camPos+dir*200.0f);


			glm::vec3 light_target = center;
			mat4 lightSpace = glm::lookAt(light_pos, light_target, vec3(0, 0, -1));


			vec3 max = scene->current_camera->corners[0], min = max;
			vec3 cornersLightSpace[8];

			for(int i = 0; i < 8; i++) {
				cornersLightSpace[i] = vec3(lightSpace * vec4(scene->current_camera->corners[i], 1.0f));
				if(cornersLightSpace[i].x > max.x)
					max.x = cornersLightSpace[i].x;
				else if(cornersLightSpace[i].x < min.x)
					min.x = cornersLightSpace[i].x;
				if(cornersLightSpace[i].y > max.y)
					max.y = cornersLightSpace[i].y;
				else if(cornersLightSpace[i].y < min.y)
					min.y = cornersLightSpace[i].y;
				if(cornersLightSpace[i].z > max.z)
					max.z = cornersLightSpace[i].z;
				else if(cornersLightSpace[i].z < min.z)
					min.z = cornersLightSpace[i].z;

			}



			//center=scene->camera.Pos+scene->camera.Dir*200.0f;
			//cout<<"cam far clip="<<max.z<<" "<<min.z<<"\n";


			depthViewMatrix = glm::lookAt(light_pos, light_target, vec3(0, 0, -1));
			depthProjectionMatrix = glm::ortho(min.x, max.x, min.y, max.y, -max.z, -min.z);
			//depthProjectionMatrix = glm::ortho(-shadowSize,shadowSize,-shadowSize,shadowSize,1.0f,1000.0f);
			//vec3 camPos = scene->camera.Pos;
			////vec3 texelSize = vec3(1.0f)/vec3(shadow_resolution);
			////camPos = floor(camPos / texelSize) * texelSize;

			//glm::vec3 light_pos =camPos+pos*500.0f ;//  +   pos;//scene->camera.Pos  + ldir*shadowSize/4.0f   +   pos;
			//glm::vec3 light_target=camPos;//  + ldir*shadowSize/10.0f;
			////glm::vec3 light_pos =scene->camera.Pos+pos;
			////glm::vec3 light_target=scene->camera.Pos;
			//glm::vec3 up_dir (0.0f, 0.0f, -1.0f);
			//depthViewMatrix = glm::lookAt (light_pos, light_target, up_dir);
			//glm::mat4 depthProjectionMatrix = glm::ortho(-shadowSize,shadowSize,-shadowSize,shadowSize,1.0f,1000.0f);//glm::frustum(-1.0f,1.0f,-1.0f,1.0f,1.0f,100.0f);//glm::perspective (fov, aspect, Near, Far);

			depthMVP = depthProjectionMatrix * depthViewMatrix;
		}
		if(spotCutoff <= 90) {
			//cout<<"spot..\n";
			depthViewMatrix = glm::lookAt (pos, pos + dir, vec3(0, 1, 0));
			depthProjectionMatrix = glm::perspective(spotCutoff * 2, 1.0f, 1.0f, 100.0f); //glm::frustum(-1.0f,1.0f,-1.0f,1.0f,1.0f,100.0f);//glm::perspective (fov, aspect, Near, Far);
			depthMVP = depthProjectionMatrix * depthViewMatrix;
		}
		// create a projection matrix for the shadow caster



		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform

		//scene->dephtShader->Use();
		//scene->depthMVP=depthMVP;

		//Camera cam;
		//cam.projection=depthProjectionMatrix;
		//cam.setMatrix(depthViewMatrix);
		//cam.mode=Camera::Matrix;
		//cam.UpdatePlanes();

		//scene->renderFlags=Scene::Shadow;
		//scene->ShadowCameras.clear();
		//scene->ShadowCameras.push_back(cam);

		//glUniformMatrix4fv(glGetUniformLocation(scene->dephtShader->getProgramId(),"depthMVP"), 1, GL_FALSE, &depthMVP[0][0]);


		Camera old = *scene->current_camera;
		scene->current_camera->setMode(Camera::Matrix);
		scene->current_camera->setMatrix(mat4(1));
		scene->current_camera->projection = depthMVP; //mat4(1);



		glBindFramebuffer(GL_FRAMEBUFFER, fb_shadow);
		// set the viewport to the size of the shadow map
		glViewport(0, 0, shadow_resolution, shadow_resolution);
		// clear the shadow map to black (or white)
		glClearColor(0.0, 0.0, 0.0, 0.0);
		// no need to clear the colour buffer
		glClear(GL_DEPTH_BUFFER_BIT);

		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);

		scene->Draw(RenderFlag::Shadow);
		// bind the default framebuffer again
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//glDisable(GL_CULL_FACE);
		glActiveTexture(0);


		*scene->current_camera = old;
		//glDebug("light framebuffer");
	}

};



class DirectionalLightComponent_old : public LightComponent
{
public:
	vector<mat4> depthMVP;
	vector<uint> shadowMap, fb_shadow;
	uint frameBuffer, rb;
	int numCascades;
	vec4 splitDepths;

	DirectionalLightComponent_old(): LightComponent() {
		flag.add("directionalLight");
		numCascades = 4;
		shadowMap.resize(numCascades);
		for(uint i = 0; i < numCascades; i++)shadowMap[i] = 0;
		depthMVP.resize(numCascades);
		fb_shadow.resize(numCascades);
	}

	virtual void updateUniforms(uint programId, uint numLight) {

		char s[35] = {0};
		int loc = 0;
		uint l = numLight, p = programId;
		sprintf(s, "dirLights[%d].dir", 0);
		loc = glGetUniformLocation(p, s);
		if(loc > -1) {
			float w = 1;
			if(isDirectionalLight)w = 0;
			glUniform4f(loc, dir[0], dir[1], dir[2], w);
		}

		sprintf(s, "dirLights[%d].diffuse", 0);
		loc = glGetUniformLocation(p, s);
		if(loc > -1) {
			glUniform4fv(loc, 1, &diffuse[0]);
		}
		sprintf(s, "dirLights[%d].specular", 0);
		loc = glGetUniformLocation(p, s);
		if(loc > -1) {
			glUniform4fv(loc, 1, &specular[0]);
		}

		sprintf(s, "dirLights[%d].split", 0);
		loc = glGetUniformLocation(p, s);
		if(loc > -1) {
			glUniform4fv(loc, 1, &splitDepths[0]);
		}

	}
	virtual bool createFrameBuffer(Scene *scene) {

		for(uint i = 0; i < numCascades; i++) {
			if(shadowMap[i])glDeleteTextures(1, &shadowMap[i]);
			//if(isDirectionalLight||spotCutoff<90)return 0;

			if(!scene->options.isShadowEnable)return 0;
			int shadow_size = (int)std::pow(2.0f, scene->options.shadowLOD);
			// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
			glGenFramebuffers(1, &fb_shadow[i]);
			glBindFramebuffer(GL_FRAMEBUFFER, fb_shadow[i]);

			glGenTextures(1, &shadowMap[i]);
			glActiveTexture (GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, shadowMap[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, shadow_size, shadow_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,shadow_size, shadow_size, 0,GL_RGB,GL_FLOAT,0);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

			glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap[i], 0);

			//	glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,shadowMap, 0);

			//glDrawBuffer(GL_NONE); // No color buffer is drawn to.
			GLenum draw_bufs[] = { GL_NONE };
			//GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0 };

			glDrawBuffers (1, draw_bufs);
			// Always check that our framebuffer is ok
			if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				Error("\n\nLight framebuffer error\n");
				glBindFramebuffer (GL_FRAMEBUFFER, 0);
				return 0;
			}
		}
		glBindFramebuffer (GL_FRAMEBUFFER, 0);
		//glDebug("directional light framebuffer");
	}
	virtual void updateFrameBuffer(Scene *scene) {
		glDebug();
		if(!scene->options.isShadowEnable)return;
		int shadow_resolution = (int)std::pow(2.0f, scene->options.shadowLOD);

		//float level[4] = {0.05, 0.1, 0.3, 1};
		float level[4] = {0.03, 0.08, 0.2, 1};

		glm::mat4 depthViewMatrix;
		glm::mat4 depthProjectionMatrix;
		float shadowSize = 200;
		vec3 camPos = scene->current_camera->getPos();

		//setPos(camPos + dir * 200.0f);
		scene->current_camera->UpdatePlanes();

		Camera old = *scene->current_camera;
		scene->current_camera->setMode(Camera::Matrix);
		scene->current_camera->setMatrix(mat4(1));
		glDebug();
		for(int c = 0; c < numCascades; c++) {




			//vec3 texelSize = vec3(1.0f)/vec3(shadow_resolution);
			//camPos = floor(camPos / texelSize) * texelSize;

			vec3 center = vec3(0);
			float distFromCentroid = 500;


			vec3 cornersCSM[8];
			for(int i = 0; i < 8; i++)cornersCSM[i] = scene->current_camera->corners[i];

			for(int i = 4; i < 8; i++)cornersCSM[i] = mix(scene->current_camera->corners[i - 4], scene->current_camera->corners[i], level[c]);

			if(c)
				for(int i = 0; i < 4; i++)cornersCSM[i] = mix(scene->current_camera->corners[i], scene->current_camera->corners[i + 4], level[c - 1]);


			center = vec3(0);

			for(int i = 0; i < 8; i++)center += cornersCSM[i];
			center /= 8.0f;

			//vec3 texelSize = vec3(1.0f)/vec3(shadow_resolution);
			//center = floor(center / texelSize) * texelSize;

			glm::vec3 light_pos = center + dir * distFromCentroid ;


			glm::vec3 light_target = center;
			mat4 lightSpace = glm::lookAt (light_pos, light_target, vec3(0, 0, -1));


			vec3 max = scene->current_camera->corners[0], min = max;
			vec3 cornersLightSpace[8];

			for(int i = 0; i < 8; i++) {
				cornersLightSpace[i] = vec3(lightSpace * vec4(cornersCSM[i], 1.0f));
				if (cornersLightSpace[i].x > max.x)
					max.x = cornersLightSpace[i].x;
				else if (cornersLightSpace[i].x < min.x)
					min.x = cornersLightSpace[i].x;
				if (cornersLightSpace[i].y > max.y)
					max.y = cornersLightSpace[i].y;
				else if (cornersLightSpace[i].y < min.y)
					min.y = cornersLightSpace[i].y;
				if (cornersLightSpace[i].z > max.z)
					max.z = cornersLightSpace[i].z;
				else if (cornersLightSpace[i].z < min.z)
					min.z = cornersLightSpace[i].z;

			}
			depthViewMatrix = glm::lookAt (light_pos, light_target, vec3(0, 0, -1));
			depthProjectionMatrix = glm::ortho(min.x, max.x, min.y, max.y, -max.z, -min.z);


			depthMVP[c] = depthProjectionMatrix * depthViewMatrix;


			//scene->depthMVP = depthMVP[c];

			//glUniformMatrix4fv(glGetUniformLocation(scene->dephtShader->getProgramId(), "depthMVP"), 1, GL_FALSE, &depthMVP[c][0][0]);
			glEnable(GL_CULL_FACE);

			glDebug();
			glViewport (0, 0, shadow_resolution, shadow_resolution);
			glBindFramebuffer (GL_FRAMEBUFFER, fb_shadow[c]);
			glClearColor (0.0, 0.0, 0.0, 0.0);
			glClear (GL_DEPTH_BUFFER_BIT);


			scene->current_camera->projection = depthMVP[c]; //mat4(1);


			glDebug();
			scene->Draw(RenderFlag::Shadow);
			glDebug();
		}
		glDebug();
		// bind the default framebuffer again
		glBindFramebuffer (GL_FRAMEBUFFER, 0);

		*scene->current_camera = old;
		glDebug("directional light framebuffer");
	}
};

class DirectionalLightComponent : public LightComponent
{
public:
	vector<mat4> depthMVP;
	vector<FrameBufferDepth> fbuffers;
	//vector<BlurKernel> blurs;
	static Shader *blurShader;
	uint frameBuffer, rb;
	int numCascades;
	vec4 splitDepths;

	DirectionalLightComponent(): LightComponent() {
		flag.add("directionalLight");
		numCascades = 4;
		depthMVP.resize(numCascades);
		fbuffers.resize(numCascades+1);
		if(!blurShader){
					blurShader=new Shader(AssetManager::getBasePath() + "Data/Shader/Blur/shader.vs", AssetManager::getBasePath() + "Data/Shader/Blur/shader.fs");
					AssetManager::addShader(blurShader);
		}
	//	blurs.resize(numCascades);
		
	}
	
	virtual void updateUniforms(uint programId, uint numLight) {

		char s[35] = {0};
		int loc = 0;
		uint l = numLight, p = programId;
		sprintf(s, "dirLights[%d].dir", 0);
		loc = glGetUniformLocation(p, s);
		if(loc > -1) {
			float w = 1;
			if(isDirectionalLight)w = 0;
			glUniform4f(loc, dir[0], dir[1], dir[2], w);
		}

		sprintf(s, "dirLights[%d].diffuse", 0);
		loc = glGetUniformLocation(p, s);
		if(loc > -1) {
			glUniform4fv(loc, 1, &diffuse[0]);
		}
		sprintf(s, "dirLights[%d].specular", 0);
		loc = glGetUniformLocation(p, s);
		if(loc > -1) {
			glUniform4fv(loc, 1, &specular[0]);
		}

		sprintf(s, "dirLights[%d].split", 0);
		loc = glGetUniformLocation(p, s);
		if(loc > -1) {
			glUniform4fv(loc, 1, &splitDepths[0]);
		}

	}
	virtual bool createFrameBuffer(Scene *scene) {

		for(uint i = 0; i < fbuffers.size(); i++) {
			if(!scene->options.isShadowEnable)return 0;
			int shadow_size = (int)std::pow(2.0f, scene->options.shadowLOD);
			fbuffers[i].resetFrameBuffer(vec2(shadow_size));
		//	blurs[i].setSize(vec2(shadow_size));
		}
		glBindFramebuffer (GL_FRAMEBUFFER, 0);
		glDebug("directional light framebuffer");
	}
	virtual void updateFrameBuffer(Scene *scene) {
		glDebug();
		if(!scene->options.isShadowEnable)return;
		int shadow_resolution = (int)std::pow(2.0f, scene->options.shadowLOD);

		//float level[4] = {0.05, 0.1, 0.3, 1};
		float level[4] = {0.03, 0.08, 0.2, 1};

		glm::mat4 depthViewMatrix;
		glm::mat4 depthProjectionMatrix;
		float shadowSize = 200;
		vec3 camPos = scene->current_camera->getPos();

		//setPos(camPos + dir * 200.0f);
		scene->current_camera->UpdatePlanes();

		Camera old = *scene->current_camera;
		scene->current_camera->setMode(Camera::Matrix);
		scene->current_camera->setMatrix(mat4(1));
		glDebug();
		for(int c = 0; c < numCascades; c++) {

			//vec3 texelSize = vec3(1.0f)/vec3(shadow_resolution);
			//camPos = floor(camPos / texelSize) * texelSize;

			vec3 center = vec3(0);
			float distFromCentroid = 500;


			vec3 cornersCSM[8];
			for(int i = 0; i < 8; i++)cornersCSM[i] = scene->current_camera->corners[i];

			for(int i = 4; i < 8; i++)cornersCSM[i] = mix(scene->current_camera->corners[i - 4], scene->current_camera->corners[i], level[c]);

			if(c)
				for(int i = 0; i < 4; i++)cornersCSM[i] = mix(scene->current_camera->corners[i], scene->current_camera->corners[i + 4], level[c - 1]);


			center = vec3(0);

			for(int i = 0; i < 8; i++)center += cornersCSM[i];
			center /= 8.0f;

			//vec3 texelSize = vec3(1.0f)/vec3(shadow_resolution);
			//center = floor(center / texelSize) * texelSize;

			glm::vec3 light_pos = center + dir * distFromCentroid ;


			glm::vec3 light_target = center;
			mat4 lightSpace = glm::lookAt (light_pos, light_target, vec3(0, 0, -1));


			vec3 max = scene->current_camera->corners[0], min = max;
			vec3 cornersLightSpace[8];

			for(int i = 0; i < 8; i++) {
				cornersLightSpace[i] = vec3(lightSpace * vec4(cornersCSM[i], 1.0f));
				if (cornersLightSpace[i].x > max.x)
					max.x = cornersLightSpace[i].x;
				else if (cornersLightSpace[i].x < min.x)
					min.x = cornersLightSpace[i].x;
				if (cornersLightSpace[i].y > max.y)
					max.y = cornersLightSpace[i].y;
				else if (cornersLightSpace[i].y < min.y)
					min.y = cornersLightSpace[i].y;
				if (cornersLightSpace[i].z > max.z)
					max.z = cornersLightSpace[i].z;
				else if (cornersLightSpace[i].z < min.z)
					min.z = cornersLightSpace[i].z;

			}
			depthViewMatrix = glm::lookAt (light_pos, light_target, vec3(0, 0, -1));
			depthProjectionMatrix = glm::ortho(min.x, max.x, min.y, max.y, -max.z, -min.z);


			depthMVP[c] = depthProjectionMatrix * depthViewMatrix;


			glEnable(GL_CULL_FACE);

			glDebug();
			glViewport (0, 0, shadow_resolution, shadow_resolution);
		
			fbuffers[0].bind();
			glClearColor (0.0, 0.0, 0.0, 0.0);
			glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			glViewport (0, 0, shadow_resolution, shadow_resolution);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0.0, 256.0, 0.0, 256.0, -1.0, 1.0); 
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();


			scene->current_camera->projection = depthMVP[c]; //mat4(1);


			glDebug();
			scene->Draw(RenderFlag::Shadow);
			glDebug();
			
			
			
			glDisable(GL_CULL_FACE);
			
			fbuffers[1].bind();
			glClearColor (0.0, 0.0, 0.0, 0.0);
			glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			
			blurShader->Use();
			
			glUniform1i(glGetUniformLocation(blurShader->getProgramId(),"inputTexture"),0);
			glUniform1f(glGetUniformLocation(blurShader->getProgramId(),"BlurFactor"),0.0023);
			glUniform1i(glGetUniformLocation(blurShader->getProgramId(),"axis"),1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,fbuffers[0].getTextureId());
			
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
			
			fbuffers[(c+2)%fbuffers.size()].bind();
			glClearColor (0.0, 0.0, 0.0, 0.0);
			glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			
			glUniform1i(glGetUniformLocation(blurShader->getProgramId(),"axis"),0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,fbuffers[1].getTextureId());
			
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
			
/*
			fbuffers[c+1].bind();//blur
			glClearColor (0.0, 0.0, 0.0, 0.0);
			glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			glViewport (0, 0, shadow_resolution, shadow_resolution);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0.0, 256.0, 0.0, 256.0, -1.0, 1.0); 
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			//glBindFramebuffer (GL_FRAMEBUFFER, fb_shadow[c]);
			
			blurShader->Use();
			
			glUniform1i(glGetUniformLocation(blurShader->getProgramId(),"inputTexture"),0);
			glUniform1i(glGetUniformLocation(blurShader->getProgramId(),"inputTexture"),0);
			glUniform1f(glGetUniformLocation(blurShader->getProgramId(),"BlurFactor"),0.0023);
			glUniform1i(glGetUniformLocation(blurShader->getProgramId(),"axis"),1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,fbuffers[0].getTextureId());
			
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
			//*/
			//fbuffers[c].unbind();
			//blurs[c].blurTexture(fbuffers[c].getTextureId(),vec2(0.005));
		}
		glDebug();
		// bind the default framebuffer again
		glBindFramebuffer (GL_FRAMEBUFFER, 0);

		*scene->current_camera = old;
		glDebug("directional light framebuffer");
	}
	unsigned int getTextureId(int numCascade=0){
		//if(numCascade<0||numCascade>=fbuffers.size())return 0;
		//return blurs[numCascade].getTextureId();
		return fbuffers[(numCascade+2)%fbuffers.size()].getTextureId();
	}
};
