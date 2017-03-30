#include "PlatformRenderer.h"
#include "Debug.h"

#define GLEW_STATIC
#include <GL/glew.h>

#include <stdio.h>
#include <iostream>


namespace Engine {

    PlatformRenderer::PlatformRenderer()
    {
    }


    PlatformRenderer::~PlatformRenderer()
    {
    }

    int PlatformRenderer::Init(){
        glewExperimental = true; // Needed for core profile
        if(glewInit() != GLEW_OK) {
            QENGINE_FATAL("Failed to initialize GLEW");
            return 0;
        }
        if(glewIsSupported("GL_VERSION_2_1")) {
            QENGINE_INFO("Ready for OpenGL 2.1");
        } else {
            QENGINE_FATAL("OpenGL 2.1 not supported");
            exit(1);
        }
        printf("Vendor: %s\n", glGetString(GL_VENDOR));
        printf("Renderer: %s\n", glGetString(GL_RENDERER));
        printf("Version: %s\n", glGetString(GL_VERSION));
        printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

        float glsl_version = 0;
        sscanf((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION), "%f", &glsl_version);
        QENGINE_INFO("glsl-version: " + glsl_version + "/" + (char*)glGetString(GL_SHADING_LANGUAGE_VERSION) + "/");
        //GlobalsManager::Istance()->set("glsl_version", glsl_version);
        //Debug("GM:glsl-version:" + GlobalsManager::Istance()->get("glsl_version"));

        if(glewIsSupported("GL_EXT_texture_filter_anisotropic") || GLEW_EXT_texture_filter_anisotropic) {
            std::cout << "support anisotropic filtering" << std::endl;
        }

        GLfloat max;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max);
        std::cout <<"GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT: "<< max << std::endl;

        glGetFloatv(GL_MAX_VERTEX_ATTRIBS, &max);
        std::cout << "GL_MAX_VERTEX_ATTRIBS: " << max << std::endl;
        glDebug();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);

        return 1;
    }
}
