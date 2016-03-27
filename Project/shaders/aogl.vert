#version 410 core

#define POSITION	0
#define NORMAL		1
#define TEXCOORD	2
#define FRAG_COLOR	0

layout(location = POSITION) in vec3 Position;
layout(location = NORMAL) in vec3 Normal;
layout(location = TEXCOORD) in vec2 Texcoord;

uniform mat4 uModelMatrix;

out block
{
	vec3 Position;
	vec2 Texcoord;
} Out;

void main()
{	
	Out.Texcoord = Texcoord;
	//gl_Position = vec4(Position, 1.0);

	//vFragTexture = Texcoord;//vec2(0.0, 0.0);
  	gl_Position = vec4(uModelMatrix*vec4(Position,1));
  	//gl_Position = vec4(pos.xy,0,1);
  	//vPosition = gl_Position.xy;
}
