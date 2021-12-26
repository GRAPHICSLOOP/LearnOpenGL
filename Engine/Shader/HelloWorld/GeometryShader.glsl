#version 330 core
in vec4 vertexCol;
in vec2 texrtureCoord;
out vec4 FragColor;

uniform sampler2D texture2d;
void main()
{
	FragColor = texture(texture2d,texrtureCoord);
	//FragColor = vertexCol;
}