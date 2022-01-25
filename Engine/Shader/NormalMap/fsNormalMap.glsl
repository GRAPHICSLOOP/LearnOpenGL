#version 330 core
out vec4 FragColor;

in VS_Out
{
	vec2 UV;
	vec3 normal;
	vec3 fragPos;
	mat3 TBN;
}vs_in;
 
struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normalmap;
	float shininess;
};

struct Light
{
	vec3 position;
	float ambient;
	vec3 lightColor;
};

uniform vec3 _ViewPos;
uniform float _PixelSize;
uniform Material _Material;
uniform Light _Light;

uniform mat4 modelMatrix;

vec3 CalcPointLight(vec3 norm,vec3 viewDir,vec3 objectColor,vec3 specularColor);

void main()
{
	vec3 viewDir = normalize(_ViewPos - vs_in.fragPos);

	vec3 objectColor = texture(_Material.diffuse,vs_in.UV).rgb;
	vec3 objectNormal = texture(_Material.normalmap,vs_in.UV).rgb;
	objectNormal = normalize(objectNormal * 2.f - 1.f);
	objectNormal = normalize(vs_in.TBN * objectNormal);
	vec3 objectSpecular = vec3(0.4f);

	//objectNormal = normalize(vec3(transpose(inverse(modelMatrix)) * vec4(objectNormal,1.f))); // 世界空间


	vec3 dirColor = CalcPointLight(objectNormal,viewDir,objectColor,objectSpecular);
	FragColor = vec4(dirColor,1.f);
}


vec3 CalcPointLight(vec3 norm,vec3 viewDir,vec3 objectColor,vec3 specularColor)
{
	// 环境光
	vec3 ambient = _Light.ambient * vec3(objectColor);

	// 漫反射
	vec3 lightDir = normalize(_Light.position - vs_in.fragPos);
	float diff = max(dot(norm,lightDir),0.0f); 
	vec3 diffuse = diff * objectColor;

	// 镜面光
	vec3 halfVector = normalize(viewDir + lightDir);
	//vec3 reflectDir = normalize(reflect(-lightDir,norm));
	float spec = pow(max(dot(norm,halfVector),0),_Material.shininess);
	vec3 specular = spec * specularColor;

	vec3 result = ambient + _Light.lightColor *(diffuse + specular); 

	return result;
};
