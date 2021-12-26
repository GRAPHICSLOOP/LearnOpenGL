#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec2 aTextureCoord;

out vec4 vertexCol;
out vec2 texrtureCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPos,1.0f);
	vertexCol = vec4(aCol,1);
	texrtureCoord = aTextureCoord;
}
