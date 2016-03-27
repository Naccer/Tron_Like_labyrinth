#version 330 core

uniform sampler2D uEarthTexture;
uniform sampler2D uCloudTexture;
in vec2 vVertexTexture;
out vec3 fFragColor;

void main() {

	fFragColor = vec3(texture2D(uEarthTexture, vVertexTexture)) + vec3(texture2D(uCloudTexture, vVertexTexture));
}