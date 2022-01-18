﻿#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoord;

uniform mat4 modelMatrix;
uniform mat4 lightViewMatrix;

void main()
{
	gl_Position = lightViewMatrix * modelMatrix * vec4(aPos,1.0f);
}
