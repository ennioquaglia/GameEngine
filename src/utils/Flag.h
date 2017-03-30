#pragma once

namespace Engine {


	class Flag
	{

	public:
		Flag() :data(0) {}
		Flag(unsigned int flag) :data(flag) {}

        const bool operator==(const unsigned int &lx) const{
			return data == lx;
        }
        bool contains(unsigned int lx) const{
            return (data&lx)==lx;
        }

        const bool operator&(const unsigned int &lx) const{
			return data & lx;
		}
        void add(const unsigned int h) {
			data |= h;
		}
        void remove(const unsigned int h) {
            data &= !h;
		}
        unsigned int data;

	};


}
