#version 120
//#version 330 core

#define MAX_LIGHTS 128

varying vec4 color=vec4(1.);

varying vec3 v; 
varying vec3 N;
varying vec2 tex_coords_frag;
uniform sampler2D diffuseMap;

void main () {

	//gl_FragColor=color;
	//gl_FragData[0] = vec4(0.);//vec3(gl_FragCoord.z),1.);//depth 

	
	gl_FragData[1] = vec4(1.0); //normal
	//gl_FragData[1].w=0.0;
	
	gl_FragData[2] = color;    //texture
	gl_FragData[3] = color;

}