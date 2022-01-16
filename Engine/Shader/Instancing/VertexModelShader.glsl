﻿#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextureCoord;
layout (location = 3) in mat4 aModelMatrixInstance;

out vec2 texrtureCoord;
out vec3 normal;
out vec3 fragPos;

//uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


void main()
{
	gl_Position = projectionMatrix * viewMatrix * aModelMatrixInstance * vec4(aPos,1.0f);
	texrtureCoord = aTextureCoord;
	fragPos = vec3(aModelMatrixInstance * vec4(aPos,1));
	//normal = aNormal; 对象空间
	normal = vec3(transpose(inverse(aModelMatrixInstance)) * vec4(aNormal,0.f)); // 世界空间
}
