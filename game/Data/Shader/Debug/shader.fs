#version 130

varying vec2 tc;
uniform sampler2D tex;
uniform sampler2D shadowMaps[10];

void main () {

	gl_FragColor = texture (shadowMaps[0], tc);
}