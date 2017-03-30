#pragma once
#include <vector>
#include <map>
#include <string>



namespace Engine
{

template <unsigned int N, unsigned int I>
struct FnvHash
{

    constexpr static unsigned int Hash(const char(&str)[N])
    {
        return (FnvHash<N, I - 1>::Hash(str) ^ str[I - 1]) * 16777619L;
    }

};

template <unsigned int N>
struct FnvHash<N, 1>
{

    constexpr static unsigned int Hash(const char(&str)[N])
    {
        return (2166136261L ^ str[0]) * 16777619L;
    }

};

class StringHash

{
    unsigned int m_hash;
public:

    template <unsigned int N>
    constexpr StringHash(const char(&str)[N])
        : m_hash(FnvHash<N, N>::Hash(str))
    {}

    constexpr unsigned int getHash() const {
        return m_hash;
    }

};


/*
        template <unsigned int N, unsigned int I>
        constexpr static unsigned int FnvHash(const char(&str)[N])
        {
                return I==1? (2166136261L ^ str[0]) * 16777619L :(FnvHash<N, I - 1>(str) ^ str[I - 1]) * 16777619L;
        }

        template <unsigned int N>
        constexpr unsigned int Hash(const char(&str)[N]) {
                return FnvHash<N, N>(str);
        }*/


namespace Counter {


/*
#ifdef _MSC_VER
    template<int N>
    struct flag {
        friend constexpr int adl_flag(flag<N>);
    };
    template<int N>
    struct writer {
        friend constexpr int adl_flag(flag<N>) {
            return N;
        }

        static constexpr int value = N;
    };
    template<int N, class = char[noexcept(adl_flag(flag<N>())) ? +1 : -1]>
    int constexpr reader(int, flag<N>) {
        return N;
    }
    template<int N>
    int constexpr reader(float, flag<N>, int R = reader(0, flag<N - 1>())) {
        return R;
    }
    int constexpr reader(float, flag<0>) {
        return 0;
    }
    template<int N = 1, int C = reader(0, flag<32>())>
    int constexpr next(int R = writer<C + N>::value) {
        return R;
    }

#else
    template<int N>
    struct flag {
      friend constexpr int adl_flag (flag<N>);
    };
    template<int N>
    struct writer {
      friend constexpr int adl_flag (flag<N>) {
        return N;
      }

      static constexpr int value = N;
    };
    template<int N, int = adl_flag (flag<N> {})>
    int constexpr reader (int, flag<N>) {
      return N;
    }

    template<int N>
    int constexpr reader (float, flag<N>, int R = reader (0, flag<N-1> {})) {
      return R;
    }

    int constexpr reader (float, flag<0>) {
      return 0;
    }
    template<int N = 1>
    int constexpr next (int R = writer<reader (0, flag<32> {}) + N>::value) {
      return R;
    }

#endif

    template<unsigned int N>
    struct Foo {
        enum { value = (1 << (Counter::next() - 1)) };
    };
    */
}



template <unsigned int N>
constexpr unsigned int Hash(const char(&str)[N]) {
    return StringHash(str).getHash();
}

extern unsigned int Hash(std::string str);


//#define StringEnum(str) (Counter::Foo<Hash(str)>::value)




//#define USE_DYNAMIC_ENUM

//#ifdef USE_DYNAMIC_ENUM

//const unsigned int StringEnum(std::string str);

//#else



//#define ENGINE_ADD_ENUM_DEFINITION(key,_value) template<int Key>struct StaticMap { enum{ value = -1 }; };



//namespace __hidden__ {

//template<unsigned int Key>
//struct StaticMap {
//    enum{ value = 0 };
//};



//}


//#define ENGINE_ADD_ENUM_DEFINITION(key,_value) namespace __hidden__{template<> struct StaticMap<Hash(key)> { enum{ value = _value }; };}
//#define StringEnum(str) __hidden__::StaticMap< Hash(str) >::value;






////enum EventType {
////    none=0,
////    Rotate,
////    shouldMove,
////    shouldWalk,
////    shouldJump,
////    isOnGround,
////    shouldWarp,
////    stopWalking,
////    shouldBeDestroyed,
////    destroy,
////    componentRemoved,
////    componentUpdated,
////    componentAdded,
////    userTypeEvent
////};




//#define _OFFSET 1

//ENGINE_ADD_ENUM_DEFINITION("none"             , 0             )
//ENGINE_ADD_ENUM_DEFINITION("Rotate"           , _OFFSET << 1  )
//ENGINE_ADD_ENUM_DEFINITION("shouldMove"       , _OFFSET << 2  )
//ENGINE_ADD_ENUM_DEFINITION("shouldWalk"       , _OFFSET << 3  )
//ENGINE_ADD_ENUM_DEFINITION("shouldJump"       , _OFFSET << 4  )
//ENGINE_ADD_ENUM_DEFINITION("isOnGround"       , _OFFSET << 5  )
//ENGINE_ADD_ENUM_DEFINITION("shouldWarp"       , _OFFSET << 6  )
//ENGINE_ADD_ENUM_DEFINITION("stopWalking"      , _OFFSET << 7  )
//ENGINE_ADD_ENUM_DEFINITION("shouldBeDestroyed", _OFFSET << 8  )
//ENGINE_ADD_ENUM_DEFINITION("destroy"          , _OFFSET << 9  )
//ENGINE_ADD_ENUM_DEFINITION("componentRemoved" , _OFFSET << 10 )
//ENGINE_ADD_ENUM_DEFINITION("componentUpdated" , _OFFSET << 11 )
//ENGINE_ADD_ENUM_DEFINITION("componentAdded"   , _OFFSET << 12 )
//ENGINE_ADD_ENUM_DEFINITION("userTypeEvent"    , _OFFSET << 13 )




//#endif





}

