#version 330 core
in vec2 texrtureCoord;
in vec3 normal;
in vec3 fragPos;
out vec4 FragColor;

 
uniform vec3 viewPos;

struct Material
{
	sampler2D diffuse0;
	sampler2D specular0;
	vec3 ambient;
    float shininess;
};

struct DirLight
{
	vec3 direction;

	vec3 lightColor;
};

struct PointLight
{
	vec3 position;
	vec3 lightColor;

	float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	vec3 lightColor;

	float cutOff;
};

uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;


vec3 CalcDirLight(DirLight light,vec3 norm,vec3 viewDir,vec3 objectColor,vec3 specularColor);
vec3 CalcPointLight(PointLight light,vec3 norm,vec3 viewDir,vec3 objectColor,vec3 specularColor);
vec3 CalcSpotLight(SpotLight light,vec3 norm,vec3 viewDir,vec3 objectColor,vec3 specularColor);

void main()
{
	vec2 uv = texrtureCoord;
	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(viewPos - fragPos);

	vec4 color1 = texture(material.diffuse0,uv);
	vec3 objectColor = vec3(color1);

	//vec4 dirColor = vec4(CalcDirLight(dirLight,norm,viewDir,objectColor,vec3(texture(material.specular0,uv))) ,1.f);
	vec4 dirColor = vec4(CalcPointLight(pointLight,norm,viewDir,objectColor,vec3(texture(material.specular0,uv))) ,1.f);
	//vec4 dirColor = vec4(CalcSpotLight(spotLight,norm,viewDir,objectColor,vec3(texture(material.specular0,uv))) ,1.f);
	FragColor = dirColor;
}


vec3 CalcDirLight(DirLight light,vec3 norm,vec3 viewDir,vec3 objectColor,vec3 specularColor)
{
	// 环境光
	vec3 ambient = material.ambient * vec3(objectColor);

	// 漫反射
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(norm,lightDir),0.0f);
	vec3 diffuse = diff * objectColor;

	// 镜面光
	vec3 reflectDir = normalize(reflect(-lightDir,norm));
	float spec = pow(max(dot(viewDir,reflectDir),0),material.shininess);
	vec3 specular = spec * specularColor;

	vec3 result = light.lightColor *(diffuse + ambient + specular); 

	return result;
};

vec3 CalcPointLight(PointLight light,vec3 norm,vec3 viewDir,vec3 objectColor,vec3 specularColor)
{
	// 环境光
	vec3 ambient = material.ambient * vec3(objectColor);

	// 漫反射
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(norm,lightDir),0.0f);
	vec3 diffuse = diff * objectColor;

	// 镜面光
	vec3 reflectDir = normalize(reflect(-lightDir,norm));
	float spec = pow(max(dot(viewDir,reflectDir),0),material.shininess);
	vec3 specular = spec * specularColor;

	// 衰减
	float dist = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	vec3 result = light.lightColor *(diffuse + ambient + specular) * attenuation; 

	return result;
};

vec3 CalcSpotLight(SpotLight light,vec3 norm,vec3 viewDir,vec3 objectColor,vec3 specularColor)
{
	// frag到灯光的方向
	vec3 lightDir = normalize(light.position - fragPos);

	// 环境光
	vec3 ambient = material.ambient * vec3(objectColor);

	// 检查当前frag缩放在圆锥内
	if(dot(-lightDir,light.direction) < light.cutOff)
		return ambient;

	// 漫反射
	float diff = max(dot(norm,lightDir),0.0f);
	vec3 diffuse = diff * objectColor;

	// 镜面光
	vec3 reflectDir = normalize(reflect(-lightDir,norm));
	float spec = pow(max(dot(viewDir,reflectDir),0),material.shininess);
	vec3 specular = spec * specularColor;

	vec3 result = light.lightColor *(diffuse + ambient + specular); 

	return result;
};