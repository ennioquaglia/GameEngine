#version 330 core

uniform sampler2D diffuseMap;
uniform int isDeferred=1;

varying vec2 tex_coords_frag;


void main(){
    // Not really needed, OpenGL does it anyway

	if(isDeferred);else discard;
	if(texture2D(diffuseMap,tex_coords_frag).a<0.2)discard;

	float depth=gl_FragCoord.z;
	
	 
	//float dx = dFdx(depth);
	//float dy = dFdy(depth);
	//gl_FragColor = vec4(depth,depth*depth + 0.25*(x*dx + dy*dy), 0.0, 1.0);
	
	//gl_FragDepth=depth;
	//gl_FragColor = vec4(vec3(depth,0.,0.),1.);
	//float dx = dFdx(depth);
	//float dy = dFdy(depth);
	//gl_FragColor = vec4(vec3(depth),1.);//vec4(depth, pow(depth, 2.0) + 0.25*(dx*dx + dy*dy), 0.0, 1.0);*/
}