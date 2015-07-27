#version 130
//#version 330 core


uniform mat4 PM,CM,MVM;
out float Angle;
out float scale;

void main() 
{ 
	gl_Position = PM * CM * MVM * gl_Vertex;
	Angle=gl_MultiTexCoord0.x;
	scale=gl_MultiTexCoord0.y;
} 