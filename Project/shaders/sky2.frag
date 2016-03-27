#version 410 core

#define POSITION	0
#define NORMAL		1
#define TEXCOORD	2
#define FRAG_COLOR	0

precision highp int;

uniform sampler2D uTexture1;
uniform sampler2D uTexture2;
uniform float uTime;

vec3 pointLight( in vec3 p, in vec3 n, in vec3 v, in vec3 diffuseColor, in vec3 specularColor, in float specularPower, vec3 lightPosition, float lightIntensity, vec3 lightColor)
{
	vec3 l = normalize(lightPosition - p);
	float ndotl = max(dot(n, l), 0.0);
	vec3 h = normalize(l+v);
	float ndoth = max(dot(n, h), 0.0);
	float d = distance(lightPosition, p);
	float att;
	int Attenuation = 0;
	if (Attenuation == 0)
		att = 1.f;
	else if (Attenuation == 1)
		att = 1.f / (d);
	else if (Attenuation == 2)
		att = 1.f / (d*d);
        else if (Attenuation == 3)
		att = 1.f / (d*d*d);
	return att * lightColor * lightIntensity * (diffuseColor * ndotl + specularColor * pow(ndoth, specularPower));
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
	vec3 n = vec3(0,-1,0);
	vec3 p = vec3(0,0,0);
	vec3 v = -n;
	vec3 diffuseColor = vec3(1.f, 1.f, 1.f);
	vec3 specularColor = vec3(1.f, 1.f, 1.f);
	float specularPower = 1.0;
	vec3 lightPosition = vec3(0.f, 1.5f, 0.f);
	float lightIntensity = 1.0f;
	vec3 lightColor = vec3(1.f, 1.f, 1.f);
	vec3 light = pointLight(p, n, v, diffuseColor, specularColor, specularPower, lightPosition, lightIntensity, lightColor);

	//vec3 color = vec3(texture2D(uTexture1, In.Texcoord).rgb)*(1-uTime);// + vec3(texture2D(uTexture2, In.Texcoord).rgb)*(uTime);
	vec3 color = vec3(0.4f, 0.6f, 1.0f)*(1-uTime) + vec3(texture2D(uTexture2, In.Texcoord).rgb)*(uTime) +light;
	FragColor = vec4(color, 1.0);
        //FragColor = vec4(In.Position, 1.0);

}

