#version 330 core

in vec2 textureCoord;
out vec4 FragColor;

uniform sampler2D diffuse;
void main()
{
	vec4 textureColor = texture(diffuse,textureCoord);

	FragColor = textureColor;
}