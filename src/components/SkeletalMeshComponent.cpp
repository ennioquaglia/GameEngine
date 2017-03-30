#include "SkeletalMeshComponent.h"
#include "Scene.h"
#include "Hash.h"
#include "Debug.h"
#include "TransformComponent.h"
#include "Preprocessor.h"
#include "Shader.h"
#include "Material.h"
#include "AssimpScene.h"
#include "LightComponent.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Engine {


//####// SkeletalMeshSystem //####//

void SkeletalMeshSystem::Init(){

    //QENGINE_INFO("StaticMeshSystem::Init()");

    m_DefaultShader=new Shader("::Data/Shader/SkeletalMesh/shader.vs","::Data/Shader/NormalMap.fs",EString()+"#define GLSL_VERSION "+330+"\n#define MAX_LIGHTS "+ 16+"\n#define MAX_BONE_MATRIX "+ 110);
    Shader::Compile(m_DefaultShader);
    if(!*m_DefaultShader)QENGINE_ERROR("m_DefaultShader shader error");

    m_ShadowShader=new Shader("::Data/Shader/SkeletalMesh/shader.vs", "::Data/Shader/Shadow.fs",EString()+"#define GLSL_VERSION "+330+"\n#define MAX_LIGHTS "+ 16+"\n#define MAX_BONE_MATRIX "+ 110);
    Shader::Compile(m_ShadowShader);
    if(!*m_ShadowShader)QENGINE_ERROR("m_ShadowShader shader error");

    m_DeferredShader=new Shader("::Data/Shader/SkeletalMesh/shader.vs", "::Data/Shader/Deferred/GPass/shader.fs",EString()+"#define GLSL_VERSION "+330+"\n#define MAX_LIGHTS "+ 16+"\n#define MAX_BONE_MATRIX "+ 110);
    Shader::Compile(m_DeferredShader);
    if(!*m_DeferredShader)QENGINE_ERROR("m_DeferredShader shader error");
}


void SkeletalMeshSystem::Update(UpdateParameter &param){
    for (SkeletalMeshComponent *c : m_Components) c->Update(param);
    //QENGINE_INFO("update" + m_Components.size());
}


void drawSkeleton(const aiNode *node, std::unordered_map<std::string, int>& bone_name_hash, const glm::mat4 &T, std::vector<glm::mat4> &bone_transforms)
{
    glm::mat4 current_node_transform = T;
    if(&(node->mName) != NULL) {
        std::unordered_map<std::string, int>::iterator it = bone_name_hash.find(node->mName.data);
        if(it != bone_name_hash.end() && it->second >= 0) {
            glBegin(GL_LINES);
            glm::vec4 pos=current_node_transform*glm::vec4(0,0,0,1);
            glVertex3fv(&pos[0]);
            current_node_transform = current_node_transform * bone_transforms[it->second];

            pos=current_node_transform*glm::vec4(0,0,0,1);
            glVertex3fv(&pos[0]);
            glEnd();
            bone_transforms[it->second] = current_node_transform;
        }
    }
    for(unsigned int i = 0; i < node->mNumChildren; ++i)
        drawSkeleton(node->mChildren[i], bone_name_hash, current_node_transform, bone_transforms);
}


void SkeletalMeshSystem::Draw(RenderParameter &param) {

    int loc;
    Camera camera=param.scene->getCamera();


    Shader *shader=0;

    if(param.flag & RenderFlag::Default){
        shader=m_DefaultShader;
    }
    if(param.flag & RenderFlag::Shadow){
        shader=m_ShadowShader;
    }
    if(param.flag & RenderFlag::Deferred){
        shader=m_DeferredShader;
    }


    if(shader){
        shader->Use();

        shader->setUniform("PM",1,0,camera.getProjectionMatrix());
        shader->setUniform("CM",1,0,camera.getMatrix());
        shader->setUniform("parallax",param.scene->options.useParalax);

        if(param.flag & RenderFlag::Default){
            LightSystem::updateUniforms(shader,const_cast<Scene*>(param.scene));
        }

        loc=shader->getUniformLocation("MVM");
        if(loc<0)QENGINE_ERROR("MVM uniform not found");
        else
        {
            for (SkeletalMeshComponent *c : m_Components) {

                TransformComponent *trans=c->getEntity()->getObjectByType<TransformComponent>();

                glUniformMatrix4fv(loc, 1, 0, &trans->getMatrix()[0][0]);

                shader->setUniform("textureMatrix",1,0,c->m_TextureMatrix);

                Material::setupShaderUniform(shader);

                c->updateUniform(shader->getProgramId());

                if(c->m_MaterialList.empty()){
                    Material::getDefaultMaterial().updateShader(shader);
                    Material::getDefaultMaterial().bind();
                    for( auto vao : c->m_MeshData->VAOs){
                        vao.draw();
                    }
                }else{

                    MaterialList::iterator it=c->m_MaterialList.begin();

                    for( auto vao : c->m_MeshData->VAOs){
                        if(it!=c->m_MaterialList.end()){
                            (*it).updateShader(shader);
                            (*it).bind();
                            ++it;
                        }
                        vao.draw();
                    }
                }
            }
            glUseProgram(0);
            glDisable(GL_DEPTH_TEST);
            for (SkeletalMeshComponent *c : m_Components) {
                TransformComponent *trans=c->getEntity()->getObjectByType<TransformComponent>();

                glPushMatrix();
                glMultMatrixf( &param.scene->getCamera().getMatrix()[0][0]);
                glMultMatrixf( &trans->getMatrix()[0][0]);

                glm::mat4 mat;
                drawSkeleton(c->m_MeshData->MeshScene->mRootNode,c->m_MeshData->bone_name_hash,mat,c->bone_transforms_pre);


                glPopMatrix();

            }
            glEnable(GL_DEPTH_TEST);
        }
    }
}

#if 0
void SkeletalMeshSystem::Draw(RenderParameter &param) {

    int loc;
    Camera camera=param.scene->getCamera();


    Shader *shader=0;

    if(param.flag & RenderFlag::Default){
        shader=m_DefaultShader;
    }
    if(param.flag & RenderFlag::Shadow){
        shader=m_ShadowShader;
    }
    if(param.flag & RenderFlag::Deferred){
        shader=m_DeferredShader;
    }


    if(shader){
        shader->Use();

        shader->setUniform("PM",1,0,camera.getProjectionMatrix());
        shader->setUniform("CM",1,0,camera.getMatrix());
        shader->setUniform("parallax",param.scene->options.useParalax);

        if(param.flag & RenderFlag::Default){
            LightSystem::updateUniforms(shader,const_cast<Scene*>(param.scene));
        }

        loc=shader->getUniformLocation("MVM");
        if(loc<0)QENGINE_ERROR("MVM uniform not found");
        else
            for (SkeletalMeshComponent *c : m_Components) {

                TransformComponent *trans=c->getEntity()->getObjectByType<TransformComponent>();

                glUniformMatrix4fv(loc, 1, 0, &trans->getMatrix()[0][0]);

                shader->setUniform("textureMatrix",1,0,c->m_TextureMatrix);

                Material::setupShaderUniform(shader);

                c->updateUniform(shader->getProgramId());

                if(c->m_MaterialList.empty()){
                    Material::getDefaultMaterial().updateShader(shader);
                    Material::getDefaultMaterial().bind();
                    for( auto vao : c->m_MeshData->VAOs){
                        vao.draw();
                    }
                }else{

                    MaterialList::iterator it=c->m_MaterialList.begin();

                    for( auto vao : c->m_MeshData->VAOs){
                        if(it!=c->m_MaterialList.end()){
                            (*it).updateShader(shader);
                            (*it).bind();
                            ++it;
                        }
                        vao.draw();
                    }
                }
            }
        glUseProgram(0);
    }
}
#endif
void SkeletalMeshSystem::addComponent(Object * obj)
{
    if (!obj)return;

    //QENGINE_INFO("Trying to add a Static Mesh");
    if (obj->getType() != SkeletalMeshComponent::getTypeStatic())return;
    SkeletalMeshComponent *c = (SkeletalMeshComponent*)obj;

    m_Components.push_back(c);

    QENGINE_INFO("Added a Skeletal Mesh! "+c->m_Filename);
}





//####// SkeletalMeshComponent //####//

SkeletalMeshComponent::SkeletalMeshComponent(PathName filename) :m_Filename(filename)
{
    m_MeshData=SkeletalMeshData::load(filename);
    Init();
}

SkeletalMeshComponent::~SkeletalMeshComponent()
{
}

void SkeletalMeshComponent::Init()
{
    addAnimation(idle,0,1000);
    int bonesNumber = 60;
    bone_transforms.resize(bonesNumber);
    bone_transforms_pre.resize(bonesNumber);

    m_bone_positions.resize(bonesNumber);
    m_bone_rotations.resize(bonesNumber);


//    for(unsigned int i=0;i<bone_transforms.size();++i){
//        bone_transforms[i]=glm::mat4(1);
//    }
}

void SkeletalMeshComponent::OnScene(Scene &scene)
{
    Object* system = scene.getObjectByType<SkeletalMeshSystem>();
    if (system)system->addComponent(this);
    QENGINE_INFO("SkeletalMeshComponent::OnScene");
}
void SkeletalMeshComponent::OnAttached(Entity &e)
{
    //QENGINE_INFO("SkeletalMeshComponent::onAttached");
    if(!e.getObjectByType<TransformComponent>()){
        QENGINE_WARN("SkeletalMeshComponent::OnAttached entity requires TransformComponent ->added");
        e.addComponent(new TransformComponent());
    }
}

void SkeletalMeshComponent::setTextureMatrix(glm::mat2 matrix){
    m_TextureMatrix=matrix;
}

void SkeletalMeshComponent::addMaterial(Material mat){
    m_MaterialList.push_back(mat);
}
void SkeletalMeshComponent::setMaterial(Material mat,unsigned int index){
    if(index<m_MaterialList.size())
        m_MaterialList.assign(index,mat);
}
void SkeletalMeshComponent::removeMaterial(unsigned int index){
    MaterialList::iterator it = std::next(m_MaterialList.begin(), index);
    m_MaterialList.erase(it);
}
Material& SkeletalMeshComponent::getMaterial(unsigned int index){
    if(index<m_MaterialList.size()){
        MaterialList::iterator it = std::next(m_MaterialList.begin(), index);
        return *it;
    }
    if(m_MaterialList.size())return *m_MaterialList.rbegin();
    return Material::getDefaultMaterial();
}






//###// SkeletalMeshComponent //###//


aiQuaternion lerp(aiQuaternion a, aiQuaternion b, float t)
{
    aiQuaternion c;
    c.w = a.w + (b.w - a.w) * t;
    c.x = a.x + (b.x - a.x) * t;
    c.y = a.y + (b.y - a.y) * t;
    c.z = a.z + (b.z - a.z) * t;
    return c;
}
aiVector3D lerp(aiVector3D a, aiVector3D b, float t)
{
    aiVector3D c;
    c.x = a.x + (b.x - a.x) * t;
    c.y = a.y + (b.y - a.y) * t;
    c.z = a.z + (b.z - a.z) * t;
    return c;
}
inline void copyAiMatrixToGLM(const aiMatrix4x4 *from, glm::mat4 &to)
{
    to[0][0] = (GLfloat)from->a1;
    to[1][0] = (GLfloat)from->a2;
    to[2][0] = (GLfloat)from->a3;
    to[3][0] = (GLfloat)from->a4;
    to[0][1] = (GLfloat)from->b1;
    to[1][1] = (GLfloat)from->b2;
    to[2][1] = (GLfloat)from->b3;
    to[3][1] = (GLfloat)from->b4;
    to[0][2] = (GLfloat)from->c1;
    to[1][2] = (GLfloat)from->c2;
    to[2][2] = (GLfloat)from->c3;
    to[3][2] = (GLfloat)from->c4;
    to[0][3] = (GLfloat)from->d1;
    to[1][3] = (GLfloat)from->d2;
    to[2][3] = (GLfloat)from->d3;
    to[3][3] = (GLfloat)from->d4;
}
glm::mat4 mat4_mix(glm::mat4 a,glm::mat4 b,float alpha){
    glm::mat4 out;
    for(int x=0;x<4;x++){
        for(int y=0;y<4;y++){
            out[x][y]=glm::mix(a[x][y],b[x][y],alpha);
        }
    }
    return out;
}

void buildNodeHierarchy(const aiNode *node, std::unordered_map<std::string, int>& bone_name_hash, const glm::mat4 &T, std::vector<glm::mat4> &bone_transforms)
{
    glm::mat4 current_node_transform = T;
    if(&(node->mName) != NULL) {
        std::unordered_map<std::string, int>::iterator it = bone_name_hash.find(node->mName.data);
        if(it != bone_name_hash.end() && it->second >= 0) {
            current_node_transform = current_node_transform * bone_transforms[it->second];
            bone_transforms[it->second] = current_node_transform;
        }
    }
    for(unsigned int i = 0; i < node->mNumChildren; ++i)
        buildNodeHierarchy(node->mChildren[i], bone_name_hash, current_node_transform, bone_transforms);
}


void buildNodeHierarchy(const aiNode *node, std::unordered_map<std::string, int>& bone_name_hash, const glm::quat &T, std::vector<glm::quat> &bone_rotations)
{
    glm::quat current_node_transform = T;
    if(&(node->mName) != NULL) {
        std::unordered_map<std::string, int>::iterator it = bone_name_hash.find(node->mName.data);
        if(it != bone_name_hash.end() && it->second >= 0) {
            current_node_transform = current_node_transform * bone_rotations[it->second];
            bone_rotations[it->second] = current_node_transform;
        }
    }
    for(unsigned int i = 0; i < node->mNumChildren; ++i)
        buildNodeHierarchy(node->mChildren[i], bone_name_hash, current_node_transform, bone_rotations);
}

#if 1  //todo old

void SkeletalMeshComponent::setAnimTime(AnimType* CurrentAnim, float time,float deltaTime)
{

    //CurrentAnim->scene=m_MeshData->MeshScene;

    if(!CurrentAnim)return;
    const aiScene* MeshScene = m_MeshData->MeshScene;
    if(!MeshScene)return;

    for(unsigned int i = 0; i < bone_transforms.size(); i++) {
        bone_transforms[i] = glm::mat4(1.0f);
    }

    float animation_time = 0;


    MeshScene = CurrentAnim->scene;
    if(!MeshScene)return;

    if(MeshScene->HasAnimations()) {

        animation_time = fmod(time, (float) MeshScene->mAnimations[0]->mDuration); //time; ;//= fmod(time_in_tics, scene->mAnimations[0]->mDuration);


        for (unsigned int i = 0; i < MeshScene->mAnimations[0]->mNumChannels; ++i) {
            const aiNodeAnim *node = MeshScene->mAnimations[0]->mChannels[i];
            std::unordered_map<std::string, int>::iterator it = m_MeshData->bone_name_hash.find(node->mNodeName.data);
            if (it == m_MeshData->bone_name_hash.end()) {
                continue;
            }
            if(it->second < 0)continue;


            int rotationKey = 0;
            while (animation_time >= node->mRotationKeys[rotationKey + 1].mTime) ++rotationKey;
            rotationKey = rotationKey % node->mNumRotationKeys;
            aiQuaternion ai_rotation = node->mRotationKeys[rotationKey].mValue;
            {
                int nextRotationKey = rotationKey + 1;
                aiQuaternion ai_rotation_next = node->mRotationKeys[nextRotationKey].mValue;
                float delta, alpha;
                delta = node->mRotationKeys[nextRotationKey].mTime - node->mRotationKeys[rotationKey].mTime;
                alpha = fabs((node->mRotationKeys[nextRotationKey].mTime - animation_time) / delta);
                ai_rotation = lerp(ai_rotation_next, ai_rotation, alpha);

            }





            int positionKey = 0;
            while (animation_time >= node->mPositionKeys[positionKey + 1].mTime) ++positionKey;
            positionKey = positionKey % node->mNumPositionKeys;
            aiVector3D ai_translation = node->mPositionKeys[positionKey].mValue;

            {

                int nextPositionKey = positionKey + 1;
                aiVector3D ai_translation_next = node->mPositionKeys[nextPositionKey].mValue;

                glm::vec3 a1(ai_translation.x, ai_translation.y, ai_translation.z);
                glm::vec3 b1(ai_translation_next.x, ai_translation_next.y, ai_translation_next.z);

                float delta = node->mPositionKeys[nextPositionKey].mTime - node->mPositionKeys[positionKey].mTime;
                float alpha = fabs((animation_time - node->mRotationKeys[positionKey].mTime) / delta);
                for(unsigned int d = 0; d < 3; d++)ai_translation[d] = ai_translation[d] + (ai_translation_next[d] - ai_translation[d]) * alpha;
            }



            //aiQuaternion ai_rotation_t_pose = node->mRotationKeys[0].mValue;

            //            glm::quat rotation_t_pose;
            //            for(unsigned m=0;m<MeshScene->mNumMeshes;++m)
            //                if(MeshScene->mMeshes[m]->mNumBones>it->second){
            //                    glm::mat4 offset_mat;
            //                    copyAiMatrixToGLM( &MeshScene->mMeshes[m]->mBones[it->second]->mOffsetMatrix, offset_mat);
            //                    rotation_t_pose=glm::quat_cast(offset_mat);
            //                    break;
            //                }

            //glm::quat rotation_t_pose((GLfloat)ai_rotation.w, (GLfloat)ai_rotation.x, (GLfloat)ai_rotation.y, (GLfloat)ai_rotation.z);



            aiQuaternion ai_rotation_pose = m_MeshData->MeshScene->mAnimations[0]->mChannels[i%m_MeshData->MeshScene->mAnimations[0]->mNumChannels]->mRotationKeys[0].mValue;
            glm::quat rotation_t_pose((GLfloat)ai_rotation_pose.w, (GLfloat)ai_rotation_pose.x, (GLfloat)ai_rotation_pose.y, (GLfloat)ai_rotation_pose.z);

            glm::quat rotation((GLfloat)ai_rotation.w, (GLfloat)ai_rotation.x, (GLfloat)ai_rotation.y, (GLfloat)ai_rotation.z);

            //rotation=glm::normalize(rotation);
            //rotation=rotation*glm::inverse(rotation_t_pose);
            //rotation=rotation_t_pose;
            //rotation=t_pose_rotations[it->second];//glm::inverse(rotation_t_pose)*(rotation);

            //QENGINE_INFO((rotation_t_pose*glm::quat()*glm::inverse(rotation_t_pose)));
            //rotation=glm::mix(rotation, glm::quat(), 0.7f);;//glm::inverse(rotation)*glm::inverse(rotation_t_pose);

            aiVector3D ai_translation_pose = m_MeshData->MeshScene->mAnimations[0]->mChannels[i%m_MeshData->MeshScene->mAnimations[0]->mNumChannels]->mPositionKeys[0].mValue;
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3((GLfloat)ai_translation_pose.x, (GLfloat)ai_translation_pose.y, (GLfloat)ai_translation_pose.z));



            //glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3((GLfloat)ai_translation.x, (GLfloat)ai_translation.y, (GLfloat)ai_translation.z));

            //glm::mat4 translation = glm::translate(glm::mat4(1.0f), t_pose_positions[it->second]);

            //todo ?
            //bone_transforms[it->second] *=   glm::mat4_cast(glm::normalize(rotation));


            m_bone_positions[it->second]=glm::vec3((GLfloat)ai_translation_pose.x, (GLfloat)ai_translation_pose.y, (GLfloat)ai_translation_pose.z);
            m_bone_rotations[it->second]=rotation;

            //bone_transforms[it->second] =  translation * glm::mat4_cast(rotation);
        }
    }



//    int hips_id=m_MeshData->bone_name_hash_index[Hash("Hips")];
//    bone_transforms[hips_id][3][0] = 0;
//    bone_transforms[hips_id][3][1] = 0;
//    bone_transforms[hips_id][3][2] = 0;

     setBonePosition("Hips",glm::vec3());


    for(unsigned int i = 0; i < bone_transforms.size(); i++){
     bone_transforms_pre[i]=bone_transforms[i] =  glm::translate(glm::mat4(1),m_bone_positions[i]) * glm::mat4_cast(m_bone_rotations[i]);
    }
    buildNodeHierarchy(m_MeshData->MeshScene->mRootNode, m_MeshData->bone_name_hash, glm::mat4(1.0f), bone_transforms_pre);

    Event msg(Hash("DoInverseKinematic"));
    sendEvent(msg);


    for(unsigned int i = 0; i < bone_transforms.size(); i++){
        bone_transforms_pre[i]=bone_transforms[i] =  glm::translate(glm::mat4(1),m_bone_positions[i]) * glm::mat4_cast(m_bone_rotations[i]);
    }

//    for(unsigned int i = 0; i < bone_transforms.size(); i++)
//        bone_transforms_pre[i] = bone_transforms[i];


    //bone_transforms[m_MeshData->bone_name_hash["Alpha_Hips"]][3][1] = 0;

    buildNodeHierarchy(m_MeshData->MeshScene->mRootNode, m_MeshData->bone_name_hash, glm::mat4(1.0f), bone_transforms);


    //float v = animation_time / (float)CurrentAnim->duration;
    //if(nextAnim!=currentAnim)//&&(v>0.7||v<0.3))



    for(unsigned int i = 0; i < bone_transforms.size(); i++) {
        bone_transforms[i] = bone_transforms[i] * m_MeshData->offset_matrix_array[i];

        //####// todo infra-animation interpolation //####//
        //bone_transforms[i] = mat4_mix(bone_transforms[i] * m_MeshData->offset_matrix_array[i], bone_transforms_pre[i],  0.8f);
        //bone_transforms_pre[i] = bone_transforms[i];
    }


    /*
    else
    for(unsigned int i = 0; i < bone_transforms.size(); i++)
    {
        bone_transforms[i] = bone_transforms[i] * mesh->offset_matrix_array[i];
        bone_transforms_pre[i] = bone_transforms[i];
    }	*/

}

#else

void SkeletalMeshComponent::setAnimTime(AnimType* CurrentAnim, float time,float deltaTime)
{

    if(!CurrentAnim)return;
    const aiScene* MeshScene = CurrentAnim->scene;
    if(!MeshScene)return;


    for(unsigned int i = 0; i < bone_transforms.size(); i++) {
        bone_transforms[i] = glm::mat4(1.0f);
    }

    float animation_time = 0;
    if(MeshScene->HasAnimations()) {

        animation_time = fmod(time, (float) MeshScene->mAnimations[0]->mDuration); //time; ;//= fmod(time_in_tics, scene->mAnimations[0]->mDuration);


        for (unsigned int i = 0; i < MeshScene->mAnimations[0]->mNumChannels; ++i) {
            const aiNodeAnim *node = MeshScene->mAnimations[0]->mChannels[i];
            std::map<std::string, int>::iterator it = m_MeshData->bone_name_hash.find(node->mNodeName.data);
            if (it == m_MeshData->bone_name_hash.end()) {
                continue;
            }
            if(it->second < 0)continue;


            int rotationKey = 0;
            while (animation_time >= node->mRotationKeys[rotationKey + 1].mTime) ++rotationKey;
            rotationKey = rotationKey % node->mNumRotationKeys;
            aiQuaternion ai_rotation = node->mRotationKeys[rotationKey].mValue;
            {
                int nextRotationKey = rotationKey + 1;
                aiQuaternion ai_rotation_next = node->mRotationKeys[nextRotationKey].mValue;
                float delta, alpha;
                delta = node->mRotationKeys[nextRotationKey].mTime - node->mRotationKeys[rotationKey].mTime;
                alpha = fabs((node->mRotationKeys[nextRotationKey].mTime - animation_time) / delta);
                ai_rotation = lerp(ai_rotation_next, ai_rotation, alpha);

            }


            int positionKey = 0;
            while (animation_time >= node->mPositionKeys[positionKey + 1].mTime) ++positionKey;
            positionKey = positionKey % node->mNumPositionKeys;
            aiVector3D ai_translation = node->mPositionKeys[positionKey].mValue;

            {

                int nextPositionKey = positionKey + 1;
                aiVector3D ai_translation_next = node->mPositionKeys[nextPositionKey].mValue;

                glm::vec3 a1(ai_translation.x, ai_translation.y, ai_translation.z);
                glm::vec3 b1(ai_translation_next.x, ai_translation_next.y, ai_translation_next.z);

                float delta = node->mPositionKeys[nextPositionKey].mTime - node->mPositionKeys[positionKey].mTime;
                float alpha = fabs((animation_time - node->mRotationKeys[rotationKey].mTime) / delta);
                for(unsigned int d = 0; d < 3; d++)ai_translation[d] = ai_translation[d] + (ai_translation_next[d] - ai_translation[d]) * alpha;
            }




            //            unsigned int scaleIndex = 0;
            //            while( 1 ) {
            //                if( scaleIndex + 1 >= node->mNumScalingKeys )
            //                    break;
            //                if( node->mScalingKeys[scaleIndex++].mTime > animation_time )
            //                    break;
            //            }
            //aiVector3D curScaling = node->mScalingKeys[scaleIndex].mValue;



            glm::quat rotation((GLfloat)ai_rotation.w, (GLfloat)ai_rotation.x, (GLfloat)ai_rotation.y, (GLfloat)ai_rotation.z);
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3((GLfloat)ai_translation.x, (GLfloat)ai_translation.y, (GLfloat)ai_translation.z));
            glm::mat4 M = translation * glm::mat4_cast(glm::normalize(rotation));

            //todo ?
            //M =  translation;//glm::mat4_cast(glm::normalize(rotation));
            bone_transforms[it->second] = translation * glm::mat4_cast(glm::normalize(rotation));// M;
        }
    }

    int hips_id=m_MeshData->bone_name_hash["Hips"];
    bone_transforms[hips_id][3][0] = 0;
    bone_transforms[hips_id][3][1] = 0;
    bone_transforms[hips_id][3][2] = 0;
    //bone_transforms[m_MeshData->bone_name_hash["Alpha_Hips"]][3][1] = 0;

    buildNodeHierarchy(m_MeshData->MeshScene->mRootNode, m_MeshData->bone_name_hash, glm::mat4(1.0f), bone_transforms);


    //float v = animation_time / (float)CurrentAnim->duration;
    //if(nextAnim!=currentAnim)//&&(v>0.7||v<0.3))
    for(unsigned int i = 0; i < bone_transforms.size(); i++) {
        bone_transforms[i] = bone_transforms[i] * m_MeshData->offset_matrix_array[i];

        //####// todo infra-animation interpolation //####//
        //bone_transforms[i] = mat4_mix(bone_transforms[i] * m_MeshData->offset_matrix_array[i], bone_transforms_pre[i],  0.8f);
        //bone_transforms_pre[i] = bone_transforms[i];
    }


    /*
    else
    for(unsigned int i = 0; i < bone_transforms.size(); i++)
    {
        bone_transforms[i] = bone_transforms[i] * mesh->offset_matrix_array[i];
        bone_transforms_pre[i] = bone_transforms[i];
    }	*/

}

#endif

void SkeletalMeshComponent::Update(UpdateParameter &param)
{
    time += param.deltaTime * m_Velocity;

    AnimType* CurrentAnim = getAnimation(currentAnim);

    float animation_time = 0;

    if(!CurrentAnim)return;


    animation_time = CurrentAnim->firstFrame + fmod(time, CurrentAnim->lastFrame - CurrentAnim->firstFrame);


    // if(preTime<time){finished=true;isPlaying=0; preTime=time;}
    // else {finished=false;isPlaying=1;AnimationTime=0; preTime=time-1;
    //    if(NextAnim){CurrentAnim=NextAnim;NextAnim=IdleAnim;}}
    //
    // if(CurrentAnim!=IdleAnim)NextAnim=IdleAnim;
    // else if(NextAnim)if(NextAnim!=IdleAnim){AnimationTime=0;preTime=time-1;CurrentAnim=NextAnim;	}
    //

    setAnimTime(CurrentAnim, animation_time , param.deltaTime);
}

void SkeletalMeshComponent::updateUniform(GLuint programID)
{
    int loc = glGetUniformLocation(programID, "bone_transforms");
    if(loc > -1)
        //glUniformMatrix4fv(loc, mesh->bone_transforms.size(), GL_FALSE, glm::value_ptr(mesh->bone_transforms[0]));
        glUniformMatrix4fv(loc, bone_transforms.size(), GL_FALSE, &bone_transforms[0][0][0]);
}


void SkeletalMeshComponent::addAnimation(AnimID anim, PathName filename, int firstFrame, int lastFrame)
{

    const aiScene* mesh = AssimpScene::load(filename,
                                            aiProcess_CalcTangentSpace          |
                                            aiProcess_GenSmoothNormals          |
                                            aiProcess_JoinIdenticalVertices     |
                                            aiProcess_ImproveCacheLocality      |
                                            aiProcess_LimitBoneWeights          |
                                            aiProcess_RemoveRedundantMaterials  |
                                            aiProcess_SplitLargeMeshes          |
                                            aiProcess_Triangulate               |
                                            aiProcess_GenUVCoords               |
                                            aiProcess_SortByPType               |
                                            aiProcess_FindDegenerates           //aiProcess_FindInvalidData
                                            )->scene;


    AnimType a = {anim, firstFrame, lastFrame, lastFrame - firstFrame, mesh};
    animations[anim] = a;

}
void SkeletalMeshComponent::addAnimation(AnimID anim, int firstFrame, int lastFrame)
{
    AnimType a = {anim, firstFrame, lastFrame, lastFrame - firstFrame, m_MeshData->MeshScene};
    animations[anim] = a;
}

void SkeletalMeshComponent::removeAnimation(AnimID anim)
{
    animations.erase(anim);
}
void SkeletalMeshComponent::playAnimation(AnimID anim)
{
    currentAnim = anim;
    time = 0;
}
void SkeletalMeshComponent::setAnimation(AnimID anim)
{
    currentAnim = anim;
}
SkeletalMeshComponent::AnimType* SkeletalMeshComponent::getAnimation(AnimID anim)
{
    Animations::iterator it = animations.find(anim);
    if (it == animations.end()){
        if(animations.size())return &(animations.begin())->second;
        QENGINE_INFO("no animation");
        return 0;
    }
    else return &(animations[anim]);
}


glm::mat4 SkeletalMeshComponent::getBoneMatrix(StringHash bone) const{
    int index=getBoneIndex(bone);
    if(index==-1)return glm::mat4(1);
    return bone_transforms_pre[index];
}

void SkeletalMeshComponent::setBoneMatrix(StringHash bone,glm::mat4 matrix){
    int index=getBoneIndex(bone);
    if(index==-1)return;

    bone_transforms_pre[index]=matrix;
}


int SkeletalMeshComponent::getBoneIndex(StringHash bone) const{
    auto &hashmap=m_MeshData->bone_name_hash_index;
    auto it=hashmap.find(bone.getHash());
    if(it==hashmap.end())return -1;
    return it->second;
}


glm::vec3 SkeletalMeshComponent::getBonePosition(StringHash bone) const{
    int index=getBoneIndex(bone);
    if(index==-1)return glm::vec3();

    return m_bone_positions[index];
}
void SkeletalMeshComponent::setBonePosition(StringHash bone,glm::vec3 pos){
    int index=getBoneIndex(bone);
    if(index==-1)return;

    m_bone_positions[index]=pos;
}

glm::quat SkeletalMeshComponent::getBoneRotation(StringHash bone) const{
    int index=getBoneIndex(bone);
    if(index==-1)return glm::quat();

    return m_bone_rotations[index];
}
void SkeletalMeshComponent::setBoneRotation(StringHash bone, glm::quat rotation){
    int index=getBoneIndex(bone);
    if(index==-1)return;

    m_bone_rotations[index]=rotation;
}





}
