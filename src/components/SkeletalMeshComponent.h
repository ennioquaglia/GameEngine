#pragma once
#include "Object.h"
#include "VAO.h"
#include "Pathname.h"
#include "Resource.h"
#include "MeshData.h"
#include "Material.h"
#include "Hash.h"

#include <string>
#include <vector>

namespace Engine {

    class Shader;
    class SkeletalMeshComponent : public Component
    {
        friend class SkeletalMeshSystem;
        PathName m_Filename;
        SkeletalMeshData *m_MeshData;
        MaterialList m_MaterialList;
        glm::mat2 m_TextureMatrix=glm::mat2(1.0f);


        float m_Velocity=1;


    public:
        COMPONENT_TYPE_INFORMATION(SkeletalMeshComponent)


        SkeletalMeshComponent(PathName filename);
        ~SkeletalMeshComponent();
        virtual void Init();
        //virtual void Destroy() {}
        virtual void OnScene(Scene &scene);
        virtual void OnAttached(Entity& e);

        void setTextureMatrix(glm::mat2 matrix);

        void addMaterial(Material mat);
        void setMaterial(Material mat,unsigned int index);
        void removeMaterial(unsigned int index);
        Material &getMaterial(unsigned int index=0);

        void setAnimVelocity(float vel){m_Velocity=vel;}
        float getAnimVelocity(){return m_Velocity;}


        virtual void Update(UpdateParameter &param);




    public:
            typedef int AnimID;
            enum {
                none=0,
                idle,
                usertype
            };
            struct AnimType {
                AnimID id;  //yes or no? redundant?
                int firstFrame;
                int lastFrame;
                int duration;
                const aiScene* scene;
            };
        private:


            typedef std::map<AnimID,AnimType> Animations;

            float time,preTime;
            bool finished,isPlaying;

            Animations animations;
            AnimID currentAnim,nextAnim;

            std::vector<glm::mat4> bone_transforms,bone_transforms_pre;
            std::vector<glm::quat> m_bone_rotations;
            std::vector<glm::vec3> m_bone_positions;

            void setAnimTime(AnimType* CurrentAnim, float time, float deltaTime);

        public:
            void updateUniform(GLuint programID);
            void addAnimation(AnimID anim,int firstFrame,int lastFrame);
            void addAnimation(AnimID anim,PathName filename,int firstFrame=0,int lastFrame=0);
            void removeAnimation(AnimID anim);
            void playAnimation(AnimID anim);
            void setAnimation(AnimID anim);

            int getBoneIndex(StringHash bone) const;

            glm::mat4 getBoneMatrix(StringHash bone) const;
            void      setBoneMatrix(StringHash bone,glm::mat4 matrix);
            glm::vec3 getBonePosition(StringHash bone) const;
            void      setBonePosition(StringHash bone, glm::vec3 pos);
            glm::quat getBoneRotation(StringHash bone) const;
            void      setBoneRotation(StringHash bone, glm::quat rotation);


        protected:
            AnimType* getAnimation(AnimID anim);

    };


    class SkeletalMeshSystem : public RenderSystem
    {
        typedef std::vector<SkeletalMeshComponent*> MeshComponents;
        MeshComponents m_Components;
        Shader *m_DefaultShader=0,*m_ShadowShader=0,*m_DeferredShader=0;
    public:
        COMPONENT_TYPE_INFORMATION(SkeletalMeshSystem)

        SkeletalMeshSystem(){Init();}
        virtual void Draw(RenderParameter &param);
        virtual void Update(UpdateParameter &param);
        virtual void addComponent(Object *obj);

        virtual void Init();
        //virtual void Destroy() {}

    };


    class AnimationManagerComponent : public Component
    {

        bool animSetupDone;
    public:
        COMPONENT_TYPE_INFORMATION(AnimationManagerComponent)
        AnimationManagerComponent(){animSetupDone=0;}

        virtual void setupAnimation(SkeletalMeshComponent* m){}
        virtual SkeletalMeshComponent* getSkeletalMesh(){

            Entity* e=getEntity();
            if(!e)return 0;
            SkeletalMeshComponent* comp=0;
            comp=e->getObjectByType<SkeletalMeshComponent>();
            return comp;
        }
        virtual void handleEvent(Event &msg) {
            if(animSetupDone)return;

            if(msg.getType()==EventType::componentAdded){
                SkeletalMeshComponent* m=getSkeletalMesh();
                if(m){
                    setupAnimation(m);
                    animSetupDone=1;
                }
            }
        }

    };



}
