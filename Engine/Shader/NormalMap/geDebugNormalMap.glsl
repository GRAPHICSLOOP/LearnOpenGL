#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 24) out;

in VS_Out
{
	vec3 pos;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
	vec3 tangentNormal;
	mat3 TBN;
}vs_in[];

out vec3 geColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

mat4 MVP = projectionMatrix * viewMatrix;

void GenerateLine(int index)
{

	geColor = vec3(0.0f,0.f,1.f);
    gl_Position = MVP * vec4(vs_in[index].pos,1.0f);
    EmitVertex();

    gl_Position = MVP * vec4(vs_in[index].pos + vs_in[index].normal * 0.5f,1.0f);
    EmitVertex();

    EndPrimitive();

	geColor = vec3(1.0f,0.f,0.f);
    gl_Position = MVP * vec4(vs_in[index].pos,1.0f);
    EmitVertex();

    gl_Position = MVP * vec4(vs_in[index].pos + vs_in[index].tangent * 0.5f,1.0f);
    EmitVertex();

    EndPrimitive();

	geColor = vec3(0.0f,1.f,0.f);
    gl_Position = MVP * vec4(vs_in[index].pos,1.0f);
    EmitVertex();

    gl_Position = MVP * vec4(vs_in[index].pos + vs_in[index].bitangent * 0.5f,1.0f);
    EmitVertex();

    EndPrimitive();

    geColor = vec3(1.0f,1.f,0.f);
    vec3 tangentNormal = normalize(vs_in[index].TBN * vs_in[index].tangentNormal);
    //vec3 tangentNormal = normalize(vec3(modelMatrix * vec4(vs_in[index].tangentNormal,1.f)));
    gl_Position = MVP * vec4(vs_in[index].pos,1.0f);
    EmitVertex();

    gl_Position = MVP * vec4(vs_in[index].pos + tangentNormal * 0.5f,1.0f);
    EmitVertex();

    EndPrimitive();
}

void main() 
{
	GenerateLine(0);
	GenerateLine(1);
	GenerateLine(2);
}