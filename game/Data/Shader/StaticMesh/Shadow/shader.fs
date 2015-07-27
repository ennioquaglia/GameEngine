#version 330 core

uniform sampler2D diffuseMap;
// Ouput data
//layout(location = 0) out float fragmentdepth;
varying vec2 tex_coords_frag;

void main(){
    // Not really needed, OpenGL does it anyway

	if(texture2D(diffuseMap,tex_coords_frag).a<0.2)discard;
	//fragmentdepth = gl_FragCoord.z;
	/*float depth=gl_FragCoord.z;
	float dx = dFdx(depth);
	float dy = dFdy(depth);
	gl_FragColor = vec4(vec3(depht),1.);//vec4(depth, pow(depth, 2.0) + 0.25*(dx*dx + dy*dy), 0.0, 1.0);*/
}