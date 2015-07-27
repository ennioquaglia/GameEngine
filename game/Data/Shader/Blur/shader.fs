#version 130

varying vec2 tc;
uniform sampler2D inputTexture;
uniform float blur;
uniform bool axis;

void main () {
	vec4 sum = vec4(0.0);
	
	//if(0)
	float blur1=0.0008;
	if(axis)
	{
		//blur in horizontal
		float blurSize=blur1;
		sum += texture(inputTexture, vec2(tc.x, tc.y - 4.0*blurSize)) * 0.05;
		sum += texture(inputTexture, vec2(tc.x, tc.y - 3.0*blurSize)) * 0.09;
		sum += texture(inputTexture, vec2(tc.x, tc.y - 2.0*blurSize)) * 0.12;
		sum += texture(inputTexture, vec2(tc.x, tc.y - blurSize)) * 0.15;
		sum += texture(inputTexture, vec2(tc.x, tc.y)) * 0.18;
		sum += texture(inputTexture, vec2(tc.x, tc.y + blurSize)) * 0.15;
		sum += texture(inputTexture, vec2(tc.x, tc.y + 2.0*blurSize)) * 0.12;
		sum += texture(inputTexture, vec2(tc.x, tc.y + 3.0*blurSize)) * 0.09;
		sum += texture(inputTexture, vec2(tc.x, tc.y + 4.0*blurSize)) * 0.05;
	}
	else{ 
		float blurSize=blur1;
		// blur in y (vertical)
		// take nine samples, with the distance blurSize between them
		sum += texture(inputTexture, vec2(tc.x - 4.0*blurSize, tc.y)) * 0.05;
		sum += texture(inputTexture, vec2(tc.x - 3.0*blurSize, tc.y)) * 0.09;
		sum += texture(inputTexture, vec2(tc.x - 2.0*blurSize, tc.y)) * 0.12;
		sum += texture(inputTexture, vec2(tc.x - blurSize, tc.y)) * 0.15;
		sum += texture(inputTexture, vec2(tc.x, tc.y)) * 0.18;
		sum += texture(inputTexture, vec2(tc.x + blurSize, tc.y)) * 0.15;
		sum += texture(inputTexture, vec2(tc.x + 2.0*blurSize, tc.y)) * 0.12;
		sum += texture(inputTexture, vec2(tc.x + 3.0*blurSize, tc.y)) * 0.09;
		sum += texture(inputTexture, vec2(tc.x + 4.0*blurSize, tc.y)) * 0.05;
	}
	
	
	gl_FragData[0] =sum;
	gl_FragData[0].a =1.;
}