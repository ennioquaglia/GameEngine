/*TODO LIST:
 * -git gud at dota
 * -shadow
 * -directional lights
 * -Post Prcessing(Framebuffer support)
 *      -The Matrix Mode
 *      -Deffered shading
 *      -god's ray
 *      -bloom
 *      -SSAO
 *      -SSGI
 *      -SSLR
 *      -color shifting
 * -Particle Systems
 * -Physics Engine
 * -SkeletalMesh & animation manager
 *
 */


#ifdef ENGINE_TEST

#include "Hash.h"
#include "GlfwWindow.h"
#include "DemoApplication.h"
#include "Debug.h"

#include <iostream>

using namespace std;



int main(void)
{


    using namespace Engine;

    std::cout<<"application context:\n";
    system("cd");
    std::cout<<"\n\n";



    Context* c = new GlfwWindow("Engine!", 800, 600);

    if (!c->Init())QENGINE_FATAL("Failed to init the context");

    QENGINE_INFO("Window context created!");

    DemoApplication *demo=new DemoApplication(c);

    if (!demo->Init())QENGINE_FATAL("Failed to init the DemoApplication");

    demo->exec();
    delete demo;


    //QENGINE_PAUSE();
    return 0;
}

#endif
