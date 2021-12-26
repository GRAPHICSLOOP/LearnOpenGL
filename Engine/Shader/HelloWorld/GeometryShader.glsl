#version 330 core
in vec4 vertexCol;
in vec2 texrtureCoord;
out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
void main()
{
	vec2 uv = texrtureCoord;
	vec4 Color1 = texture(texture1,uv);
	vec4 Color2 = texture(texture2,uv);
	FragColor = mix(Color1,Color2,0.2f);
	//FragColor = vertexCol;
}