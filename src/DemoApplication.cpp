#include "DemoApplication.h"
#include "Context.h"
#include "TransformComponent.h"
#include "StaticMeshComponent.h"
#include "SkeletalMeshComponent.h"
#include "InputManager.h"
#include "Material.h"
#include "LightComponent.h"
#include "PostProcessEffect.h"
#include "PhysicsComponent.h"


#include "demoentity.h"

#include <glm/gtc/quaternion.hpp>
#include <GLFW\glfw3.h>
#include <windows.h>



namespace Engine {










class  TextureDebug : public RenderSystem
{

    COMPONENT_TYPE_INFORMATION(TextureDebug)
    Shader * shader;
public:
    TextureDebug() {
        shader = new Shader( "::Data/Shader/Debug/shader.vs",  "::Data/Shader/Debug/shader.fs");
        Shader::Compile(shader);
        glDebug();

    }
    virtual void Draw(RenderParameter &param) {
        if(!(param.flag & RenderFlag::Default))return;
        shader->Use();
        LightSystem::updateUniforms(shader,const_cast<Scene*>(param.scene));

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


class  SpinnerComponent : public Component
{

    COMPONENT_TYPE_INFORMATION(TextureDebug)
    Shader * shader;
public:
    SpinnerComponent() {
        m_Flag.add(Object::update);

    }

    static int aaaa;

    virtual void Update(UpdateParameter &param) {
        if(!getEntity())return;
        static double alpha=0;
        auto trans=getEntity()->getComponentByType<TransformComponent>();
        //m_Sun->dir = glm::normalize(glm::vec3(sin(alpha*0.01)*0.5, 1, cos(alpha*0.01)*0.5));
        //trans->setPosition(glm::vec3(cos(alpha),sin(alpha*2)*0.1f+0.2,sin(alpha))*(aaaa?20.0f:22.0f));

        alpha+=param.deltaTime;

        trans->setScale(glm::vec3(0.15f));

        SkeletalMeshComponent *m=param.scene->getObjectByType<SkeletalMeshComponent>();
        if(m){
            if(aaaa==0){
                trans->setScale(glm::vec3(1.05f));
//                glm::vec3 dir=glm::vec3(m->getBoneMatrix("RightHandIndex1")*glm::vec4(0,0,0,1) - m->getBoneMatrix("RightHand")*glm::vec4(0,0,0,1));

//                glm::mat4 mat=m->getEntity()->getComponentByType<TransformComponent>()->getMatrix()*glm::lookAt(glm::vec3(0.f),dir,glm::vec3(0,1,0));
//                trans->setRotation(glm::normalize(glm::quat_cast(mat)));

                //trans->setRotation(glm::quat_cast(m->getEntity()->getComponentByType<TransformComponent>()->getRotation()));
                trans->setMatrix(m->getEntity()->getComponentByType<TransformComponent>()->getMatrix()*(m->getBoneMatrix("RightHand"))*glm::scale(glm::mat4(1),glm::vec3(100.f)));//*glm::translate(glm::mat4(1),glm::vec3(-100,sin(alpha)*400,-150)));
            }else if(aaaa==1)
                trans->setMatrix(m->getEntity()->getComponentByType<TransformComponent>()->getMatrix()*(m->getBoneMatrix("RightArm")));
            else trans->setMatrix(m->getEntity()->getComponentByType<TransformComponent>()->getMatrix()*(m->getBoneMatrix("RightForeArm")));
        }

        ++aaaa;
        if(aaaa>2)aaaa=0;
        //trans->setPosition(param.scene->getCamera().getPos()+param.scene->getCamera().getDir()*15.0f);

    }

};
int SpinnerComponent::aaaa=0;


DemoApplication::DemoApplication(Context * context):m_Context(context)
{
    m_Scene = 0;
}

DemoApplication::~DemoApplication()
{
    Destroy();
}

bool DemoApplication::Init()
{

    if (!m_Context) {
        QENGINE_FATAL("DemoApplication::Init()  context pointer is null!");
        return 0;
    }

    m_Scene=new Scene(m_Context);
    m_Effect=new RealDeferredPostProcessor(m_Scene);//new MatrixModePostProcessor(m_Scene);//RealDeferredPostProcessor(m_Scene);
    m_Scene->setPostProcessorEffect(m_Effect);




    Material m("::Data/Texture/floor1_d.png","::Data/Texture/floor1_s.png","::Data/Texture/floor1_n.png");
    m.setHeightMap("::Data/Texture/floor1_h.png");


    m_Scene->deserializeXML("::Data/Level/livello.data");

    Entity *e=m_Scene->newEntity();
    trans=new TransformComponent();
    //        trans->setPosition(glm::vec3(10,0,0));
    //        trans->setScale(glm::vec3(1.3f,1.3f,1.5f));
    //        //trans->setRotation(glm::angleAxis(1.57f,glm::vec3(0,1,0)));
    //        e->addComponent(trans);

    //        StaticMeshComponent *mesh;
    //        mesh= new StaticMeshComponent("::Data/Model/mappa5.obj");
    //        mesh->addMaterial(m);//Material("::Data/Texture/floor1_d.png","::Data/Texture/floor1_s.png","::Data/Texture/floor1_n.png"));//,"::Data/Texture/floor1_n.png"));
    //        e->addComponent(mesh);
    //        e->addComponent(new StaticMeshPhysicsComponent("::Data/Model/mappa5.obj"));
    //        m_Scene->addObject(e);



    //        e=m_Scene->newEntity();

    //        mesh= new StaticMeshComponent("::Data/Model/opera_arte.obj");
    //        mesh->addMaterial(Material("#FFFF00FF"));//,"::Data/Texture/floor1_n.png"));
    //        e->addComponent(mesh);
    //        e->addComponent(new StaticMeshPhysicsComponent("::Data/Model/opera_arte.obj"));
    //        m_Scene->addObject(e);




    //        e=m_Scene->newEntity();
    //        trans=new TransformComponent();
    //        trans->setScale(glm::vec3(1.0f));
    //        e->addComponent(trans);
    //        //new StaticMeshComponent("::Data/Model/sphere.obj");
    //        mesh=new StaticMeshComponent("::Data/Model/mappa3.obj");

    //        mesh->addMaterial(m);//,"::Data/Texture/floor1_n.png"));
    //        mesh->setTextureMatrix(glm::mat2(glm::scale(glm::mat4(1),glm::vec3(0.001f)) ));
    //        e->addComponent(mesh);
    //        e->addComponent(new StaticMeshPhysicsComponent("::Data/Model/mappa3.obj"));
    //        //QENGINE_PAUSE();
    //        m_Scene->addObject(e);



    //        e=m_Scene->newEntity();

    m_Sun=new DirectionalLightComponent();
    LightComponent* l;
    l=m_Sun;
    l->setDirectional(1);
    l->dir = glm::normalize(glm::vec3(0.5, 1, 0.2));
    e->addComponent(l);


//    l=new LightComponent();
//    l->linearAttenuation=0.1;
//    l->quadraticAttenuation=0.01;
//    e->addComponent(l);


    m_Scene->addObject(e);
    //QENGINE_PAUSE();









    BulletPhysicsSystem *physicsSystem=new BulletPhysicsSystem();
    m_Scene->addObject(physicsSystem);


    m_Scene->addObject(new MainPlayer());



//    e=m_Scene->newEntity();
//    e->addComponent(new TransformComponent());
//    e->addComponent(new StaticMeshComponent("::Data/Model/sword1.obj"));
//    e->addComponent(new SpinnerComponent());
//    m_Scene->addObject(e);

//    e=m_Scene->newEntity();
//    e->addComponent(new TransformComponent());
//    e->addComponent(new StaticMeshComponent("::Data/Model/sphere.obj"));
//    e->addComponent(new SpinnerComponent());
//    m_Scene->addObject(e);

//    e=m_Scene->newEntity();
//    e->addComponent(new TransformComponent());
//    e->addComponent(new StaticMeshComponent("::Data/Model/sphere.obj"));
//    e->addComponent(new SpinnerComponent());
//    m_Scene->addObject(e);



    LightSystem *lightSystem = new LightSystem();
    m_Scene->addObject(lightSystem);

    SkeletalMeshSystem *skeletalMeshSystem = new SkeletalMeshSystem();
    m_Scene->addObject(skeletalMeshSystem);




    StaticMeshSystem *meshSystem = new StaticMeshSystem();
    m_Scene->addObject(meshSystem);


    m_Scene->PostSetup();
    //QENGINE_PAUSE();
    return 1;
}

bool DemoApplication::Destroy()
{
    delete m_Scene;
    delete m_Context;

    return 0;
}

bool DemoApplication::exec()
{
    float deltaTime, currentFrame, lastFrame;
    lastFrame = glfwGetTime();

    while (!m_Context->isClosed()) {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if(deltaTime> 0.1f)deltaTime=0.1f;

        m_Context->ClearBuffer();



        m_Scene->Update(deltaTime);
        Update(deltaTime*m_Scene->getVelocity());

        if(m_Context->isVisible())
            m_Scene->Render();

        m_Context->Update();
        //Sleep(5);

    }
    return 1;
}

void DemoApplication::Update(float deltaTime)
{

    //    static double alpha=0;
    //    //m_Sun->dir = glm::normalize(glm::vec3(sin(alpha*0.01)*0.5, 1, cos(alpha*0.01)*0.5));
    //    trans->setPosition(glm::vec3(cos(alpha),sin(alpha*2)*0.1f+0.2,sin(alpha))*22.0f);
    //    alpha+=deltaTime;





    if (m_Context->getInputManager().isKeyPressed(GLFW_KEY_F2))QENGINE_INFO(m_Scene->getCamera().Pos);

    if(m_Context->getInputManager().isKeyPressed(GLFW_KEY_F3))m_Scene->options.useVSM=!m_Scene->options.useVSM;
    if(m_Context->getInputManager().isKeyPressed(GLFW_KEY_F4)){
        static int b=1;
        m_Scene->setPostProcessorEffect(b++%2?m_Effect:0);
    }
    if(m_Context->getInputManager().isKeyPressed(GLFW_KEY_F5))m_Scene->options.useParalax=!m_Scene->options.useParalax;
    if(m_Context->getInputManager().isKeyPressed(GLFW_KEY_F6)){
        Entity *e=m_Scene->newEntity();
        TransformComponent *trans=new TransformComponent();

        trans->setPosition(glm::vec3(0,10,rand()/(float)RAND_MAX-0.5));
        e->addComponent(trans);
        e->addComponent(new StaticMeshComponent("::Data/Model/sphere.obj"));
        SpherePhysicsComponent *p=new SpherePhysicsComponent();
        e->addComponent(new LightComponent());
        p->setMass(1);
        e->addComponent(p);
        m_Scene->addObject(e);
    }

    if(m_Context->getInputManager().isKeyPressed(GLFW_KEY_1)){
        static int b=1;
        m_Scene->setVelocity( b++%2 ? 0. : 1 );
    }
    if(m_Context->getInputManager().isKeyPressed(GLFW_KEY_2)){
        static int b=1;
        m_Scene->setVelocity( b++%2 ? 1. : 0.1 );
    }
}







}
