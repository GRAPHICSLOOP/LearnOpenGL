#version 330 core
in vec3 geColor;
out vec4 FragColor;

void main()
{
	FragColor = vec4(geColor,1.f);
}
