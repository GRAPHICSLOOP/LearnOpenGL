#version 330 core

out vec4 FragColor;

in VS_Out
{
	vec2 texCoord;
	vec3 normal;
	vec3 fragPos;
	vec4 lightViewPos;
} vs_in;

struct Material
{
	sampler2D diffuse;
	float shininess;
};

struct DirLight
{
	vec3 direction;
	vec3 ambient;
	vec3 lightColor;
};

uniform sampler2D depthMap;
uniform Material _Material;
uniform DirLight _Light;
uniform vec3 _ViewPos;



float CalcShadow()
{
	float shadow = 1.f;

	vec3 lightSpacePos = vs_in.lightViewPos.xyz / vs_in.lightViewPos.w;
	lightSpacePos = lightSpacePos * 0.5 + 0.5;

	float depth = texture(depthMap,lightSpacePos.xy).r;

	float bias = 0.005;
	shadow = depth < lightSpacePos.z - bias ? 1.f : 0.f;

	return shadow;
};

vec3 CalcDirLight(vec3 norm,vec3 viewDir,vec3 objectColor,vec3 specularColor)
{
	float shadow = CalcShadow();

	// 环境光
	vec3 ambient = _Light.ambient * vec3(objectColor);

	// 漫反射
	vec3 lightDir = -_Light.direction;
	float diff = max(dot(norm,lightDir),0.0f);
	vec3 diffuse = diff * objectColor;

	// 镜面光
	vec3 halfwayDir = normalize(lightDir + viewDir); 
	//vec3 reflectDir = normalize(reflect(-lightDir,norm));
	float spec = pow(max(dot(viewDir,halfwayDir),0),_Material.shininess);
	vec3 specular = spec * specularColor;

	vec3 result = ambient + _Light.lightColor * (diffuse) * (1 - shadow); 

	return result;
};


void main()
{
	vec3 objectColor = texture(_Material.diffuse,vs_in.texCoord).rgb;
	
	vec3 viewDir = normalize(_ViewPos - vs_in.fragPos);
	vec3 finalColor = CalcDirLight(normalize(vs_in.normal),viewDir,objectColor,vec3(1.f,1.0f,1.0f));

    FragColor = vec4(finalColor,1.0f);
}