#pragma once
#include <string>


namespace Engine{


class Shell{


    //static std::string m_DataPath;

public:
    static std::string getDataPath(){
        return "../../../../QEngine-gcc/EngineNewNew/game/";
    }

    static void setDataPath(std::string str){
       // m_DataPath=str;
    }

    static std::string loadTextFile(std::string path);

};



}
