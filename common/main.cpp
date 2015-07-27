#include <iostream>
#include "Preprocessor.h"
#include <fstream>

#define a 


#ifdef b

#elif defined a


using namespace std;
#endif

std::string loadTextFile(std::string path)
{
    std::ifstream in(path.c_str(), std::ifstream::in);
    std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    if(!contents.size()) {
            contents="Error during file loading! Does the file exist? \n";
        }
    //cout<<contents;
    return contents;
}


int main()
{
    
    Preprocessor p;
    p.addVoiceToDictionary("GLSL_VERSION","120");
    cout <<"file<<\n\n\n\n"<<p.parse(loadTextFile("shader.vs"))<<"\n\n\n>>\n";
    p.printDictionary();
    return 0;       
} 