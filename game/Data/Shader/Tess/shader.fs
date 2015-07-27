#version 430


/*//////////////////////////////
#define MAX_LIGHTS 32
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform sampler2D HeightMap;

uniform vec4 LightPos[MAX_LIGHTS];
uniform int numLights;

varying vec2 tex_coords_frag;
in vec3 v;

in vec3 gFacetNormal;
in vec3 gTriDistance;
in vec3 gPatchDistance;
in vec3 gPrimitive;
*///////////////////////////////
/*


//enum
#define Shadow 1
#define Tessellation 2
#define Light 4

#define MAX_LIGHTS 256
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform sampler2D shadowMap;

uniform vec4 LightPos[MAX_LIGHTS];
uniform int numLights;
uniform vec3 EyeWorldPos;

uniform int flag;


varying vec4 color;
varying vec2 tex_coords_frag;
varying vec4 ShadowCoord;
varying vec3 v;   				//vertex position
varying vec3 N;					//normal
varying vec3 T;					//tangent


vec2 poissonDisk[4] = vec2[4](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

vec3 CalcBumpedNormal()
{
    vec3 Normal = normalize(N);
    vec3 Tangent = normalize(T);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent, Normal);
    vec3 BumpMapNormal = texture2D(normalMap, tex_coords_frag).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}

*/
void main () {
	gl_FragColor=vec4(1,0.1,1,1);
	
	/*//////////////////////////////
	float visibility=1.0;
	vec4 fragColor=vec4(0.0);
	vec3 n=CalcBumpedNormal() ;
	vec4 diffMat= texture2D(diffuseMap,tex_coords_frag);
	vec4 specMat = texture2D(specularMap,tex_coords_frag);
	
	for(int i=0;i<numLights&&i<MAX_LIGHTS;i++){

		float dist=	length(LightPos[i].xyz- v);
		vec3 L = normalize(LightPos[i].xyz- v);   
		vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)    <--- prima era normalize(-v);
		vec3 R = normalize(-reflect(L,n));  
		
		
		//calculate Diffuse Term:  
		//vec4(0.5,0.5,0.5,0.5);
		vec4 Idiff = diffMat * max(dot(n,L), 0.0);
		Idiff = clamp(Idiff, 0.0, 1.0);     
	   
		// calculate Specular Term:
		/*
		vec4 Ispec =specMat* pow(max(dot(R,E),0.0),100);
		Ispec = clamp(Ispec, 0.0, 1.0); 
		//*/
		/*
		vec3 VertexToEye = normalize( EyeWorldPos- v);
		
		vec3 reflection_eye = reflect (-VertexToEye, n);
		vec3 surface_to_viewer_eye = normalize (L);
		float dot_prod_specular = dot (reflection_eye, surface_to_viewer_eye);
		dot_prod_specular = max (dot_prod_specular, 0.0);
		float specular_factor = pow (dot_prod_specular, 10);
		vec4 Ispec =specMat*specular_factor;
		

		float att=max(1.0-(dist/LightPos[i].w),0.0);att*=att;
		fragColor+=(Idiff+Ispec)*att;
		
		
		
	}
	
	if(flag & Shadow)
	{
		//vec3 L=vec3(5,40,40);
		//float cosTheta=clamp(dot( n,L ),0,1);
		//float bias = 0.005*tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
		//bias = clamp(bias, 0,0.1);
		
		float bias=0.0005;
		
		if ( texture2D( shadowMap, ShadowCoord.xy ).z  <  ShadowCoord.z-bias){
			visibility = 0.5;
		}
		
		for (int i=0;i<4;i++){
		  if ( texture2D( shadowMap, ShadowCoord.xy + poissonDisk[i]/1000.0 ).z  <  ShadowCoord.z-bias ){
			visibility*=0.8;
		  }
		}
	}
	
	gl_FragColor=visibility*fragColor;//+texture(normalMap,tex_coords_frag);//+texture(normalMap,tex_coords_frag);//+vec4(tex_coords_frag,0.0,1.0);			
	gl_FragColor.a=1.0;
	*/

}