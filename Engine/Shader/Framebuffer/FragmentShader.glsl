#version 330 core

in vec2 textureCoord;
out vec4 FragColor;

uniform sampler2D diffuse;
void main()
{
	vec4 textureColor = texture(diffuse,textureCoord);
	float average = 0.2126 * textureColor.r + 0.7152 * textureColor.g + 0.0722 * textureColor.b;
	FragColor = vec4(vec3(average),1.0f);
}