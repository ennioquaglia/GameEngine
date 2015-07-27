#version 330

varying vec2 tc;

 
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform sampler2D rnm;
uniform sampler2D colorMap;

uniform mat4 PM,CM;


 
uniform float distanceThreshold=10;
uniform vec2 filterRadius=vec2(10./800.,10./600.);
 
const int sample_count = 16;
const vec2 poisson16[] = vec2[](    // These are the Poisson Disk Samples
                                vec2( -0.94201624,  -0.39906216 ),
                                vec2(  0.94558609,  -0.76890725 ),
                                vec2( -0.094184101, -0.92938870 ),
                                vec2(  0.34495938,   0.29387760 ),
                                vec2( -0.91588581,   0.45771432 ),
                                vec2( -0.81544232,  -0.87912464 ),
                                vec2( -0.38277543,   0.27676845 ),
                                vec2(  0.97484398,   0.75648379 ),
                                vec2(  0.44323325,  -0.97511554 ),
                                vec2(  0.53742981,  -0.47373420 ),
                                vec2( -0.26496911,  -0.41893023 ),
                                vec2(  0.79197514,   0.19090188 ),
                                vec2( -0.24188840,   0.99706507 ),
                                vec2( -0.81409955,   0.91437590 ),
                                vec2(  0.19984126,   0.78641367 ),
                                vec2(  0.14383161,  -0.14100790 )
                               );
 
vec3 decodeNormal(in vec2 coord)
{
    // restore normal
    vec4 n=CM*vec4((texture2D(normalMap,coord)*2.-1.).xyz,0.);
	n.xyz/n.w;
	return normalize(n.xyz);
}

float getDepth(in vec2 coord){
	float f=500.0;
	float n = 0.1;
	return (2 * n) / (f + n - texture( depthMap, coord ).x * (f - n));
}
float linearizeDepth(in float depth){
	float f=500.0;
	float n = 0.1;
	return (2 * n) / (f + n - depth * (f - n));
}
vec3 calculatePosition(in vec2 coord)
{
	float z = getDepth(coord);
	vec4 v;
    v=inverse(PM)*vec4(vec3(coord,z)*2.-1,1.);
	v.xyz/=v.w;
	return v.xyz;
}
 
 
vec4 ComputeReflection()
{
	//Tweakable variables
	float initialStepAmount = .01;
	float stepRefinementAmount = .7;
	int maxRefinements = 3;
	int maxDepth = 1;

	//Values from textures
	vec2 screenSpacePosition2D = tc;
	vec3 cameraSpacePosition = calculatePosition(screenSpacePosition2D);
	vec3 cameraSpaceNormal = decodeNormal(tc);
	float roughness = 1.;//texture(otherTexture,screenSpacePosition2D).x;
	float reflectivity =1. ;//texture(otherTexture,screenSpacePosition2D).y;

	//Screen space vector
	vec3 cameraSpaceViewDir = normalize(cameraSpacePosition);
	vec3 cameraSpaceVector = normalize(reflect(cameraSpaceViewDir,cameraSpaceNormal));
	vec3 screenSpacePosition = normalize(cameraSpacePosition);//convertCameraSpaceToScreenSpace(cameraSpacePosition);
	vec3 cameraSpaceVectorPosition = cameraSpacePosition + cameraSpaceVector;
	vec3 screenSpaceVectorPosition =cameraSpacePosition + cameraSpaceVector;// convertCameraSpaceToScreenSpace(cameraSpaceVectorPosition);
	vec3 screenSpaceVector = initialStepAmount*normalize(screenSpaceVectorPosition - screenSpacePosition);

	//Jitter the initial ray
	//float randomOffset1 = clamp(rand(gl_FragCoord.xy),0,1)/1000.0;
	//float randomOffset2 = clamp(rand(gl_FragCoord.yy),0,1)/1000.0;
	//screenSpaceVector += vec3(randomOffset1,randomOffset2,0);
	vec3 oldPosition = screenSpacePosition + screenSpaceVector;
	vec3 currentPosition = oldPosition + screenSpaceVector;

	//State
	vec4 color = vec4(0,0,0,1);
	int count = 0;
	int numRefinements = 0;
	int depth = 0;
	return vec4(0.5);
	//Ray trace!
	while(depth < maxDepth) //doesnt do anything right now
	{
		while(count < 1000)
		{
			//Stop ray trace when it goes outside screen space
			if(currentPosition.x < 0 || currentPosition.x > 1 ||
			   currentPosition.y < 0 || currentPosition.y > 1 ||
			   currentPosition.z < 0 || currentPosition.z > 1)
				break;

			//intersections
			vec2 samplePos = currentPosition.xy;
			float currentDepth = linearizeDepth(currentPosition.z);
			float sampleDepth = getDepth( samplePos);
			float diff = currentDepth - sampleDepth;
			float error = length(screenSpaceVector);
			if(diff >= 0 && diff < error)
			{
				
				screenSpaceVector *= stepRefinementAmount;
				currentPosition = oldPosition;
				numRefinements++;
				if(numRefinements >= maxRefinements)
				{
				
					vec3 normalAtPos = texture(normalMap, samplePos).xyz;
					float orientation = dot(cameraSpaceVector,normalAtPos);
					if(orientation < 0)
					{
						float cosAngIncidence = -dot(cameraSpaceViewDir,cameraSpaceNormal);
						cosAngIncidence = clamp(1-cosAngIncidence,0.0,1.0);
						color = texture(colorMap, samplePos) * cosAngIncidence;
						return vec4(0.5);//color;
					}
					break;
				}
			}

			//Step ray
			oldPosition = currentPosition;
			currentPosition = oldPosition + screenSpaceVector;
			count++;
		}
		depth++;
	}
	return color;
}
 
 
 
 
 
 
uniform vec2 TexelSize=512/vec2(800,600);


/// <summary>
/// Occluder bias to minimize self-occlusion.
/// <summary>
uniform float OccluderBias=0.0005;


/// <summary>
/// Specifies the size of the sampling radius.
/// <summary>
uniform float SamplingRadius=0.08;
uniform vec2 Attenuation=vec2(1.5,5);

 float SamplePixels (vec3 srcPosition, vec3 srcNormal, vec2 uv)
{
	uv=clamp(uv,0.,1.);
	// Get the 3D position of the destination pixel
	vec3 dstPosition = calculatePosition(uv);//texture2D(Sample0, uv).xyz;

	// Calculate ambient occlusion amount between these two points
	// It is simular to diffuse lighting. Objects directly above the fragment cast
	// the hardest shadow and objects closer to the horizon have minimal effect.
	vec3 positionVec = dstPosition - srcPosition;
	float intensity = max(dot(normalize(positionVec), srcNormal) - OccluderBias, 0.0);

	// Attenuate the occlusion, similar to how you attenuate a light source.
	// The further the distance between points, the less effect AO has on the fragment.
	float dist = length(positionVec);
	float attenuation = 1.0 / (Attenuation.x + (Attenuation.y * dist));
	
	
	
	return (srcPosition.z <= dstPosition.z ? 1.0 : 0.0)* attenuation ;
	return intensity * attenuation;
}


/// <summary>
/// Fragment shader entry.
/// <summary>
void main ()
{

	if(texture(normalMap,tc).w==0.)discard;
	
	// Get position and normal vector for this fragment
	vec3 srcPosition = calculatePosition(tc);//texture2D(Sample0, tc).xyz;
	vec3 srcNormal = decodeNormal(tc);//texture2D(Sample1, tc).xyz;
	vec2 randVec = normalize(texture2D(rnm, tc).xy * 2.0 - 1.0);
	float srcDepth = srcPosition.z;//texture2D(Sample0, tc).w;
	
	// The following variable specifies how many pixels we skip over after each
	// iteration in the ambient occlusion loop. We can't sample every pixel within
	// the sphere of influence because that's too slow. We only need to sample
	// some random pixels nearby to apprxomate the solution.
	//
	// Pixels far off in the distance will not sample as many pixels as those close up.
	float kernelRadius = SamplingRadius * (1.0 - srcDepth);
	
	// Sample neighbouring pixels
	vec2 kernel[4];
	kernel[0] = vec2(0.0, 1.0);		// top
	kernel[1] = vec2(1.0, 0.0);		// right
	kernel[2] = vec2(0.0, -1.0);	// bottom
	kernel[3] = vec2(-1.0, 0.0);	// left
	
	const float Sin45 = 0.707107;	// 45 degrees = sin(PI / 4)
	
	// Sample from 16 pixels, which should be enough to appromixate a result. You can
	// sample from more pixels, but it comes at the cost of performance.
	float occlusion = 0.0;
	for (int i = 0; i < 4; ++i)
	{
		vec2 k1 = reflect(kernel[i], randVec);
		vec2 k2 = vec2(k1.x * Sin45 - k1.y * Sin45,
					   k1.x * Sin45 + k1.y * Sin45);
		k1 *= TexelSize;
		k2 *= TexelSize;
		
		occlusion += SamplePixels(srcPosition, srcNormal, tc + k1 * kernelRadius);
		occlusion += SamplePixels(srcPosition, srcNormal, tc + k2 * kernelRadius * 0.75);
		occlusion += SamplePixels(srcPosition, srcNormal, tc + k1 * kernelRadius * 0.5);
		occlusion += SamplePixels(srcPosition, srcNormal, tc + k2 * kernelRadius * 0.25);
	}
	
	// Average and clamp ambient occlusion
	occlusion /= 16.0;
	occlusion = clamp(occlusion, 0.0, 1.0);
	
	gl_FragColor.a = (1.-pow(occlusion,2.));
	
	//  gl_FragColor.xyz=ComputeReflection().xyz;//color.xyz;
   return;
	/*
	vec3 reflection=vec3(0);
	    
	float maxDelta=0.1;
	float stepSize=0.01;
	float fade = 5.0;
	vec3 reflected = reflect(normalize(srcPosition), srcNormal);
	vec3 vectorSS = normalize(reflected)*stepSize;
	
	vec2 samplePos = vec2(0);   // texcoord for the depth and color
    float sampleDepth = 0;  // depth from texture
    float currentDepth = 0; // current depth calculated with reflection vector
    float deltaD = 0;
    vec4 color = vec4(0);
    for (int i = 1; i < 35; i++)
    {
        samplePos = srcPosition.xy + vectorSS.xy*i;
        currentDepth = srcPosition.z + vectorSS.z*i;        
        sampleDepth =getDepth(samplePos) ;
        deltaD = currentDepth - sampleDepth;
        if ( deltaD > 0 && deltaD < maxDelta)
        {
            color = texture(colorMap, samplePos);
            //color.xyz *= fade / i;
            break;
        }
    }
   gl_FragColor.xyz=ComputeReflection().xyz;//color.xyz;
   return;
	
	

	
	/*
	vec2 samplePos=srcPosition.xy;
	vec2 sampleDir=normalize(-reflected.xy)/20.;
	
	for(int i=0;i<10;i++){
			samplePos+=sampleDir;
			
			float sampleDepth=getDepth(samplePos);
			
			float deltaD = srcPosition.z - sampleDepth;
			//if ( deltaD > 0 && deltaD < 1.)
			{
				reflection += texture(colorMap, samplePos).xyz/5.;///deltaD;
				//reflection/= distance(srcPosition,calculatePosition(samplePos));
				//break;
			}
	}
	
	gl_FragColor.xyz=reflection;
	
	*/
}