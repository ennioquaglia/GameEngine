#version 150 core

layout (triangles) in;
layout (triangle_strip,  max_vertices=12) out;

uniform mat4 depthMVPs[4];
uniform mat4 MVM;

uniform int numMVPs;


out int shadowIndex;

void main() {
	for(int i=0;i<numMVPs;i++){
		shadowIndex=i;
		gl_Position = depthMVPs[i]*MVM*gl_in[0].gl_Position;
		EmitVertex();

		gl_Position = depthMVPs[i]*MVM*gl_in[1].gl_Position;
		EmitVertex();

		gl_Position = depthMVPs[i]*MVM*gl_in[2].gl_Position;
		EmitVertex();
		EndPrimitive();
	}
}