

#if GLSL_VERSION>=330
	#version 330

	uniform sampler2D diffuseMap;
	// Ouput data
	layout(location = 0) out vec4 outColor;
	in vec2 TexCoord;
	in vec4 v_position; 
	void main(){
		float depth=gl_FragCoord.z;
		float moment1 = depth;
		float moment2 = depth * depth;
	
		// Adjusting moments (this is sort of bias per pixel) using partial derivative
		float dx = dFdx(depth);
		float dy = dFdy(depth);
		moment2 += 0.25*(dx*dx+dy*dy) ;
		
	
		outColor = vec4( moment1,moment2, 0.0, 0.0 );
	}

#else

	
	Comprati una scheda video nuova poveraccio!


#endif
	

	