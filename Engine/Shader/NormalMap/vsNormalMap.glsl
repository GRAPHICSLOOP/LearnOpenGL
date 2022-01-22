#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;



out VS_Out
{
	vec2 UV;
	vec3 normal;
	vec3 fragPos;
	mat3 TBN;
}vs_out;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPos,1.0f);
	vs_out.UV = aTextureCoord;
	vs_out.fragPos = vec3(modelMatrix * vec4(aPos,1));
	//normal = aNormal; 对象空间
	vs_out.normal = (vec3(transpose(inverse(modelMatrix)) * vec4(aNormal,0.f))); // 世界空间
	vec3 T = normalize(vec3(transpose(inverse(modelMatrix)) * vec4(aTangent,0.f))); // 世界空间
	vec3 B = normalize(vec3(transpose(inverse(modelMatrix)) * vec4(aBitangent,0.f))); // 世界空间
	vec3 N = normalize(vec3(transpose(inverse(modelMatrix)) * vec4(aNormal,0.f))); // 世界空间
	vs_out.TBN = mat3(T,B,N);
}
