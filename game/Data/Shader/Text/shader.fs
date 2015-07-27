#version 330

varying vec2 tc;
uniform sampler2D char;
uniform int charId=0;
uniform vec4 color=vec4(1.,0.,0.,1.);

vec2 getChar(int i){

	float unit=1.0/16.0;
	float x=trunc(mod(i,16.0));
	float y=trunc(i/16);
	return vec2(x*unit,y*unit);
}

void main () {

	vec2 coord=tc/16.0+getChar(charId);

	gl_FragColor = texture(char,coord)*color;;
	if(gl_FragColor.a<0.3)discard;
	gl_FragColor.a=1.;
}