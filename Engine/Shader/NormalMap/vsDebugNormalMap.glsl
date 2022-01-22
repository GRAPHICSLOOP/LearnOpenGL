#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;


out VS_Out
{
	vec3 pos;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
	vec3 tangentNormal;
	mat3 TBN;
}vs_out;

uniform	sampler2D normalmap;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	vs_out.pos = vec3(modelMatrix * vec4(aPos,1.f));
	vs_out.normal = normalize(vec3(transpose(inverse(modelMatrix)) * vec4(aNormal,0.f))); // 世界空间
	vs_out.tangent = normalize(vec3(transpose(inverse(modelMatrix)) * vec4(aTangent,0.f))); // 世界空间
	vs_out.bitangent = normalize(vec3(transpose(inverse(modelMatrix)) * vec4(aBitangent,0.f))); // 世界空间
	vec3 T = vs_out.tangent; // 世界空间
	vec3 B = vs_out.bitangent; // 世界空间
	vec3 N = vs_out.normal; // 世界空间
	vs_out.TBN = mat3(T,B,N);

	vec3 objectNormal = texture(normalmap,aTextureCoord).rgb;
	objectNormal = normalize(objectNormal * 2.f - 1.f);
	vs_out.tangentNormal = objectNormal;

	gl_Position = vec4(1.f);
}
