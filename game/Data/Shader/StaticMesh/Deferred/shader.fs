#version 330

//enum
#define Shadow 1
#define Tessellation 2
#define Light 4


#define MAX_LIGHTS 128

//material
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform sampler2D emissiveMap;
uniform samplerCube enviromentMap;
uniform float shininess=0.99;
uniform float metallic=0.;

uniform mat4 PM,CM;

uniform vec3 EyeWorldPos;

uniform int flag;
uniform int isDeferred=1;
uniform vec3 emissiveAmbient=vec3(0.);

varying vec4 color;
varying vec3 normal_camera;
varying vec2 tex_coords_frag;
varying vec4 ShadowCoord;
varying vec3 v;   				//vertex position
varying vec3 N;					//normal
varying vec3 T;					//tangent

vec3 CalcBumpedNormal()
{
	if(T==vec3(0.))return normalize(N);
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

void main () {

	float visibility=1.0;
	vec4 fragColor=vec4(0.0);
	vec3 n=CalcBumpedNormal() ;
	vec4 diffMat= texture2D(diffuseMap,tex_coords_frag);
	vec4 specMat = texture2D(specularMap,tex_coords_frag);
	vec4 emissiveFactor=texture2D(emissiveMap,tex_coords_frag)+vec4(emissiveAmbient,1.);
	
	if(diffMat.a<0.1)discard;
	
	vec3 incident_eye = normalize(vec3(CM*vec4(v,1.)));
	vec3 normal =normalize(vec3(CM*vec4(n,0.)));;//vec3 (inverse (CM) * vec4 (n, 0.0));;// normalize (n);

	vec3 reflected = reflect (incident_eye, normal);
	// convert from eye to world space
	reflected = vec3 (inverse (CM) * vec4 (reflected, 0.0));
	
	
	
	//gl_FragData[0] = vec4(0.);//vec3(gl_FragCoord.z),1.);//depth 

	
	gl_FragData[1] = vec4((n+1.)*0.5,1.0); //normal
	//if(!isDeferred)gl_FragData[1].w=0.0;
	gl_FragData[2] =vec4( vec3(diffMat*color)+metallic*texture (enviromentMap, reflected).xyz,shininess);    //texture

	
	gl_FragData[3] = specMat*color+metallic*texture (enviromentMap, reflected);
	gl_FragData[4] = vec4(emissiveAmbient,1.);

}