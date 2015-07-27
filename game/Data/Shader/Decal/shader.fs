#version 330 core


varying vec3 v; 
varying vec3 N;
varying vec2 tex_coords_frag;
uniform sampler2D diffuseMap;

void main () {
	
	vec4 mat=texture(diffuseMap,tex_coords_frag);
	if(mat.a<0.2)discard;
	gl_FragColor = vec4(0.,1.,0.,1.0)+texture(diffuseMap,tex_coords_frag);   
}