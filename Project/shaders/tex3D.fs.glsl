#version 330 core

in vec2 vFragTexture;
in vec3 vNormal;
in vec3 vPosition;

uniform vec3 uColor;
uniform sampler2D uTexture;
out vec3 fFragColor;

void main() {
  vec4 tex = texture(uTexture, vFragTexture);
  fFragColor = vNormal;
  
};
