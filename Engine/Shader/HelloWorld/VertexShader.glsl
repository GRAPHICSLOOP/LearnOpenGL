#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec2 aTextureCoord;

out vec4 vertexCol;
out vec2 texrtureCoord;
void main()
{
	gl_Position = vec4(aPos.x,aPos.y,aPos.z,1);
	vertexCol = vec4(aCol,1);
	texrtureCoord = aTextureCoord;
}
