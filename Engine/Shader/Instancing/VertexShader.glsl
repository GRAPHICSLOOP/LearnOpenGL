#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out vec3 color;

uniform vec2 offset[100];

void main()
{
	vec2 of = aPos + offset[gl_InstanceID];
	gl_Position = vec4(of,0.0f,1.0f);
	color = aColor;
}
