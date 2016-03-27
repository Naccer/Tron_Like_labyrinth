#version 330 core

layout(location = 1) in vec3 aVertexPosition;
layout(location = 2) in vec3 aVertexNormal;
layout(location = 3) in vec2 aVertexTexCoords;

uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uNormalMatrix;

out vec2 vFragTexture;
out vec3 vNormal;
out vec3 vPosition;

void main() 
{
    vec4 vertexPosition = vec4(aVertexPosition, 1);
    vec4 vertexNormal = vec4(aVertexNormal, 0);

    vPosition = vec3(uMVMatrix * vertexPosition);
    vNormal = vec3(uNormalMatrix * vertexNormal);
    vFragTexture = aVertexTexCoords;

    gl_Position = uMVPMatrix * vertexPosition;
}
