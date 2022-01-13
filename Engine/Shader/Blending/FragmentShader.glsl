#version 330 core

in vec2 textureCoord;
out vec4 FragColor;

uniform sampler2D diffuse;
void main()
{
	vec4 textureColor = texture(diffuse,textureCoord);

	// 当透明度不满足时丢弃该fragment
	if(textureColor.a < 1.f)
		discard;

	FragColor = textureColor;
}