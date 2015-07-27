#include "Preprocessor.h"
#include "GlobalsManager.h"
#include <sstream>
#include <iostream>




#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

using namespace std;

#define min(a,b) a>b?b:a

// trim from start
std::string ltrim(std::string s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
std::string rtrim(std::string s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
std::string trim(std::string s) {
        return ltrim(rtrim(s));
}
 

void Preprocessor::addVoiceToDictionary(std::string key,std::string value) {
	dictionary[key]=value;
}

bool Preprocessor::isStringTrue(std::string comparision){
	unsigned int c=0,m=1000000;//min
	const int num=6;
	string operators[num]= {"<=",">=",">","<","==","!="};
	string keyword;
	
	for(int i = 0; i < num; i++)
	{
		if((c=comparision.find(operators[i]))!=string::npos)if(c<m) {
				m=c;
				keyword=operators[i];
			}
	}
	if(m==1000000){
			istringstream ss(comparision);
			string key;
			ss>>key;
			if(dictionary.find(key)!=dictionary.end()){
				   string val=dictionary[key];
				   if(val==""||val=="0"||val=="NULL")return 0;
				   else return 1;	
			}
		   return 0;	
	}
	
	comparision.replace(m,keyword.size()," ");
	string val1,val2;
	istringstream ss(comparision);
	ss>>val1>>val2;
	
	float v1=0,v2=0;
	
	if(dictionary.find(val1)!=dictionary.end()){
		   val1=dictionary[val1];	
	}
	{
		istringstream ss(val1);
		ss>>v1;
	}
	if(dictionary.find(val2)!=dictionary.end()){
		   val2=dictionary[val2];	
	} 
	{
		istringstream ss(val2);
		ss>>v2;
	}
	
	if(keyword=="==")return v1==v2;
	if(keyword=="!=")return v1!=v2;
	if(keyword=="<=")return v1<=v2;
	if(keyword==">=")return v1>=v2;
	if(keyword=="<")return v1<v2;
	if(keyword==">")return v1>v2;
	return 0;
}

void Preprocessor::printDictionary() {
	for(map<string,string>::iterator it=dictionary.begin(); it!=dictionary.end(); it++)
		cout<<it->first<<" = "<<it->second<<"\n";
	cout<<"\n";
}

std::string eraseComment(std::string source) {
	while(source.find("/*") != std::string::npos) {
		size_t Beg = source.find("/*");
		source.erase(Beg, (source.find("*/", Beg) - Beg)+2);
	}
	while(source.find("//") != std::string::npos) {
		size_t Beg = source.find("//");
		source.erase(Beg, source.find("\n", Beg) - Beg);
	}
	return source;
}


int getfirstKeyword(std::string source,unsigned  int cursor,string &keyword) {
	unsigned int c=0,m=1000000;//min
	const int num=13;
	string keywords[num]= {"#define","#undef","#ifdef","#ifndef","#endif","#if","#elif defined","#elifdef","#elif","#elseif defined","#elseifdef","#elseif","#else"};

	for(int i = 0; i < num; i++)
	{
		if((c=source.find(keywords[i],cursor))!=string::npos)if(c<m) {
				m=c;
				keyword=keywords[i];
			}
	}
	if(m==1000000)return 0;
	return m;
}

int getfirstTerminator(std::string source,unsigned int cursor,string &keyword) {
	unsigned int c=cursor-1,stack=0;
	while((c=getfirstKeyword(source,c+1,keyword))) {
		//cout<<keyword<<stack<<"-\n";
		if(keyword=="#define")continue;
		if(keyword=="#endif"){
			if(!stack)break;
			stack--;
		} else {
			if(keyword=="#elseif"||keyword=="#else"||keyword=="#elif"||keyword=="#elseifdef"||keyword=="#elifdef")if(!stack)break;
			stack++;
		}
	}
	return c;
}
bool isAlphanumericCharacter(char in){
	if(in>47&&in<58)return 1;//numeric
	if(in>64&&in<91)return 1;//Uppercase
	if(in>96&&in<123)return 1;//lowercase
	if(in=='_')	return 1;
	return 0;			
}
bool isAlphanumericCharacter(string in){
	return isAlphanumericCharacter(in[0]);
}
bool isSpaceChar(char c){
	if(c==' '||c=='\n'||c=='.'||c=='\t'||c=='-'||c=='('||c==')'||c=='='||c=='+'||c=='<'||c=='>'||c=='/')return 1;
	return 0;
}
bool isSpaceChar(string s){
	return isSpaceChar(s[0]);
}
string Preprocessor::parse(std::string source) {
	unsigned int cursor=0,stackSize=0;;
	source=eraseComment(source);
	string keyword;
	bool eraseNextBlock=0;
	while((cursor=getfirstKeyword(source,0,keyword)))
	{
	    
		if(keyword=="#define") {
			istringstream ss(source.substr(cursor,source.find("\n",cursor)-cursor));
			string key,value;
			ss>>key>>key>>value;
			addVoiceToDictionary(key,value);
			source.erase(cursor,source.find("\n",cursor)-cursor);
		}else if(keyword=="#undef"){
			istringstream ss(source.substr(cursor,source.find("\n",cursor)-cursor));
			string key;
			ss>>key>>key;
			dictionary.erase(key);
			source.erase(cursor,source.find("\n",cursor)-cursor);	
		} else if(keyword=="#ifdef"||keyword=="#elseif defined"||keyword=="#elif defined"||keyword=="#elifdef"||keyword=="#elseifdef") {
			if(keyword.substr(0,3)!="#el")stackSize++;
			istringstream ss(source.substr(cursor,source.find("\n",cursor)-cursor));
			string key,value;
			ss>>key>>key;
			if(key=="defined")ss>>key;
			if(dictionary.find(key)!=dictionary.end()&&!eraseNextBlock) {
				eraseNextBlock=1;
				source.erase(cursor,source.find("\n",cursor)-cursor);
			} else {
				int c=getfirstTerminator(source,cursor+1,keyword);
				source.erase(cursor,c-cursor);
			}
		}
		else if(keyword=="#ifndef"){
			stackSize++;
			istringstream ss(source.substr(cursor,source.find("\n",cursor)-cursor));
			string key,value;
			ss>>key>>key;
			if(dictionary.find(key)==dictionary.end()&&!eraseNextBlock) {
				eraseNextBlock=1;
				source.erase(cursor,source.find("\n",cursor)-cursor);
			} else {
				unsigned int c=getfirstTerminator(source,cursor+1,keyword);
				source.erase(cursor,c-cursor);
			}
		}
		else if(keyword=="#if"||keyword=="#elseif"||keyword=="#elif") {
			if(keyword.substr(0,3)!="#el")stackSize++;
			string comparision=source.substr(cursor+keyword.length(),source.find("\n",cursor)-(cursor+keyword.length()));

			if(isStringTrue(comparision)&&!eraseNextBlock) {
				eraseNextBlock=1;
				source.erase(cursor,source.find("\n",cursor)-cursor);
			} else {
				unsigned int c=getfirstTerminator(source,cursor+1,keyword);
				source.erase(cursor,c-cursor);
			}
		}
		else if(keyword=="#else") {
			if(eraseNextBlock) {
				unsigned int c=getfirstTerminator(source,cursor+1,keyword);
				source.erase(cursor,c-cursor);
			} else {
				source.erase(cursor,source.find("\n",cursor)-cursor);
			}
			eraseNextBlock=0;
		}
		else if(keyword=="#endif") {
			source.erase(cursor,min(source.find("\n",cursor)-cursor,6));
			eraseNextBlock=0;
			if(stackSize)stackSize--;
			else cout<<"preprocessor parser error: too much #endif\n";
		}
		else cout<<"\n\nerror: cannot parse \""<<keyword<<"\"\n\n";

	}

	cursor=-1;
	bool repeat;
	do{
		repeat=0;
		for(std::map<string,string>::iterator it=dictionary.begin();it!=dictionary.end();it++){
			while((cursor=source.find(it->first,cursor+1)) != std::string::npos){
				if((!isAlphanumericCharacter(source.substr(cursor-1,1)))&&(!isAlphanumericCharacter(source.substr(cursor+(it->first).length(),1)))){
					source.replace(cursor,it->first.length(),it->second);
					repeat=1;
				}
			}	
		} 
	}while(repeat);  	

	return trim(source);
}

string GLSL_Preprocessor::parse(std::string source) {
	//string version=Engine::stoi((int)(GlobalsManager::Istance()->get("glsl_version")*100));
	unsigned int cursor=0;

	while((cursor=source.find("#define",cursor+1))!=string::npos) {
		istringstream ss(source.substr(cursor,50));
		string key,value;
		ss>>key>>key>>value;
		addVoiceToDictionary(key,value);
	}

	return source;

}