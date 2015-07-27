#include "RenderComponent.h"
#include "../Render/RenderFlag.h"
#include "../common/Hash.h"

Shader* MeshGraphicComponent::shader = 0;
Shader* SkeletalMeshGraphicComponent::shader = 0;
Shader* MeshGraphicComponent::shader_shadow = 0;
Shader* SkeletalMeshGraphicComponent::shader_shadow = 0;
Shader* DirectionalLightComponent::blurShader = 0;

MeshGraphicComponent::MeshGraphicComponent(string filename)
{
	glDebug();
	mFilename = filename;
	mesh = 0;
	//shader = 0;
	flag.add("castShadow");
	init();
}

void MeshGraphicComponent::bindRenderState(RenderParameter &param)
{
	glDebug();
	if(param.flag & RenderFlag::Default) {
		if(shader) {
			shader->Use();
			glDebug(); 
		}
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
	} else if(param.flag & RenderFlag::Shadow) {
		if(shader_shadow) {
			shader_shadow->Use();
			glDebug();
		}
		glEnableVertexAttribArray(0);
	}
}
void MeshGraphicComponent::unbindRenderState(RenderParameter &param)
{
	Shader* currentShader = 0;
	if(param.flag & RenderFlag::Default)currentShader = shader;
	else if(param.flag & RenderFlag::Shadow)currentShader = shader_shadow;
	if(currentShader)currentShader->Use();
	
	if(param.flag & RenderFlag::Default) {
		
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
	} else if(param.flag & RenderFlag::Shadow) {

	}
	glEnableVertexAttribArray(0);

}
void MeshGraphicComponent::unitDraw(RenderParameter &param)//Actor *actor,Engine::Scene* scene)
{
	glDebug();
	TransformComponent* trans = 0;
	if(m_pOwner)trans = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);

	//glPushMatrix();
	glColor3f(0, 1, 0);


	Shader* currentShader = 0;
	if(param.flag & RenderFlag::Default)currentShader = shader;
	else if(param.flag & RenderFlag::Shadow)currentShader = shader_shadow;

	
	if(currentShader) {
		glDebug();

		int loc;
		glDebug();
		if(trans) {
			loc=currentShader->getUniformLocation("MVM");
			if(loc>=0)glUniformMatrix4fv(loc, 1, 0, &trans->getMatrix()[0][0]);
		}
		glDebug(param.flag);
		loc=currentShader->getUniformLocation("textureMatrix");
		if(param.flag & RenderFlag::Default)if(loc>-1)glUniformMatrix2fv(loc, 1, 0, &textureMatrix[0][0]);

		glDebug(param.flag);
		
		mesh->draw(materials, currentShader,param.flag);
		glDebug();

	}


	//glPopMatrix();
}
 

bool MeshGraphicComponent::init()
{
	textureMatrix = mat2(1);

	glDebug();
	if(!shader) {
		Preprocessor *preprocessor = new Preprocessor();
		preprocessor->addVoiceToDictionary("GLSL_VERSION", int(GlobalsManager::Istance()->get("glsl_version") * 100));
		preprocessor->addVoiceToDictionary("MAX_LIGHTS", 10);

		//if(GlobalsManager::Istance()->get("glsl_version")>=3.3)
		shader = new Shader(AssetManager::getBasePath() + "Data/Shader/StaticMesh/Default/shader.vs", AssetManager::getBasePath() + "Data/Shader/NormalMap.fs", preprocessor);
		//else shader=new Shader("Data/Shader/StaticMesh/Default_120/shader.vs","Data/Shader/StaticMesh/Default_120/shader.fs");
		AssetManager::addShader(shader);
		delete preprocessor;
	}
	if(!shader_shadow) {
		Preprocessor *preprocessor = new Preprocessor();
		preprocessor->addVoiceToDictionary("GLSL_VERSION", int(GlobalsManager::Istance()->get("glsl_version") * 100));

		shader_shadow = new Shader(AssetManager::getBasePath() + "Data/Shader/StaticMesh/Shadow/shader.vs", AssetManager::getBasePath() + "Data/Shader/Shadow.fs", preprocessor);
		AssetManager::addShader(shader_shadow);
		delete preprocessor;
	}
	mesh = AssetManager::addStaticMesh(mFilename);
	glDebug();
	if(!mesh) {
		Engine::error(Engine::EString() + "unable to load mesh at : " + mFilename);
		destroy();
		return 0;
	}

	return 1;
}
void MeshGraphicComponent::setTextureMatrix(mat2 matrix)
{
	textureMatrix = matrix;
}
void MeshGraphicComponent::addMaterial(Material* m)
{
	if(shader)
		m->setShader(shader,0);
	materials.push_back(m);
}









void SkeletalMeshGraphicComponent::bindRenderState(RenderParameter &param)
{
	glDebug();
	if(param.flag & RenderFlag::Default) {
		if(shader) {
			shader->Use();
			glDebug();
		}
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);
	} else if(param.flag & RenderFlag::Shadow) {
		if(shader_shadow) {
			shader_shadow->Use();
			glDebug();
		}
		glEnableVertexAttribArray(0);
	}
}
void SkeletalMeshGraphicComponent::unbindRenderState(RenderParameter &param)
{
	Shader* currentShader = 0;
	if(param.flag & RenderFlag::Default)currentShader = shader;
	else if(param.flag & RenderFlag::Shadow)currentShader = shader_shadow;
	if(currentShader)currentShader->Use();
	
	if(param.flag & RenderFlag::Default) {
		
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);
		glDisableVertexAttribArray(5);
	} else if(param.flag & RenderFlag::Shadow) {

	}
	glEnableVertexAttribArray(0);

}
void SkeletalMeshGraphicComponent::unitDraw(RenderParameter &param)//Actor *actor,Engine::Scene* scene)
{

	glDebug();
	TransformComponent* trans = 0;
	if(m_pOwner)trans = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);

	//glPushMatrix();
	glColor3f(0, 1, 0);


	Shader* currentShader = 0;
	if(param.flag & RenderFlag::Default)currentShader = shader;
	else if(param.flag & RenderFlag::Shadow)currentShader = shader_shadow;

	
	if(currentShader) {
		currentShader->Use();
		glDebug();
		GLuint p = currentShader->getProgramId();
		int loc;
		glDebug();
		if(trans) {
			loc=currentShader->getUniformLocation("MVM");
			if(loc>=0)glUniformMatrix4fv(loc, 1, 0, &trans->getMatrix()[0][0]);
			
		}
		glDebug(param.flag);
		loc=currentShader->getUniformLocation("textureMatrix");
		if(param.flag & RenderFlag::Default)if(loc>-1)glUniformMatrix2fv(loc, 1, 0, &textureMatrix[0][0]);

		
		
		glDebug(param.flag);
		mesh->updateUniform(currentShader->getProgramId());
		mesh->draw(materials, currentShader,param.flag);
		glDebug();

	}


	//glPopMatrix();
}