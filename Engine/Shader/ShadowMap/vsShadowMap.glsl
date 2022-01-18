#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 lightViewMatrix;

out VS_Out{
	vec2 texCoord;
	vec3 normal;
	vec3 fragPos;
	vec4 lightViewPos;
}vs_out;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPos,1.0f);
	vs_out.texCoord = aTextureCoord;
	vs_out.fragPos = vec3(modelMatrix * vec4(aPos,1));
	//normal = aNormal; 对象空间
	vs_out.normal = vec3(transpose(inverse(modelMatrix)) * vec4(aNormal,0.f)); // 世界空间
	vs_out.lightViewPos = lightViewMatrix * vec4(vs_out.fragPos,1.f);
}
