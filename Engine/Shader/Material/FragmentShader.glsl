#version 330 core
in vec2 texrtureCoord;
in vec3 normal;
in vec3 fragPos;
out vec4 FragColor;

 
uniform vec3 viewPos;
uniform sampler2D texture1;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	vec3 ambient;
    float shininess;
};

struct Light
{
	vec3 lightColor;
	vec3 lightPos;
};

uniform Material material;
uniform Light light;


struct DirLight
{
	vec3 direction;

	vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


uniform DirLight dirLight;

void main()
{
	vec2 uv = texrtureCoord;

	vec4 color1 = texture(material.diffuse,uv);
	vec4 color2 = texture(texture1,uv);
	vec4 objectColor = mix(color1,color2,0.05f) * vec4(light.lightColor,1.f);

	// 环境光
	vec3 ambient = material.ambient * vec3(objectColor);

	// 漫反射
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.lightPos - fragPos);
	float diff = max(dot(norm,lightDir),0.0f);
	vec3 diffuse = diff * vec3(objectColor);

	// 镜面光
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = normalize(reflect(-lightDir,normal));
	float spec = pow(max(dot(viewDir,reflectDir),0),material.shininess);
	vec3 specular = spec * vec3(texture(material.specular,uv));

	FragColor = vec4(light.lightColor *(diffuse + ambient + specular),1.f); 
}