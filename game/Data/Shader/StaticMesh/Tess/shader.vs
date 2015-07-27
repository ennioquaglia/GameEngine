#version 330

attribute vec3 vertex_position_modelspace;
attribute vec3 normal_modelspace;
attribute vec2 tex_coords;
attribute vec3 tangent;


out vec3 vPosition;
out vec3 vNormal;
out vec3 vTangent;
out vec2 vCoord;

uniform mat2 textureMatrix;

void main()
{
	vTangent=tangent;
	vec4 position = vec4(vertex_position_modelspace, 1.0);
    vPosition=position.xyz;
	gl_Position =  position;
	vNormal=normalize( normal_modelspace);
	vCoord=textureMatrix*vec2(tex_coords.x,1.0-tex_coords.y);
    //vPosition=gl_Position = gl_ProjectionMatrix *gl_ModelViewMatrix  * gl_Vertex;
}
