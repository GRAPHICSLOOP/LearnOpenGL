#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoord;

out vec2 texrtureCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out VS_Out{
	vec3 normal;
	vec2 texCoord;
}vs_out;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPos,1.0f);
	texrtureCoord = aTextureCoord;
	//normal = aNormal; 对象空间
	vs_out.normal = vec3(transpose(inverse(modelMatrix)) * vec4(aNormal,0.f)); // 世界空间
	vs_out.texCoord = texrtureCoord;
}