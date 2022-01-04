#version 330 core
in vec2 texrtureCoord;
in vec3 normal;
in vec3 fragPos;
out vec4 FragColor;

 
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	vec2 uv = texrtureCoord;

	vec4 Color1 = texture(texture1,uv);
	vec4 Color2 = texture(texture2,uv);
	vec4 objectColor = mix(Color1,Color2,0.2f) * vec4(lightColor,1.f);

	vec4 ambient = vec4(0.1f);// 环境光

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm,lightDir),0.0f);// 漫反射

	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = normalize(reflect(-lightDir,normal));
	float spec = pow(max(dot(viewDir,reflectDir),0),32);// 高光

	FragColor = objectColor *(diff + ambient + spec);
}