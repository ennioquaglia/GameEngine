#include "Parser.h"
#include "pugixml.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

#include "../Component/ComponentLibrary.h"
#include "../Core/Scene.h"

using namespace std;
using namespace glm;
using namespace Engine;


string path = "";

bool loadNode(Engine::Scene* scene, pugi::xml_node &node, mat4 matrix = mat4(1))
{ 
	mat4 m = matrix;
	Actor* actor = new Actor;
	//Debug(node.attribute("name").value());
	/*vec3 pos=vec3(0),scale=vec3(1);
	vec3 rot;

	pugi::xml_node child=node.child("position");
	if(child){
		string text=child.text().get();
		istringstream ss(text);
		for(int i=0;i<3;i++)ss>>pos[i];
	}
	child=node.child("scale");
	if(child){
		string text=child.text().get();
		istringstream ss(text);
		for(int i=0;i<3;i++)ss>>scale[i];  
	}
	child=node.child("rotation");
	if(child){
		string text=child.text().get();
		istringstream ss(text);
		vec3 vec;
		for(int i=0;i<3;i++)ss>>vec[i];
		rot=vec;
		rot[0]=vec[1];
		rot[1]=vec[2];
		rot[2]=vec[3];
		//rot[3]=vec[0];
		//rot=normalize(rot);
		//Debug(rot.x+" "+rot.y+" "+rot.z+" "+rot.w+" ");
	}
	TransformComponent* trans=new TransformComponent();
	//	trans->setPos(pos);
	//	trans->setScale(scale);
	 Rotation r;
	 r.setEulerAngles(rot);
	m=translate(mat4(1),pos)*r.getMatrix()*glm::scale(mat4(1),scale);
	trans->setMatrix(m,1);

	actor->addComponent(trans);
	/*/
	pugi::xml_node child = node.child("matrix");
	if(child) {
		TransformComponent* trans = new TransformComponent();


		if(string(child.attribute("sid").value()) == "transform") {
			float mat[16] = {0};
			string text = child.text().get();
			//Debug(text);
			istringstream ss(text);
			for(int i = 0; i < 16; i++)ss >> mat[i];
			mat4 axis = mat4(1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);

			//m*=(axis)*(((glm::make_mat4(mat))))*inverse(axis);
			//m=(m)*((glm::make_mat4(mat)))*(m);
			m = glm::make_mat4(mat);
			//(inverse(axis)*transpose(m)*(axis))
			//*scale(mat4(1),vec3(5));
			//rotate(mat4(1),89.0f,vec3(1,0,0))*
			//trans->setLocalMatrix(ComponentId::Render,m);
			trans->setMatrix((m), 1);
		} else {
			//Debug("\n\nposition scale rot\n\n");
			vec3 pos = vec3(0), scale = vec3(1);
			vec3 rot;
			pugi::xml_node child1 = child.child("position");
			if(child1) {
				string text = child1.text().get();
				istringstream ss(text);
				for(int i = 0; i < 3; i++)ss >> pos[i];
			//	Debug("\n\nposition="+pos+"\n\n");
			}
			child1 = child.child("scale");
			if(child1) {
				string text = child1.text().get();
				istringstream ss(text);
				for(int i = 0; i < 3; i++)ss >> scale[i];
			}
			child1 = child.child("rotation");
			if(child1) {
				string text = child1.text().get();
				istringstream ss(text);
				vec3 vec;
				for(int i = 0; i < 3; i++)ss >> vec[i];
				//rot = vec;
				rot[0] = vec[0];
				rot[1] =vec[1];
				rot[2] = vec[2]; 
				//Debug(rot);
			}
			trans->setPos(pos);
			trans->setScale(scale);
			Rotation r;
			r.setEulerAngles(rot); 
			trans->setRotation(r); 
			//m = translate(mat4(1), pos)    * r.getMatrix()* glm::scale(mat4(1), scale);
			//trans->setMatrix(m, 1);
		}



		//trans->setMatrix(translate(mat4(1),vec3(5,5,5)));
		//Debug(translate(mat4(1),vec3(5,5,5)));
		//Debug(trans->getMatrix());
		/*Debug(m);
		Debug(trans->getMatrix());
		Debug(trans->getScale());
		Debug(vec3(m[0][0],m[1][1],m[2][2]));
		Debug(trans->getPos());  */
		//Debug(vec3(m*vec4(0,0,0,1)));
		//Debug(trans->getPos(1));
		actor->addComponent(trans);
	}//*/


	child = node.child("MeshGraphicComponent");
	if(child) {
		//Debug(path + child.attribute("src").value());
		MeshGraphicComponent* mesh = new MeshGraphicComponent(path + child.attribute("src").value());
		
		for (child= child.child("material"); child; child = child.next_sibling("material"))
		{
			Material* mat=new Material();
			pugi::xml_node child1 = child.child("diffuse");
			if(child1){
				mat->setDiffuse(AssetManager::getBasePath() +child1.text().get());
				//Debug(AssetManager::getBasePath() +child1.text().get());
			}
			child1 = child.child("normal");
			if(child1)mat->setNormal(AssetManager::getBasePath() +child1.text().get());
			child1 = child.child("specular");
			if(child1)mat->setSpecular(AssetManager::getBasePath() +child1.text().get());
			child1 = child.child("shininess");
			if(child1){
				istringstream ss(child1.text().get());
				float shininess;
				ss >> shininess;
				mat->setShininess(shininess);
			}

		    mesh->addMaterial(mat);
		}
		mesh->setTextureMatrix(mat2(scale(mat4(1),vec3(1000))));
		actor->addComponent(mesh);
	}

	child = node.child("StaticMeshStaticPhysicsComponent");
	if(child) {
		StaticMeshStaticPhysicsComponent* mesh = new StaticMeshStaticPhysicsComponent(path + child.attribute("src").value());
		if(child.attribute("mass")) {

			istringstream ss(child.attribute("mass").value());
			float mass;
			ss >> mass;
			//mesh->setMass(mass);
			//Debug("massssss "+mass);
		}

		actor->addComponent(mesh);
	}

	scene->addActor(actor);

	//child=node.child("node");
	for (pugi::xml_node tool = node.child("node"); tool; tool = tool.next_sibling("node")) {
		loadNode(scene, tool, m);
	}
	return 1;

}
bool loadScene(string filename, Engine::Scene* scene)
{

	path = filename;
	filename += "level.xml";
	ifstream s(filename.data());
	if(!s) {
		cout << "file doesn't exist\n";
		return 0;
	}
	s.close();
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.data());

	if (result)
		std::cout << "XML [" << filename << "] parsed without errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n\n";
	else {
		std::cout << "XML [" << filename << "] parsed with errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n";
		std::cout << "Error description: " << result.description() << "\n";
		std::cout << "Error offset: " << result.offset << " (error at [..." << (filename.data() + result.offset) << "]\n\n";
		return 0;
	}
	pugi::xml_node root = doc.child("scene");

	for (pugi::xml_node_iterator it = root.begin(); it != root.end(); ++it) {
		loadNode(scene, (*it));
	}
	return 1;

}
