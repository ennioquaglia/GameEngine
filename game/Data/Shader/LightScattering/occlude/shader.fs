

varying vec2 tc;
uniform sampler2D texture;
uniform sampler2D char;
uniform vec2 size;
vec2 pixel=vec2(100,50);

vec2 getChar(float i){
	i*=255;
	float unit=1.0/16.0;
	float x=trunc(mod(i,16.0));
	float y=trunc(i/16);
	return vec2(x*unit,y*unit);
}

void main () {
	vec4 frag=vec4(0.);
	float dx = (1./pixel.x);
	 float dy =(1./pixel.y);
	 vec2 coord = vec2(dx*floor(tc.x/dx),
					   dy*floor(tc.y/dy));
					   
	frag+=texture (texture, coord);
	float c=frag.r+frag.b+frag.b;
	c/=3.0;

	coord=mod(tc*pixel,1.0);
	//gl_FragColor =texture (texture,tc);//
	
	//uncomment to get matrix mode
	coord=coord/16.0+getChar(c);
	coord.y=-coord.y;
	gl_FragColor =( texture(char,coord))*vec4(0.,1.0,0.,1.)*c;//vec4((coord+getChar(c)),0,1);//texture (char,(tc/16+getChar(c))*pixel);//vec4(0.,0.,0.,1.)+frag;
	gl_FragColor.a=1.;
}