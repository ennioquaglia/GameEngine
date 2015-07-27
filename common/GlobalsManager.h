#pragma once
#include <map>
#include <string>
#include "Debug.h"

class GlobalsManager
{

	std::map<std::string, float> data;
	
	GlobalsManager() {}
public:
	static GlobalsManager* Istance(){
		static GlobalsManager *istance=new GlobalsManager();
		//istance->cont();
		return istance;
	}
	void count(){
		static int c=0;
		Debug("global manager invoke n:"+c);
		c++;
		for(std::map<std::string, float>::iterator it=data.begin();it!=data.end();it++)
			Debug("["+it->first+"]="+it->second);
	}
	float get(std::string key){
		if(data.find(key)!=data.end())return data[key];
		return -1.;
	}
	void set(std::string key,float d){
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

