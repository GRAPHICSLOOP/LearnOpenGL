#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aOffset;

out vec3 color;

void main()
{
	vec2 of = aPos + aOffset;
	gl_Position = vec4(of,0.0f,1.0f);
	color = aColor;
}
