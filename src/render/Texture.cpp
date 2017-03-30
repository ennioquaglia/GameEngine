#define GLEW_STATIC
#include <GL/glew.h>

#include "Texture.h"
#include "Debug.h"
#include <SOIL\SOIL.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm/gtc/random.hpp>
#include <vector>

namespace Engine {

Texture::Texture() :texture(0)
{
}

Texture::Texture(PathName filename) : texture(0) {
    import(filename);
}


Texture::~Texture()
{
}

void Texture::destroy()
{
}

bool Engine::Texture::bind() {
    if (!getID())return false;
    glBindTexture(GL_TEXTURE_2D, getID());
    return true;
}

const unsigned int Texture::getID() const {
    //if (!texture);//cout<<"text error";
    return texture;
}



void Texture::import(PathName _filename)
{
    std::string filename=_filename.toStd();
    this->filename = _filename;

    {
        //one pixel texture
        float rgba[4] = { -1, 0, 0, 1 };
        if (filename[0] == '#') {
            for (unsigned int i = 0; i < 4; i++) {
                int num = 0;
                std::string c = filename.substr(1 + i * 2, 2);
                sscanf(c.data(), "%x", &num);
                rgba[i] = (float)num;
                rgba[i] /= (float)255;
            }
            if (filename.size() == 7)rgba[3] = 1;
        }
        if (filename == "white") {
            rgba[0] = 1;
            rgba[1] = 1;
            rgba[2] = 1;
        }
        if (filename == "black") {
            rgba[0] = 0;
        }
        if (filename == "normal") {
            rgba[0] = 0.5;
            rgba[1] = 0.5;
            rgba[2] = 1;
            rgba[3] = 0;
        }
        if (rgba[0] >= 0) {
            glDebug();
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, rgba);

            glDebug();
            return;
        }
    }

    if (filename == "noise512") {
        glDebug();
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        const int size = 512;
        std::vector<float> data(size * size * 4);
        for (int i = 0; i < size * size; i++) {
            glm::vec3 n = glm::linearRand(0.1f, 1.0f)*glm::sphericalRand(1.0f) / 2.0f + glm::vec3(0.5f);
            data[i * 4 + 0] = n.x;
            data[i * 4 + 1] = n.y;
            data[i * 4 + 2] = n.z;
            data[i * 4 + 3] = (n.x + n.y + n.z) / 3.0f;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_FLOAT, &data[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glDebug();
        return;
    }

    texture = SOIL_load_OGL_texture(
                filename.c_str(),
                SOIL_LOAD_AUTO,
                SOIL_CREATE_NEW_ID,
                SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
                );

    if (!texture) {
        printf("SOIL loading error(%s): '%s'\n", filename.c_str(), SOIL_last_result());
        return;
    }


    glBindTexture(GL_TEXTURE_2D, texture);
    glDebug();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glDebug();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glDebug();



#ifdef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#ifdef GL_TEXTURE_MAX_ANISOTROPY_EXT
    GLfloat fLargest = 1;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
#endif
#endif


    glDebug();
    QENGINE_INFO("new Texture (" + filename + ")");

}

}
