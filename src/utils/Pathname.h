#pragma once

#include "EString.h"
#include "Shell.h"



namespace Engine{


    class PathName{


    public:
        PathName(){}
        PathName(EString str){
            original=data=str.data;
            EString::replaceAll(data,"::",Shell::getDataPath());
        }
        template<unsigned int N>
        PathName(const char (&str)[N]){
            EString s(str);
            original=data=s.data;
            EString::replaceAll(data,"::",Shell::getDataPath());
        }
        PathName(const char *str){
            EString s(str);
            original=data=s.data;
            EString::replaceAll(data,"::",Shell::getDataPath());
        }
        PathName(std::string str){
            original=data=str;
            EString::replaceAll(data,"::",Shell::getDataPath());
        }
        friend bool operator< (const PathName& lhs, const PathName& rhs){ return lhs.data<rhs.data; }
        friend bool operator==(const PathName& lhs, const PathName& rhs){ return lhs.data==rhs.data; }
        std::string data,original;
        std::string toStd() const{
            return data;
        }

    };

    typedef  PathName FileName;

}
