
#include "Shell.h"
#include <fstream>
#include "Debug.h"

namespace Engine{


    std::string Shell::loadTextFile(std::string path){//todo move to utility header

        std::ifstream in(path.c_str(), std::ifstream::in);
        std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

        if (contents.empty()) {
            QENGINE_ERROR("Error during file loading! Does the file exist?\n");
            contents = "Error during file loading! Does the file exist? \n";
        }
        //QENGINE_INFO(contents);
        return contents;

    }

}
