#version 330 core
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{           
    float sampled = texture(text,texCoords).r;
    FragColor = vec4(textColor, sampled);
    //FragColor = vec4(1.f,1.f,1.f,1.f);
}