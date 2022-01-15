#version 330 core
in vec2 texCoord;
in vec2 texrtureCoord;
in vec3 normal;
in vec3 fragPos;
out vec4 FragColor;

struct Material
{
	sampler2D diffuse0;

};

uniform Material material;

void main()
{
	vec4 color1 = texture(material.diffuse0,texrtureCoord);
    FragColor = color1;
}