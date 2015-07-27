#version 330 core

uniform sampler2D diffuseMap;
// Ouput data
//layout(location = 0) out float fragmentdepth;
varying vec2 tex_coords_frag;

void main(){
    // Not really needed, OpenGL does it anyway
	if(texture2D(diffuseMap,tex_coords_frag.xy).a<0.2)discard;
	//fragmentdepth = gl_FragCoord.z;
	//gl_FragColor=vec4(vec3(pos.z),1.);
	gl_FragColor = vec4(vec3(gl_FragCoord.z),1.);
}