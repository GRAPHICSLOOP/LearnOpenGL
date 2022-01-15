#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_Out{
    vec3 normal;
    vec2 texCoord;
} gs_in[];

float normalLength = 0.1f;

void GenerateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();

    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal * normalLength,0.f);
    EmitVertex();

    EndPrimitive();
}

void main() {    
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}