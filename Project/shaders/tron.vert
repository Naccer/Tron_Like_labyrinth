#version 410 core

#define POSITION	0
#define NORMAL		1
#define TEXCOORD	2
#define FRAG_COLOR	0

precision highp float;
precision highp int;

uniform mat4 MVP;
uniform mat4 MV;
uniform float Time;
uniform int InstanceCount;


const float PI = 3.14159265359;
const float TWOPI = 6.28318530718;
const float PI_2 = 1.57079632679;
const float SQUARE_SIZE = 10.0;

//layout(std140, column_major) uniform;

layout(location = POSITION) in vec3 Position;
layout(location = NORMAL) in vec3 Normal;
layout(location = TEXCOORD) in vec2 Texcoord;

out block
{
	flat int InstanceId;
	vec2 Texcoord;
	vec3 CameraSpacePosition;
	vec3 CameraSpaceNormal;
} Out;

void main()
{	
	Out.Texcoord = Texcoord;
	vec3 p = Position;
	vec3 n = Normal;

	Out.CameraSpacePosition = vec3(MV * vec4(p, 1.0));
	Out.CameraSpaceNormal = vec3(MV * vec4(n, 0.0));
	Out.InstanceId = gl_InstanceID;
	gl_Position = MVP * vec4(p, 1.0);
}
