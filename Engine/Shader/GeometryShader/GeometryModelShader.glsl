#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_Out{
    vec3 normal;
    vec2 texCoord;
} gs_in[];

uniform float time;
out vec2 texCoord;

vec3 getNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[1].gl_Position - gl_in[2].gl_Position);
    return normalize(cross(b,a));
}

vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0;
    vec3 pos= normal*(sin(time) + 1)/2 * magnitude;
    return position + vec4(pos,0.f);
};

void main() {    
    vec3 normal = getNormal();
    gl_Position = explode(gl_in[0].gl_Position,normal);
    texCoord = gs_in[0].texCoord;
    EmitVertex(); 
    gl_Position = explode(gl_in[1].gl_Position,normal);
    texCoord = gs_in[1].texCoord;
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position,normal);
    texCoord = gs_in[2].texCoord;
    EmitVertex();

    EndPrimitive();
}