#pragma once

#define ME_INLINE inline

#include <vector>



namespace Engine
{




    template <unsigned int N, unsigned int I>

    struct FnvHash

    {

        ME_INLINE static unsigned int Hash(const char(&str)[N])

        {

            return (FnvHash<N, I-1>::Hash(str) ^ str[I-1])*16777619u;

        }

    };



    template <unsigned int N>

    struct FnvHash<N, 1>

    {

        ME_INLINE static unsigned int Hash(const char(&str)[N])

        {

            return (2166136261u ^ str[0])*16777619u;

        }

    };



    class StringHash

    {
        unsigned int m_hash;
    public:

        template <unsigned int N>

        ME_INLINE StringHash(const char(&str)[N])

            : m_hash(FnvHash<N, N>::Hash(str))

        {

        }
        uint getHash() const {
            return m_hash;
        }

    };


    class Flag
    {
    	typedef list<uint> FlagList;
        FlagList flags;
    public:
        Flag() {}
        bool operator==(const StringHash &lx) {
            for(FlagList::iterator it=flags.begin();it!=flags.end();it++)if(lx.getHash()==*it)return 1;
            return 0;
        }
        void add(StringHash h){
			for(FlagList::iterator it=flags.begin();it!=flags.end();it++)if(h.getHash()==(*it))return;
			flags.push_back(h.getHash());
		}
		void remove(StringHash h){
			for(FlagList::iterator it=flags.begin();it!=flags.end();it++)
				if(h.getHash()==*it){flags.erase(it);return;}
		}

    };
}

