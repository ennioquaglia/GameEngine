#version 330 core

varying vec2 tc;
uniform sampler2D depth_tex;

void main () {
	gl_FragColor = texture (depth_tex, tc)*0.5;
}