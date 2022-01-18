#version 330 core
out vec4 FragColor;
in vec2 textureCoord;

uniform sampler2D depthTexture;

void main()
{
	float depthValue = texture(depthTexture,textureCoord).r;
	FragColor = vec4(vec3(depthValue),1.f);
}