#version 410 core

#define POSITION	0
#define NORMAL		1
#define TEXCOORD	2
#define FRAG_COLOR	0

layout(location = POSITION) in vec3 Position;
layout(location = NORMAL) in vec3 Normal;
layout(location = TEXCOORD) in vec2 Texcoord;

uniform mat4 uModelMatrix;
uniform sampler2D uTexture1;
uniform sampler2D uTexture2;
uniform float uTime;

out block
{
	vec3 Position;
	vec2 Texcoord;
	vec3 Texture1;
	vec3 Texture2;
	float uTime;
} Out;

void main()
{	
	Out.Texcoord = Texcoord;
	//gl_Position = vec4(Position, 1.0);

	//vFragTexture = Texcoord;//vec2(0.0, 0.0);
  	gl_Position = vec4(uModelMatrix*vec4(Position,1));
  	//gl_Position = vec4(pos.xy,0,1);
  	//vPosition = gl_Position.xy;
	vec3 texture1 = vec3(texture2D(uTexture1, Texcoord).rgb);
	Out.Texture1 = texture1;
	vec3 texture2 = vec3(texture2D(uTexture2, Texcoord).rgb);
	Out.Texture2 = texture2;
	Out.uTime = uTime;
}
