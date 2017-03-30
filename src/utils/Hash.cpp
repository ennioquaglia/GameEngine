#include "Hash.h"

namespace Engine{


unsigned int Hash(std::string str) {

    str+=(char)0;
    unsigned hash = 2166136261U;
    for(size_t i = 0; i < str.length(); i++)
    {
        hash = hash ^ (str[i]);       /* xor  the low 8 bits */
        hash = hash * 16777619;  /* multiply by the magic number */
    }
    return hash;
}


#ifdef USE_DYNAMIC_ENUM




const unsigned int StringEnum(std::string str) {
    static unsigned int _count=1;
    static std::map<std::string,unsigned int> _map;

    if(_map.find(str)==_map.end())_map[str]=1<<_count++;

    return _map[str];
}


#endif



}
