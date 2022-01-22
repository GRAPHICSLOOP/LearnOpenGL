#version 330 core
out vec4 FragColor;

in VS_Out
{
	vec2 UV;
	vec3 normal;
	vec3 fragPos;
	vec4 lightViewPos;
}vs_in;
 
struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct Light
{
	vec3 direction;
	float ambient;
	vec3 lightColor;
};

uniform sampler2D _DepthMap;
uniform vec3 _ViewPos;
uniform float _PixelSize;
uniform Material _Material;
uniform Light _Light;

vec3 CalcDirLight(vec3 norm,vec3 viewDir,vec3 objectColor,vec3 specularColor);

float ShadowCalculation(vec3 norm);

void main()
{
	vec3 norm = normalize(vs_in.normal);
	vec3 viewDir = normalize(_ViewPos - vs_in.fragPos);

	vec3 objectColor = texture(_Material.diffuse,vs_in.UV).rgb;
	vec3 objectSpecular = texture(_Material.specular,vs_in.UV).rgb;

	vec3 dirColor = CalcDirLight(norm,viewDir,objectColor,objectSpecular);
	FragColor = vec4(dirColor,1.f);
}


vec3 CalcDirLight(vec3 norm,vec3 viewDir,vec3 objectColor,vec3 specularColor)
{
	float shadow = ShadowCalculation(norm);
	
	// 环境光
	vec3 ambient = _Light.ambient * vec3(objectColor);

	// 漫反射
	vec3 lightDir = -_Light.direction; // 默认在CPU侧normalize
	float diff = max(dot(norm,lightDir),0.0f);
	vec3 diffuse = diff * objectColor;

	// 镜面光
	vec3 halfVector = normalize(viewDir + lightDir);
	//vec3 reflectDir = normalize(reflect(-lightDir,norm));
	float spec = pow(max(dot(viewDir,halfVector),0),_Material.shininess);
	vec3 specular = spec * specularColor;

	vec3 result = ambient + _Light.lightColor *(diffuse + specular) * (1 - shadow); 

	return result;
};

float ShadowCalculation(vec3 norm)
{
	float shadow = 0.0f;
	float bias = max((1.f - dot(-_Light.direction,norm)) *_PixelSize,0.0001);// 确保光线和法线平行时，要减去一个误差
	// 此时得到还只是-1到1的标准设备区 我们需要转换到0-1
	vec3 lightViewMapPos = vs_in.lightViewPos.xyz / vs_in.lightViewPos.w;
	lightViewMapPos = lightViewMapPos * 0.5f + 0.5f;

	vec2 texelSize = 1.0 / vec2(800.f,600.f);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(_DepthMap, lightViewMapPos.xy + vec2(x, y) * texelSize).r; 
			float currentDepth = lightViewMapPos.z - bias;
			shadow +=  currentDepth < pcfDepth ? 0.f : 1.f;// 当前深度值小于_DepthMap说明不在阴影中     
		}    
	}
	shadow /= 9.0;

	return shadow;
}