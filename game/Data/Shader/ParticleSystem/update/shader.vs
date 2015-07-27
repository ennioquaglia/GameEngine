
varying vec2 Texcoord;
varying vec3 color;

void main() {
	color=gl_Color.xyz;
    Texcoord = gl_MultiTexCoord0.xy;
    gl_Position = vec4(gl_MultiTexCoord0.xy*2.0 - 1.0, 0.0, 1.0);
}

