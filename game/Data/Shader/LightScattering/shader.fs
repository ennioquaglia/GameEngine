#version 330

varying vec2 tc;
uniform sampler2D firstPass;
uniform sampler2D normal;

uniform float exposure=0.0041;
uniform float decay=0.9994;
uniform float density=0.8407;
uniform float weight=5.65;


uniform vec2 lightPositionOnScreen;
const int NUM_SAMPLES = 100 ;


void main()
{	
	vec2 deltaTextCoord = vec2( tc - lightPositionOnScreen.xy );
	vec2 textCoo =tc;// gl_TexCoord[0].st;
	deltaTextCoord *= 1.0 /  float(NUM_SAMPLES) * density;
	float illuminationDecay = 1.0;


	for(int i=0; i < NUM_SAMPLES ; i++)
	{
			 textCoo -= deltaTextCoord;
			 vec4 sample = texture2D(firstPass, textCoo );
		
			 sample *= illuminationDecay * weight;

			 gl_FragColor += sample;

			 illuminationDecay *= decay;
	 }
	 gl_FragColor *= exposure;
	gl_FragColor =gl_FragColor+texture2D(normal, tc );
	float contrast=1.2;
	gl_FragColor.rgb = ((gl_FragColor.rgb - 0.5f) * max(contrast, 0)) + 0.5f;
	//gl_FragColor.xyz=gl_FragColor.xyz*pow(gl_FragColor.xyz,vec3(0.05));//cross(gl_FragColor.xyz,vec3(1.));
}
