#version 410 core

#define POSITION	0
#define NORMAL		1
#define TEXCOORD	2
#define FRAG_COLOR	0

precision highp int;

uniform sampler2D uTexture;
uniform float uTime;
uniform vec3 uCameraPosition;
uniform vec3 uCameraNormal;
uniform vec4 uLightPosition;


vec3 pointLight( in vec3 p, in vec3 n, in vec3 v, in vec3 diffuseColor, in vec3 specularColor, in float specularPower)
{
	vec3 color = vec3(1.f, 1.f, 1.f);
	float intensity = 1.f;
	vec3 outColor = vec3(0);
	vec3 l = normalize(p  - uCameraPosition);
	float ndotl =  max(dot(n, l), 0.0);
	vec3 h = normalize(l+v);
	float ndoth = max(dot(n, h), 0.0);
	float d = distance(p, uCameraPosition);
	float att = 1.f / (d*d);
	outColor +=  vec3(1.f, 1.f, 1.f) + att *color * intensity * (diffuseColor * ndotl + specularColor * pow(ndoth, specularPower));
	//return outColor;
	return vec3(0.f, 0.f, 0.f);
}


layout(location = FRAG_COLOR, index = 0) out vec4 FragColor;

in block
{
	vec3 Position;
	vec2 Texcoord;
} In;

vec3 L;

void main()
{
	vec3 p = vec3(uLightPosition.xyz);
	vec3 n = normalize(uCameraNormal);
	if (!gl_FrontFacing)
		n = -n;
	vec3 v = normalize(-uCameraPosition);
	vec3 diffuseColor = texture(uTexture, In.Texcoord).rgb;
	vec3 specularColor = texture(uTexture, In.Texcoord).rrr;
	float specularPower = 30.0f;

	float t;
	if (uTime > 0.8)
		t = 0.8;
	else
		t = uTime;

	vec3 color = vec3(texture2D(uTexture, In.Texcoord).rgb)*(1-t);

	FragColor = vec4(color + pointLight(p, n, v, diffuseColor, specularColor, specularPower), 1.0);


		
	
	//FragColor = vec4(color, 1.0);
        //FragColor = vec4(In.Position, 1.0);

}

