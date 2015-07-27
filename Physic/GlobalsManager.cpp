
#include <map>
#include <string>

class GlobalsManager
{

	std::map<std::string, int> data;
	
	GlobalsManager() {}
public:
	static GlobalsManager* Istance(){
		static GlobalsManager *istance=new GlobalsManager();
		return istance;
	}
	
	int get(std::string key){
		if(data.find(key)!=data.end())return data[key];
		return 0;
	}
	void set(std::string key,int d){
		this->data[key]=d;
	}
	/*
	template<T> T get(std::string key){
		   return T;	
	}
	
	template<T> void set(std::string key,T data){
		//this->data[key]=data;
	}
*/

};
